#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ez-lang.h"

void parameters_print(FILE* output, const parameters_t* params) {
    for (int i = 0; i < params->nparameters; i++) {
        /* TODO expression_print(output, params->parameters[i]); */
        if (i + 1 < params->nparameters) {
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

    return v;
}

void valref_delete(valref_t* v) {
    if (v) {
        valref_delete(v->next);
        /* TODO delete inner expressions */
        free(v);
    }
}

void valref_print(FILE* output, const valref_t* value) {
    fprintf(output, "%s", value->identifier.value);
    if (value->has_indexing) {
        for (int i = 0; i < value->nindexings; i++) {
            fprintf(output, "[");
            /* TODO expression_print(output, value->indexings[i]); */
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
        fprintf(output, "%s", value->string);
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

