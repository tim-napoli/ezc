#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ez-lang.h"

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

void value_wipe(value_t* value) {
    switch (value->type) {
      case VALUE_TYPE_STRING:
        free(value->string);
        break;

      case VALUE_TYPE_VALREF:
        valref_delete(value->valref);
        break;
    }
}

