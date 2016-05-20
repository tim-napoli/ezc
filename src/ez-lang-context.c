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

void context_print(context_t* ctx) {
    fprintf(stdout, "Context: %s\n\n", ctx->identifier.value);
    vector_map(&ctx->structures, structure_print);
    vector_map(&ctx->globals, symbol_print);
}

// TODO : add constant;

void context_add_global(context_t* context, symbol_t* global) {
    // TODO : check already exists.
    vector_push(&context->globals, global);
}

void context_add_structure(context_t *context, structure_t *structure) {
    // TODO : Check already exists.
    vector_push(&context->structures, structure);
}

// TODO : add function

// TODO : add procedure

void context_add_local(context_t* context, symbol_t* local) {
    vector_push(&context->locals, local);
}

// TODO add arg.

structure_t* context_find_structure(const context_t* ctx, const identifier_t *id) {
    for (int i = 0; i < ctx->structures.size; i++) {
        structure_t *structure = vector_get(&ctx->structures, i);

        if (strcmp(structure->identifier.value, id->value) == 0) {
            return structure;
        }
    }

    if (ctx->parent_context) {
        return context_find_structure(ctx->parent_context, id);
    }

    return NULL;
}
