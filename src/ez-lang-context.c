#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ez-lang.h"


context_t *context_new(identifier_t *identifier, context_t *global_context) {
    context_t *c = calloc(1, sizeof(context_t));

    if (!c) {
        fprintf(stderr, "couldn't allocate program context\n");
        exit(EXIT_FAILURE);
    }

    memcpy(&c->identifier, identifier, sizeof(identifier_t));
    c->global_context = global_context;

    return c;
}

// TODO : add constant;

void context_add_global(context_t *context, symbol_t *global) {
    context->globals[(context->nglobals)++] = global;
}

void context_add_structure(context_t *context, structure_t *structure) {
    context->structures[(context->nstructures)++] = structure;
}

// TODO : add function

// TODO : add procedure

void context_add_local(context_t *context, symbol_t *local) {
    context->locals[(context->nlocals)++] = local;
}

// TODO add arg.
