#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ez-lang.h"


context_t *context_new(identifier_t identifier, context_t *global_context) {
    program_context_t *c = malloc(sizeof(program_context_t));

    if (!c) {
        fprintf(stderr, "couldn't allocate program context\n");
        return NULL;
    }

    memset(c, 0, sizeof(program_context_t));
    memcpy(&c->identifier, identifier, sizeof(identifier_t));
    c->global_context = global_context;

    return c;
}

// TODO : add constant;

void context_add_global(program_context_t *program, symbol_t *global) {
    program->globals[(program->nglobals)++] = global;
}

void context_add_structure(program_context_t *program, structure_t *structure) {
    program->structures[(program->nstructures)++] = structure;
}

// TODO : add function

// TODO : add procedure

void context_add_local(function_context_t *function, symbol_t *local) {
    program->locals[(program->nlocals)++] = local;
}

// TODO add arg.

bool context
