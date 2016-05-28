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

type_t* type_function_new(function_signature_t* signature) {
    type_t* t = type_new(TYPE_TYPE_FUNCTION);
    t->signature = signature;

    return t;
}

void type_print(FILE* output, const context_t* ctx, const type_t* type) {
    if (!type) {
        fprintf(output, "void");
        return;
    }

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
        type_print(output, ctx, type->optional_type);
        fprintf(output, " >");
        break;

      case TYPE_TYPE_VECTOR:
        fprintf(output, "ez::vector< ");
        type_print(output, ctx, type->vector_type);
        fprintf(output, " >");
        break;

      case TYPE_TYPE_STRUCTURE:
        if (program_has_builtin_structure(ctx->program,
                                          &type->structure_type->identifier))
        {
            fprintf(output, "ez::");
        }
        fprintf(output, type->structure_type->identifier.value);
        break;

      case TYPE_TYPE_FUNCTION:
        fprintf(output, "std::function< ");
        type_print(output, ctx, type->signature->return_type);
        fprintf(output, "(");
        for (int i = 0; i < type->signature->args_types.size; i++) {
            access_type_t at =
                (access_type_t)type->signature->args_access.elements[i];
            if (at == ACCESS_TYPE_INPUT) {
                fprintf(output, "const ");
            }

            type_print(output, ctx, type->signature->args_types.elements[i]);
            if (at == ACCESS_TYPE_OUTPUT || at == ACCESS_TYPE_INPUT_OUTPUT) {
                fprintf(output, "&");
            }

            if (i + 1 < type->signature->args_types.size) {
                fprintf(output, ", ");
            }
        }
        fprintf(output, ") >");
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
            } else
            if (a->type == TYPE_TYPE_FUNCTION) {
                return function_signature_is_equals(a->signature,
                                                    b->signature);
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
    if (!type) {
        return false;
    }
    return    type->type == TYPE_TYPE_INTEGER
           || type->type == TYPE_TYPE_NATURAL
           || type->type == TYPE_TYPE_REAL;
}

bool type_is_integer(const type_t* type) {
    if (!type) {
        return false;
    }
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
    } else
    if (copy->type == TYPE_TYPE_FUNCTION) {
        copy->signature = function_signature_copy(type->signature);
    } else
    if (copy->type == TYPE_TYPE_OPTIONAL) {
        copy->optional_type = type_copy(type->optional_type);
    }
    return copy;
}

char* type_print_ez(const type_t* type, char* buf) {
    char subbuf[512] = "";
    const type_t* it = type;
    while (it != NULL) {
        switch (it->type) {
          case TYPE_TYPE_BOOLEAN:
            strcat(buf, "boolean");
            it = NULL;
            break;

          case TYPE_TYPE_INTEGER:
            strcat(buf, "integer");
            it = NULL;
            break;

          case TYPE_TYPE_NATURAL:
            strcat(buf, "natural");
            it = NULL;
            break;

          case TYPE_TYPE_REAL:
            strcat(buf, "real");
            it = NULL;
            break;

          case TYPE_TYPE_CHAR:
            strcat(buf, "char");
            it = NULL;
            break;

          case TYPE_TYPE_STRING:
            strcat(buf, "string");
            it = NULL;
            break;

          case TYPE_TYPE_VECTOR:
            strcat(buf, "vector of ");
            it = it->vector_type;
            break;

          case TYPE_TYPE_STRUCTURE:
            strcat(buf, it->structure_type->identifier.value);
            it = NULL;
            break;

          case TYPE_TYPE_OPTIONAL:
            strcat(buf, "optional ");
            it = it->optional_type;
            break;

          case TYPE_TYPE_FUNCTION:
            strcat(buf, "function ");
            strcat(buf, function_signature_print_ez(it->signature, subbuf));
            it = NULL;
            break;
        }
    }

    return buf;
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

void symbol_print(FILE* output, const context_t* ctx, const symbol_t* symbol) {
    type_print(output, ctx, symbol->is);
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

void structure_print(FILE* output, const context_t* ctx,
                     const structure_t* structure)
{
    fprintf(output, "struct %s {\n", structure->identifier.value);
    for (int i = 0; i < structure->members.size; i++) {
        fprintf(output, "    ");
        symbol_print(output, ctx, structure->members.elements[i]);
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
