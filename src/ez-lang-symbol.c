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

type_t* type_optional_new(type_t* of) {
    type_t* optional = type_new(TYPE_TYPE_OPTIONAL);
    optional->optional_type = of;

    return optional;
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

      case TYPE_TYPE_OPTIONAL:
        fprintf(output, "ez::optional< ");
        type_print(output, type->optional_type);
        fprintf(output, " >");
        break;

      case TYPE_TYPE_VECTOR:
        fprintf(output, "ez::vector< ");
        type_print(output, type->vector_type);
        fprintf(output, " >");
        break;

      case TYPE_TYPE_STRUCTURE:
        fprintf(output, type->structure_type->identifier.value);
        break;
    }
}

bool types_are_equals(const type_t* a, const type_t* b) {
    if (a && b) {
        if (a->type == b->type) {
            if (a->type == TYPE_TYPE_STRUCTURE) {
                /* Can compare structure address since it comes from the
                 * program. */
                return a->structure_type == b->structure_type;
            } else
            if (a->type == TYPE_TYPE_VECTOR) {
                return types_are_equals(a->vector_type, b->vector_type);
            } else
            if (a->type == TYPE_TYPE_OPTIONAL) {
                return types_are_equals(a->optional_type, b->optional_type);
            }
            return true;
        }
        return false;
    } else if (!a && !b) {
        return true;
    }
    return false;
}

bool type_is_number(const type_t* type) {
    return    type->type == TYPE_TYPE_INTEGER
           || type->type == TYPE_TYPE_NATURAL
           || type->type == TYPE_TYPE_REAL;
}

bool type_is_integer(const type_t* type) {
    return    type->type == TYPE_TYPE_INTEGER
           || type->type == TYPE_TYPE_NATURAL;
}

bool types_are_equivalent(const type_t* a, const type_t* b) {
    if (type_is_number(a)) {
        return type_is_number(b);
    }
    return types_are_equals(a, b);
}

type_t* type_copy(const type_t* type) {
    type_t* copy = type_new(type->type);
    if (copy->type == TYPE_TYPE_VECTOR) {
        copy->vector_type = type_copy(type->vector_type);
    } else
    if (copy->type == TYPE_TYPE_STRUCTURE) {
        copy->structure_type = type->structure_type;
    }
    return copy;
}

type_t* type_boolean;
type_t* type_integer;
type_t* type_natural;
type_t* type_real;
type_t* type_char;
type_t* type_string;

__attribute__((constructor))
static void _allocate_primitive_types(void) {
    type_boolean = type_boolean_new();
    type_integer = type_integer_new();
    type_natural = type_natural_new();
    type_real    = type_real_new();
    type_char    = type_char_new();
    type_string  = type_string_new();
}

__attribute__((destructor))
static void _free_primitive_types(void) {
    type_delete(type_boolean);
    type_delete(type_integer);
    type_delete(type_natural);
    type_delete(type_real);
    type_delete(type_char);
    type_delete(type_string);
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

void symbol_print(FILE* output, const symbol_t* symbol) {
    type_print(output, symbol->is);
    fprintf(output, " %s ", symbol->identifier.value);
}

bool symbol_is(const symbol_t* symbol, const identifier_t* id) {
    return strcmp(symbol->identifier.value, id->value) == 0;
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
    fprintf(output, "struct %s {\n", structure->identifier.value);
    for (int i = 0; i < structure->members.size; i++) {
        fprintf(output, "    ");
        symbol_print(output, structure->members.elements[i]);
        fprintf(output, ";\n");
    }
    fprintf(output, "};\n");
}

bool structure_is(const structure_t* structure, const identifier_t* id) {
    return strcmp(structure->identifier.value, id->value) == 0;
}

bool structure_has_member(const structure_t* structure, const identifier_t* id)
{
    return vector_contains(&structure->members, id, (cmp_func_t)&symbol_is);
}

symbol_t* structure_find_member(const structure_t* structure,
                                const identifier_t* id) {
    return vector_find(&structure->members, id, (cmp_func_t)&symbol_is);
}
