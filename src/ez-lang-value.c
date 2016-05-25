#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ez-lang.h"

void parameters_init(parameters_t* params) {
    vector_init(&params->parameters, 0);
}

void parameters_wipe(parameters_t* params) {
    vector_wipe(&params->parameters, (delete_func_t)&expression_delete);
}

void parameters_add(parameters_t* params, expression_t* expr) {
    vector_push(&params->parameters, expr);
}

void parameters_print(FILE* output, const parameters_t* params) {
    for (int i = 0; i < params->parameters.size; i++) {
        expression_print(output, params->parameters.elements[i]);
        if (i + 1 < params->parameters.size) {
            fprintf(output, ", ");
        }
    }
}

valref_t* valref_new(const identifier_t* identifier) {
    valref_t* v = malloc(sizeof(valref_t));
    if (!v) {
        fprintf(stderr, "couldn't allocate varref\n");
        return NULL;
    }

    memset(v, 0, sizeof(valref_t));
    memcpy(&v->identifier, identifier, sizeof(identifier_t));

    parameters_init(&v->parameters);
    vector_init(&v->indexings, 0);

    return v;
}

void valref_delete(valref_t* v) {
    if (v) {
        valref_delete(v->next);
        parameters_wipe(&v->parameters);
        vector_wipe(&v->indexings, (delete_func_t)&expression_delete);
        free(v);
    }
}

void valref_set_next(valref_t* v, valref_t* n) {
    v->next = n;
}

parameters_t* valref_get_parameters(valref_t* v) {
    return &v->parameters;
}

void valref_add_parameter(valref_t* v, expression_t* p) {
    vector_push(&v->parameters.parameters, p);
}

void valref_set_is_funccall(valref_t* v, bool is_funccall) {
    v->is_funccall = is_funccall;
}

void valref_set_has_indexing(valref_t* v, bool has_indexing) {
    v->has_indexing = has_indexing;
}

void valref_add_index(valref_t* v, expression_t* index) {
    vector_push(&v->indexings, index);
}

void valref_print(FILE* output, const valref_t* value) {
    fprintf(output, "%s", value->identifier.value);
    if (value->has_indexing) {
        for (int i = 0; i < value->indexings.size; i++) {
            fprintf(output, "[");
            expression_print(output, value->indexings.elements[i]);
            fprintf(output, "]");
        }
    }

    if (value->is_funccall) {
        fprintf(output, "(");
        parameters_print(output, &value->parameters);
        fprintf(output, ")");
    }

    if (value->next) {
        fprintf(output, ".");
        valref_print(output, value->next);
    }
}

static const type_t* _valref_get_type(const context_t* ctx,
                                      const valref_t* valref,
                                      const type_t* type)
{
    const type_t* next_type = NULL;

    if (valref->has_indexing) {
        if (!type) {
            type = context_find_identifier_type(ctx, &valref->identifier);
        }
        assert (type->type == TYPE_TYPE_VECTOR);
        for (int i = 0; i < valref->indexings.size; i++) {
            type = type->vector_type;
            assert (type->type == TYPE_TYPE_VECTOR);
        }
    }

    if (valref->next) {
        if (!type) {
            next_type = context_find_identifier_type(ctx, &valref->identifier);
        } else {
            assert (type->type == TYPE_TYPE_STRUCTURE);
            structure_t* structure = type->structure_type;
            symbol_t* member = structure_find_member(structure,
                                                     &valref->identifier);
            next_type = member->is;
        }
        return _valref_get_type(ctx, valref->next, next_type);
    } else
    if (valref->is_funccall) {
        function_t* func = program_find_function(ctx->program,
                                                 &valref->identifier);
        return func->return_type;
    }

    return type;
}

const type_t* valref_get_type(const context_t* ctx, const valref_t* valref)
{
    const type_t* initial_type =
        context_find_identifier_type(ctx, &valref->identifier);

    return _valref_get_type(ctx, valref, initial_type);
}

void value_wipe(value_t* value) {
    switch (value->type) {
      case VALUE_TYPE_STRING:
        free(value->string);
        break;

      case VALUE_TYPE_VALREF:
        valref_delete(value->valref);
        break;

      default:
        break;
    }
}

void value_print(FILE* output, const value_t* value) {
    switch (value->type) {
      case VALUE_TYPE_STRING:
        fprintf(output, "\"%s\"", value->string);
        break;

      case VALUE_TYPE_REAL:
        fprintf(output, "%f", value->real);
        break;

      case VALUE_TYPE_INTEGER:
        fprintf(output, "%d", value->integer);
        break;

      case VALUE_TYPE_NATURAL:
        fprintf(output, "%u", value->natural);
        break;

      case VALUE_TYPE_BOOLEAN:
        fprintf(output, "%s", (value->boolean) ? "true" : "false");
        break;

      case VALUE_TYPE_VALREF:
        valref_print(output, value->valref);
        break;
    }
}

type_t* value_get_type(const context_t* ctx, const value_t* value) {
    switch (value->type) {
      case VALUE_TYPE_STRING:
        return type_string_new();

      case VALUE_TYPE_REAL:
        return type_real_new();

      case VALUE_TYPE_INTEGER:
        return type_integer_new();

      case VALUE_TYPE_NATURAL:
        return type_natural_new();

      case VALUE_TYPE_BOOLEAN:
        return type_boolean_new();

      case VALUE_TYPE_VALREF:
        return type_copy(valref_get_type(ctx, value->valref));
    }

    return NULL;
}

