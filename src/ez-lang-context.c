#include "ez-lang.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void context_init(context_t* ctx) {
    ctx->program = NULL;
    ctx->function = NULL;
}

void context_set_program(context_t* ctx, program_t* prg) {
    ctx->program = prg;
}

void context_set_function(context_t* ctx, function_t* func) {
    ctx->function = func;
}

identifier_t context_get_program_identifier(context_t* ctx) {
    return ctx->program->identifier;
}

bool context_has_identifier(const context_t* ctx,
                            const identifier_t* id)
{
    if (ctx->function) {
        if (function_has_arg(ctx->function, id)
        ||  function_has_local(ctx->function, id)
        ||  function_is(ctx->function, id))
        {
            return true;
        }
    }

    if (program_has_global(ctx->program, id)
    ||  program_has_constant(ctx->program, id)
    ||  program_has_structure(ctx->program, id)
    ||  program_has_function(ctx->program, id)
    ||  program_has_procedure(ctx->program, id))
    {
        return true;
    }

    return false;
}

structure_t* context_find_structure(const context_t* ctx,
                                    const identifier_t* structure_id) {
    return program_find_structure(ctx->program, structure_id);
}

bool context_valref_is_valid(const context_t* ctx, const valref_t* valref) {

    if (!context_has_identifier(ctx, &valref->identifier)) {
        return false;
    }

    if (ctx->function) {

        function_arg_t* arg = function_find_arg(ctx->function, &valref->identifier);

        if (arg) {
            return valref_is_valid(valref, arg->symbol);
        }

        symbol_t* local = function_find_local(ctx->function, &valref->identifier);

        if (local) {
            return valref_is_valid(valref, local);
        }
    }

    if (ctx->program) {
        symbol_t* global = program_find_global(ctx->program, &valref->identifier);

        if (global) {
            return valref_is_valid(valref, global);
        }

        constant_t* constant = program_find_constant(ctx->program, &valref->identifier);

        if (constant) {
            return valref_is_valid(valref, constant->symbol);
        }
    }

    // TODO : if is funccall : search in vector functions.

    return false;
}

bool valref_is_valid(const valref_t* valref, const symbol_t* symbol) {
    if (symbol->is->type == TYPE_TYPE_STRUCTURE && !valref->next) {
        return false;
    }

    if (symbol->is->type != TYPE_TYPE_STRUCTURE && valref->next) {
        return false;
    }

    if (symbol->is->type == TYPE_TYPE_STRUCTURE && valref->next) {
        symbol_t* next_symbol = structure_find_member(symbol->is->structure_type, &valref->next->identifier);

        if (!next_symbol) {
            return false;
        }

        return valref_is_valid(valref->next, next_symbol);
    }

    return true;
}
