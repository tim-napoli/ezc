#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ez-lang.h"


context_t *context_new(identifier_t *identifier, context_t *parent_context) {
    context_t *c = calloc(1, sizeof(context_t));

    if (!c) {
        fprintf(stderr, "couldn't allocate program context\n");
        exit(EXIT_FAILURE);
    }

    memcpy(&c->identifier, identifier, sizeof(identifier_t));
    c->parent_context = parent_context;

    vector_init(&c->globals, 0);
    vector_init(&c->locals, 0);
    vector_init(&c->structures, 0);

    return c;
}

void context_delete(context_t* ctx) {
    vector_wipe(&ctx->globals, (delete_func_t)&symbol_delete);
    vector_wipe(&ctx->locals, (delete_func_t)&symbol_delete);
    vector_wipe(&ctx->structures, (delete_func_t)&structure_delete);
    free(ctx);
}

// TODO : add constant;

void context_add_global(context_t *context, symbol_t *global) {
    vector_push(&context->globals, global);
}

void context_add_structure(context_t *context, structure_t *structure) {
    vector_push(&context->structures, structure);
}

// TODO : add function

// TODO : add procedure

void context_add_local(context_t *context, symbol_t *local) {
    vector_push(&context->locals, local);
}

// TODO add arg.
