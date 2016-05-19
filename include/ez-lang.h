#ifndef _ez_lang_h_
#define _ez_lang_h_

#include <stdbool.h>

#define IDENTIFIER_SIZE             32
#define VALREF_ARRAY_INDEXING_MAX   16
#define PARAMETERS_SIZE             32

typedef struct expression expression_t;

typedef struct identifier {
    char value[IDENTIFIER_SIZE];
} identifier_t;

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

#endif

