#ifndef _ez_lang_h_
#define _ez_lang_h_

#include <stdint.h>
#include <stdbool.h>

#define IDENTIFIER_SIZE             32
#define VALREF_ARRAY_INDEXING_MAX   16
#define PARAMETERS_SIZE             32
#define STRUCT_SIZE                 16

typedef struct identifier {
    char value[IDENTIFIER_SIZE];
} identifier_t;


typedef struct array_indexing {
    // expression_t expression;
} array_indexing_t;

typedef struct parameters {
    unsigned int  nparameters;
    // expression_t* parameters[PARAMETERS_SIZE];
} parameters_t;

typedef struct valref {
    identifier_t  identifier;

    bool             has_indexing;
    unsigned int     nindexings;
    array_indexing_t indexings[VALREF_ARRAY_INDEXING_MAX];

    bool         is_funccall;
    parameters_t parameters;

    struct valref* next;
} valref_t;

valref_t* valref_new(const identifier_t* identifier);

void valref_delete(valref_t* valref);

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


typedef enum {
    TYPE_TYPE_BOOLEAN,
    TYPE_TYPE_INTEGER,
    TYPE_TYPE_NATURAL,
    TYPE_TYPE_REAL,
    TYPE_TYPE_CHAR,
    TYPE_TYPE_STRING,
    TYPE_TYPE_VECTOR,
    TYPE_TYPE_STRUCTURE
} type_types;

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
    uint8_t number_of_members;
};

struct type {
    type_types type;
    union {
        structure_t *structure_type;
        type_t *vector_type;
    };
};

type_t *type_new(type_types type);
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

#endif
