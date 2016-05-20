#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ez-lang.h"

type_t* type_new(type_type_t type) {
    type_t* t = calloc(1, sizeof(type_t));

    if (!t) {
        fprintf(stderr, "couldn't allocate type\n");
        exit(EXIT_FAILURE);
    }

    t->type = type;

    return t;
}

void type_delete(type_t* t) {
    if (t) {
        if (t->type == TYPE_TYPE_VECTOR) {
            type_delete(t->vector_type);
        }

        free(t);
    }
}

void type_print(type_t* t) {
    switch (t->type) {
        case TYPE_TYPE_BOOLEAN:
            fprintf(stdout, "BOOLEAN");
            break;

        case TYPE_TYPE_INTEGER:
            fprintf(stdout, "INTEGER");
            break;

        case TYPE_TYPE_NATURAL:
            fprintf(stdout, "NATURAL");
            break;

        case TYPE_TYPE_REAL:
            fprintf(stdout, "REAL");
            break;

        case TYPE_TYPE_CHAR:
            fprintf(stdout, "CHAR");
            break;

        case TYPE_TYPE_STRING:
            fprintf(stdout, "STRING");
            break;

        case TYPE_TYPE_VECTOR:
            fprintf(stdout, "VECTOR OF ");
            type_print(t->vector_type);
            break;

        case TYPE_TYPE_STRUCTURE:
            fprintf(stdout, "%s", t->structure_type->identifier.value);
            break;

        default:
            fprintf(stdout, "UNKNOWN");
    }
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

type_t *type_structure_new(structure_t *s) {
    type_t *t = type_new(TYPE_TYPE_STRUCTURE);
    t->structure_type = s;

    return t;
}

symbol_t *symbol_new(const identifier_t *identifier, type_t *is) {
    symbol_t *s = calloc(1, sizeof(symbol_t));

    if (!s) {
        fprintf(stderr, "couldn't allocate symbol\n");
        exit(EXIT_FAILURE);
    }

    memcpy(&s->identifier, identifier, sizeof(identifier_t));
    s->is = is;

    return s;
}

void symbol_delete(symbol_t *symbol) {
    if (symbol) {
        if (symbol->is) {
            type_delete(symbol->is);
        }

        free(symbol);
    }
}

void symbol_print(int i, void *s) {
    symbol_t *smbl = s;
    fprintf(stdout, "%s is ", smbl->identifier.value);
    type_print(smbl->is);
    fprintf(stdout, "\n");
}

structure_t *structure_new(const identifier_t *identifier) {
    structure_t *s = calloc(1, sizeof(structure_t));

    if (!s) {
        fprintf(stderr, "couldn't allocate symobl\n");
        exit(EXIT_FAILURE);
    }

    memcpy(&s->identifier, identifier, sizeof(identifier_t));
    vector_init(&s->members, 0);

    return s;
}

void structure_delete(structure_t *structure) {
    if (structure) {
        vector_wipe(&structure->members, (delete_func_t)&symbol_delete);
        free(structure);
    }
}

void structure_add_member(structure_t *structure, symbol_t *member) {
    vector_push(&structure->members, member);
}

void structure_print(int i, void *v) {
    structure_t *s = v;

    fprintf(stdout, "Structure: %s\n", s->identifier.value);
    vector_map(&s->members, symbol_print);
    fprintf(stdout, "\n");
}
