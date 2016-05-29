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

void parameters_print(FILE* output, const context_t* ctx,
                      const parameters_t* params)
{
    for (int i = 0; i < params->parameters.size; i++) {
        expression_print(output, ctx, params->parameters.elements[i]);
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

    return v;
}

void valref_delete(valref_t* v) {
    if (v) {
        valref_delete(v->next);
        parameters_wipe(&v->parameters);
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

void valref_print(FILE* output, const context_t* ctx, const valref_t* value) {
    if (program_has_builtin_function(ctx->program, &value->identifier)
    ||  program_has_builtin_procedure(ctx->program, &value->identifier))
    {
        fprintf(output, "ez::");
    }

    fprintf(output, "%s", value->identifier.value);

    if (value->is_funccall) {
        fprintf(output, "(");
        parameters_print(output, ctx, &value->parameters);
        fprintf(output, ")");
    }

    if (value->next) {
        fprintf(output, ".");
        valref_print(output, ctx, value->next);
    }
}

void value_wipe(value_t* value) {
    switch (value->type) {
      case VALUE_TYPE_STRING:
        free(value->string);
        break;

      case VALUE_TYPE_VALREF:
        valref_delete(value->valref);
        break;

      case VALUE_TYPE_EMPTY:
        type_delete(value->empty_type);
        break;

      default:
        break;
    }
}

void empty_print(FILE* output, const context_t* ctx, const type_t* type) {
    fprintf(output, "ez::optional< ");
    type_print(output, ctx, type->optional_type);
    fprintf(output, " >()");
}

void value_print(FILE* output, const context_t* ctx, const value_t* value) {
    switch (value->type) {
      case VALUE_TYPE_STRING:
        fprintf(output, "\"%s\"", value->string);
        break;

      case VALUE_TYPE_CHAR:
        fprintf(output, "'%c'", value->character);
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
        valref_print(output, ctx, value->valref);
        break;

      case VALUE_TYPE_EMPTY:
        empty_print(output, ctx, value->empty_type);
        break;
    }
}

