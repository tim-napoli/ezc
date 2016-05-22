#include "ez-lang.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

context_t *context_new(identifier_t *identifier, context_t *parent_context) {
  context_t *c = calloc(1, sizeof(context_t));

  if (!c) {
    fprintf(stderr, "couldn't allocate program context\n");
    exit(EXIT_FAILURE);
  }

  memcpy(&c->identifier, identifier, sizeof(identifier_t));
  c->parent_context = parent_context;

  vector_init(&c->constants, 0);
  vector_init(&c->globals, 0);
  vector_init(&c->locals, 0);
  vector_init(&c->structures, 0);
  vector_init(&c->arguments, 0);

  return c;
}

void context_delete(context_t *ctx) {
  vector_wipe(&ctx->constants, (delete_func_t)&symbol_delete);
  vector_wipe(&ctx->globals, (delete_func_t)&symbol_delete);
  vector_wipe(&ctx->locals, (delete_func_t)&symbol_delete);
  vector_wipe(&ctx->structures, (delete_func_t)&structure_delete);
  vector_wipe(&ctx->arguments, (delete_func_t)&structure_delete);
  free(ctx);
}

void context_print(context_t *ctx) {
  fprintf(stdout, "Context: %s\n\n", ctx->identifier.value);

  vector_map(&ctx->structures, structure_print);
  fprintf(stdout, "\n");

  vector_map(&ctx->constants, symbol_print);
  fprintf(stdout, "\n");

  vector_map(&ctx->globals, symbol_print);
  fprintf(stdout, "\n");
}

void context_add_constant(context_t *context, symbol_t *constant) {
  vector_push(&context->constants, constant);
}

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

structure_t *context_find_structure(const context_t *ctx,
                                    const identifier_t *id) {
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

symbol_t* context_find_symbol(const context_t *ctx,
                              const identifier_t *id) {
  symbol_t* symbol = NULL;

  if ((symbol = context_find_argument(ctx, id))) {
      return symbol;
  } else
  if ((symbol = context_find_local(ctx, id))) {
      return symbol;
  } else
  if ((symbol = context_find_global(ctx, id))) {
      return symbol;
  } else
  if ((symbol = context_find_constant(ctx, id))) {
      return symbol;
  } else
  if (ctx->parent_context) {
    return context_find_symbol(ctx->parent_context, id);
  }

  return NULL;
}

symbol_t* context_find_argument(const context_t* ctx, const identifier_t* id){
    for (size_t i = 0; i < ctx->arguments.size; i++) {
        argument_t* arg = vector_get(&ctx->arguments, i);

        if (strcmp(arg->symbol->identifier.value, id->value) == 0) {
            return arg->symbol;
        }
    }

    return NULL;
}

symbol_t* _find_symbol(const vector_t* vector, const identifier_t* id) {
    for (size_t i = 0; i < vector->size; i++) {
        symbol_t* symbol = vector_get(vector, i);
        if (strcmp(symbol->identifier.value, id->value) == 0) {
            return symbol;
        }
    }

    return NULL;
}

symbol_t* context_find_local(const context_t* ctx, const identifier_t* id) {
    return _find_symbol(&ctx->locals, id);
}

symbol_t* context_find_global(const context_t* ctx, const identifier_t* id) {
    return _find_symbol(&ctx->globals, id);
}

symbol_t* context_find_constant(const context_t* ctx, const identifier_t* id) {
    return _find_symbol(&ctx->constants, id);
}
