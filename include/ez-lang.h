#ifndef _ez_lang_h_
#define _ez_lang_h_

#include <stdbool.h>

#define IDENTIFIER_SIZE             32
#define VALREF_ARRAY_INDEXING_MAX   16
#define PARAMETERS_SIZE             32

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

#endif

