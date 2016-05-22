#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ez-lang.h"

/**
 * Types
 */

type_t* type_new(type_type_t type) {
    type_t* t = calloc(1, sizeof(type_t));

    if (!t) {
        fprintf(stderr, "couldn't allocate type\n");
        exit(EXIT_FAILURE);
    }

    t->type = type;
    t->constant = false;

    return t;
}

void type_delete(type_t* t) {
    if (t) {
        if (t->type == TYPE_TYPE_VECTOR) {
            type_delete(t->vector_type);
        }

        if (t->constant) {
            expression_delete(t->constant_expression);
        }

        free(t);
    }
}

type_t* type_boolean_new() {
    return type_new(TYPE_TYPE_BOOLEAN);
}

type_t* type_integer_new() {
    return type_new(TYPE_TYPE_INTEGER);
}

type_t* type_natural_new() {
    return type_new(TYPE_TYPE_NATURAL);
}

type_t* type_real_new() {
    return type_new(TYPE_TYPE_REAL);
}
type_t* type_char_new() {
    return type_new(TYPE_TYPE_CHAR);
}

type_t* type_string_new() {
    return type_new(TYPE_TYPE_STRING);
}

type_t* type_vector_new(type_t* of) {
    type_t* vector = type_new(TYPE_TYPE_VECTOR);
    vector->vector_type = of;

    return vector;
}

type_t* type_structure_new(structure_t* s) {
    type_t* t = type_new(TYPE_TYPE_STRUCTURE);
    t->structure_type = s;

    return t;
}

void type_print(FILE* output, const type_t* type) {
    switch (type->type) {
      case TYPE_TYPE_BOOLEAN:
        fprintf(output, "bool");
        break;

      case TYPE_TYPE_INTEGER:
        fprintf(output, "int");
        break;

      case TYPE_TYPE_NATURAL:
        fprintf(output, "unsigned int");
        break;

      case TYPE_TYPE_REAL:
        fprintf(output, "double");
        break;

      case TYPE_TYPE_CHAR:
        fprintf(output, "char");
        break;

      case TYPE_TYPE_STRING:
        fprintf(output, "std::string");
        break;

      case TYPE_TYPE_VECTOR:
        fprintf(output, "std::vector<");
        type_print(output, type->vector_type);
        fprintf(output, ">");
        break;

      case TYPE_TYPE_STRUCTURE:
        fprintf(output, type->structure_type->identifier.value);
        break;
    }
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

void symbol_delete(symbol_t* symbol) {
    if (symbol) {
        if (symbol->is) {
            type_delete(symbol->is);
        }

        free(symbol);
    }
}

void symbol_set_constant(symbol_t* symbol, expression_t *constant) {
    symbol->is->constant = true;
    symbol->is->constant_expression = constant;
}

void symbol_print(FILE* output, const symbol_t* symbol) {
    type_print(stderr, symbol->is);
    fprintf(stderr, " %s ", symbol->identifier.value);
}

/**
 * Structures.
 */

structure_t* structure_new(const identifier_t* identifier) {
    structure_t* s = calloc(1, sizeof(structure_t));

    if (!s) {
        fprintf(stderr, "couldn't allocate symobl\n");
        exit(EXIT_FAILURE);
    }

    memcpy(&s->identifier, identifier, sizeof(identifier_t));
    vector_init(&s->members, 0);

    return s;
}

void structure_delete(structure_t* structure) {
    if (structure) {
        vector_wipe(&structure->members, (delete_func_t)&symbol_delete);
        free(structure);
    }
}

void structure_add_member(structure_t* structure, symbol_t* member) {
    vector_push(&structure->members, member);
}

void structure_print(FILE* output, const structure_t* structure) {
    fprintf(stderr, "struct %s {\n", structure->identifier.value);
    for (int i = 0; i < structure->members.size; i++) {
        fprintf(output, "    ");
        symbol_print(output, structure->members.elements[i]);
        fprintf(output, ";\n");
    }
    fprintf(output, "};\n");
}

