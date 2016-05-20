#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ez-lang.h"

type_t* type_new(type_type_t type) {
    type_t* t = malloc(sizeof(type_t));

    if (!t) {
        fprintf(stderr, "couldn't allocate type\n");
        return NULL;
    }

    memset(t, 0, sizeof(type_t));
    t->type = type;

    return t;
}

void type_delete(type_t *t) {
    if (t->type == TYPE_TYPE_STRUCTURE) {
        structure_delete(t->structure_type);
    } else
    if (t->type == TYPE_TYPE_VECTOR) {
        type_delete(t->vector_type);
    }

    free(t);
}

type_t *type_boolean_new() {
    return type_new(TYPE_TYPE_BOOLEAN);
}

type_t *type_integer_new() {
    return type_new(TYPE_TYPE_INTEGER);
}

type_t *type_natural_new() {
    return type_new(TYPE_TYPE_NATURAL);
}

type_t *type_real_new() {
    return type_new(TYPE_TYPE_REAL);
}
type_t *type_char_new() {
    return type_new(TYPE_TYPE_CHAR);
}

type_t *type_string_new() {
    return type_new(TYPE_TYPE_STRING);
}

type_t *type_vector_new(type_t *of) {
    type_t *vector = type_new(TYPE_TYPE_VECTOR);
    vector->vector_type = of;

    return vector;
}

symbol_t *symbol_new(const identifier_t *identifier, type_t *is) {
    symbol_t *s = malloc(sizeof(symbol_t));

    if (!s) {
        fprintf(stderr, "couldn't allocate symbol\n");
        return NULL;
    }

    memset(s, 0, sizeof(symbol_t));
    memcpy(&s->identifier, identifier, sizeof(identifier_t));

    s->is = is;

    return s;
}

void symbol_delete(symbol_t *symbol) {
    if (symbol) {
        type_delete(symbol->is);
        free(symbol);
    }
}

structure_t *structure_new(const identifier_t *identifier) {
    structure_t *s = malloc(sizeof(structure_t));

    if (!s) {
        fprintf(stderr, "couldn't allocate symobl\n");
        return NULL;
    }

    memset(s, 0, sizeof(structure_t));
    memcpy(&s->identifier, identifier, sizeof(identifier_t));
    s->nmembers = 0;

    return s;
}

void structure_add_member(structure_t *structure, symbol_t *member) {
    structure->members[(structure->nmembers)++] = member;
}

void structure_delete(structure_t *structure) {
    for (uint8_t i = 0; i < structure->nmembers; i++) {
        symbol_delete(structure->members[i]);
    }

    free(structure);
}

