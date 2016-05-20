#ifndef _ez_lang_h_
#define _ez_lang_h_

#include <stdbool.h>
#include "vector.h"

#define IDENTIFIER_SIZE             32
#define INSTRUCTIONS_SIZE           128
#define ELSIF_SIZE                  32

typedef struct expression expression_t;

typedef struct identifier {
    char value[IDENTIFIER_SIZE];
} identifier_t;

bool identifier_is_reserved(const identifier_t *id);

typedef struct parameters {
    vector_t      parameters;    /* of expression_t* */
} parameters_t;

void parameters_init(parameters_t* params);

void parameters_wipe(parameters_t* params);

void parameters_print(FILE* output, const parameters_t* params);

typedef struct valref {
    identifier_t  identifier;

    bool             has_indexing;
    vector_t         indexings;     /* of expression_t* */

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

void expression_print(FILE* output, const expression_t* expr);

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
    vector_t members;   /* of symbol_t* */
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
void type_print(type_t* t);

type_t *type_boolean_new();
type_t *type_integer_new();
type_t *type_natural_new();
type_t *type_real_new();
type_t *type_char_new();
type_t *type_string_new();
type_t *type_vector_new(type_t* of);
type_t *type_structure_new(structure_t* s);

symbol_t *symbol_new(const identifier_t *identifier, type_t *is);
void symbol_delete(symbol_t *symbol);
void symbol_print(int i, void *s);

structure_t *structure_new(const identifier_t* identifier);
void structure_add_member(structure_t* structure, symbol_t* member);
void structure_delete(structure_t* structure);
void structure_print(int i, void* v);

typedef struct instruction instruction_t;

typedef struct elsif_instr {
    expression_t*  coundition;
    vector_t instructions;  /* of instruction_t* */
} elsif_instr_t;

elsif_instr_t* elsif_instr_new(expression_t* coundition);

void elsif_instr_delete(elsif_instr_t* elsif);

typedef struct if_instr {
    expression_t* coundition;
    vector_t instructions;      /* of instruction_t* */
    vector_t elsifs;            /* of elsif_instr_t* */
    vector_t else_instrs;       /* of instruction_t* */
} if_instr_t;

if_instr_t* if_instr_new(expression_t* coundition);

void if_instr_delete(if_instr_t* if_instr);

typedef struct loop_instr {
    expression_t* coundition;
    vector_t      instructions;     /* of instruction_t* */
} loop_instr_t;

loop_instr_t* loop_instr_new(expression_t* coundition);

void loop_instr_delete(loop_instr_t* loop);

typedef struct while_instr {
    expression_t* coundition;
    vector_t      instructions;     /* of instruction_t* */
} while_instr_t;

while_instr_t* while_instr_new(expression_t* coundition);

void while_instr_delete(while_instr_t* loop);

typedef enum {
    INSTRUCTION_TYPE_PRINT,
    INSTRUCTION_TYPE_READ,
    INSTRUCTION_TYPE_RETURN,
    INSTRUCTION_TYPE_FLOWCONTROL,
    INSTRUCTION_TYPE_EXPRESSION,
} instruction_type_t;



typedef struct context {
    identifier_t identifier;
    // TODO : constants
    vector_t globals; /* of symbol_t* */
    vector_t locals; /* of symbol_t* */
    vector_t structures; /* of_structure_t* */
    // TODO : functions
    // TODO : procedures
    // TODO : args

    struct context *parent_context;
} context_t;

context_t *context_new(identifier_t *identifier, context_t *parent_context);
void context_delete(context_t* ctx);
void context_print(context_t* ctx);

// TODO : add constant;
void context_add_global(context_t *program, symbol_t *global);
void context_add_structure(context_t *program, structure_t *structure);
// TODO : add function
// TODO : add procedure
// TODO : add arg
void context_add_local(context_t *function, symbol_t *local);

structure_t *context_find_structure(const context_t *ctx,
                                    const identifier_t *identifier);

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
