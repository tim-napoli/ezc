#ifndef _ez_lang_h_
#define _ez_lang_h_

#include <stdbool.h>

#define IDENTIFIER_SIZE             32
#define MAX_GLOBALS                 16
#define MAX_STRUCTURES              16
#define MAX_LOCALS                  16
#define VALREF_ARRAY_INDEXING_MAX   16
#define PARAMETERS_SIZE             32
#define STRUCT_SIZE                 16

typedef struct expression expression_t;

typedef struct identifier {
    char value[IDENTIFIER_SIZE];
} identifier_t;

bool identifier_is_reserved(const identifier_t *id);

typedef struct parameters {
    unsigned int  nparameters;
    expression_t* parameters[PARAMETERS_SIZE];
} parameters_t;

void parameters_print(FILE* output, const parameters_t* params);

typedef struct valref {
    identifier_t  identifier;

    bool             has_indexing;
    unsigned int     nindexings;
    expression_t*    indexings[VALREF_ARRAY_INDEXING_MAX];

    bool         is_funccall;
    parameters_t parameters;

    struct valref* next;
} valref_t;

valref_t* valref_new(const identifier_t* identifier);

void valref_delete(valref_t* valref);

void valref_print(FILE* output, const valref_t* value);

typedef enum {
    VALUE_TYPE_STRING,
    VALUE_TYPE_REAL,
    VALUE_TYPE_INTEGER,
    VALUE_TYPE_NATURAL,
    VALUE_TYPE_BOOLEAN,
    VALUE_TYPE_VALREF,
} value_type_t;

typedef struct value {
    value_type_t type;
    union {
        char*        string;
        double       real;
        int          integer;
        unsigned int natural;
        bool         boolean;
        valref_t*    valref;
    };
} value_t;

void value_wipe(value_t* value);

void value_print(FILE* output, const value_t* value);

typedef enum {
    EXPRESSION_TYPE_VALUE,

    EXPRESSION_TYPE_CMP_OP_EQUALS,
    EXPRESSION_TYPE_CMP_OP_DIFFERENT,
    EXPRESSION_TYPE_CMP_OP_LOWER_OR_EQUALS,
    EXPRESSION_TYPE_CMP_OP_GREATER_OR_EQUALS,
    EXPRESSION_TYPE_CMP_OP_LOWER,
    EXPRESSION_TYPE_CMP_OP_GREATER,

    EXPRESSION_TYPE_BOOL_OP_AND,
    EXPRESSION_TYPE_BOOL_OP_OR,
    EXPRESSION_TYPE_BOOL_OP_NOT,

    EXPRESSION_TYPE_ARITHMETIC_OP_PLUS,
    EXPRESSION_TYPE_ARITHMETIC_OP_MINUS,
    EXPRESSION_TYPE_ARITHMETIC_OP_MUL,
    EXPRESSION_TYPE_ARITHMETIC_OP_DIV,
    EXPRESSION_TYPE_ARITHMETIC_OP_MOD,

    EXPRESSION_TYPE_SIZE
} expression_type_t;

extern int expression_type_predecence[EXPRESSION_TYPE_SIZE];

struct expression {
    expression_type_t type;
    union {
        value_t value;
        struct {
            struct expression *left, *right;
        };
    };
};

expression_t* expression_new(expression_type_t type);

void expression_delete(expression_t* expr);

int expression_predecence(const expression_t* expr);


typedef enum {
    TYPE_TYPE_BOOLEAN,
    TYPE_TYPE_INTEGER,
    TYPE_TYPE_NATURAL,
    TYPE_TYPE_REAL,
    TYPE_TYPE_CHAR,
    TYPE_TYPE_STRING,
    TYPE_TYPE_VECTOR,
    TYPE_TYPE_STRUCTURE
} type_type_t;

typedef struct symbol symbol_t;
typedef struct structure structure_t;
typedef struct type type_t;

struct symbol {
    identifier_t identifier;
    type_t *is;
};

struct structure {
    identifier_t identifier;
    symbol_t *members[STRUCT_SIZE];
    unsigned int nmembers;
};

struct type {
    type_type_t type;
    union {
        structure_t *structure_type;
        type_t *vector_type;
    };
};

type_t *type_new(type_type_t type);
void type_delete(type_t *type);
type_t *type_boolean_new();
type_t *type_integer_new();
type_t *type_natural_new();
type_t *type_real_new();
type_t *type_char_new();
type_t *type_string_new();
type_t *type_vector_new(type_t *of);

symbol_t *symbol_new(const identifier_t *identifier, type_t *is);
void symbol_delete(symbol_t *symbol);

structure_t *structure_new(const identifier_t *identifier);
void structure_add_member(structure_t *structure, symbol_t *member);
void structure_delete(structure_t *structure);

typedef struct context {
    identifier_t identifier;
    // TODO : constants
    symbol_t *globals[MAX_GLOBALS];
    symbol_t *locals[MAX_LOCALS];
    structure_t *structures[MAX_STRUCTURES];
    // TODO : functions
    // TODO : procedures
    // TODO : args

    unsigned int nconstants;
    unsigned int nglobals;
    unsigned int nstructures;
    unsigned int nfunctions;
    unsigned int nprocedures;
    unsigned int nargs;
    unsigned int nlocals;

    struct context *global_context;
} context_t;

context_t *context_new(identifier_t *identifier, context_t *global_context);
// TODO : add constant;
void context_add_global(context_t *program, symbol_t *global);
void context_add_structure(context_t *program, structure_t *structure);
// TODO : add function
// TODO : add procedure
// TODO : add arg
void context_add_local(context_t *function, symbol_t *local);

bool context_structure_exists(identifier_t identifier);
bool context_valref_exists(valref_t *valref);
bool context_identifier_is_function(identifier_t identifier);
bool context_identifier_is_procedure(identifier_t identifier);

typedef enum {
    MODIFIER_IN,
    MODIFIER_OUT,
    MODIFIER_INOUT
} type_modifier_t;

typedef struct argument {
    type_modifier_t modifier;
    symbol_t *symbol;
} argument_t;

#endif
