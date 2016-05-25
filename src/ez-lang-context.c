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

bool context_has_function(const context_t* ctx, const identifier_t* id) {
    if (function_is(ctx->function, id)) {
        return true;
    }

    if (program_has_function(ctx->program, id)) {
        return true;
    }

    return program_has_procedure(ctx->program, id);
}

bool context_valref_is_valid(const context_t* ctx, const valref_t* valref) {

    if (!context_has_identifier(ctx, &valref->identifier)) {
        return false;
    }

    if (valref->is_funccall) {
        if (context_has_function(ctx, &valref->identifier)) {
            return context_parameters_is_valid(ctx, &valref->parameters);
        }

        return false;
    }

    if (valref->has_indexing) {

    }

    if (ctx->function) {

        function_arg_t* arg = function_find_arg(ctx->function, &valref->identifier);

        if (arg) {
            return context_valref_next_is_valid(valref, arg->symbol);
        }

        symbol_t* local = function_find_local(ctx->function, &valref->identifier);

        if (local) {
            return context_valref_next_is_valid(valref, local);
        }
    }

    if (ctx->program) {
        symbol_t* global = program_find_global(ctx->program, &valref->identifier);

        if (global) {
            return context_valref_next_is_valid(valref, global);
        }

        constant_t* constant = program_find_constant(ctx->program, &valref->identifier);

        if (constant) {
            return context_valref_next_is_valid(valref, constant->symbol);
        }
    }

    return false;
}

bool context_valref_next_is_valid(const valref_t* valref, const symbol_t* symbol) {
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

        return context_valref_next_is_valid(valref->next, next_symbol);
    }

    return true;
}

bool context_value_is_valid(const context_t* ctx, const value_t* value) {
    if (value->type == VALUE_TYPE_VALREF) {
        return context_valref_is_valid(ctx, value->valref);
    }

    return true;
}

bool context_expression_is_valid(const context_t* ctx, const expression_t* e) {
    if (e->type == EXPRESSION_TYPE_VALUE) {
        return context_value_is_valid(ctx, &e->value);
    } else {
        bool left_is_valid, right_is_valid;

        if (e->left) {
            left_is_valid = context_expression_is_valid(ctx, e->left);
        }

        if (e->right) {
            right_is_valid = context_expression_is_valid(ctx, e->right);
        }

        return left_is_valid && right_is_valid;
    }
}

bool context_parameters_is_valid(const context_t* ctx, const parameters_t* p) {
    bool is_valid = true;

    for (size_t i = 0; i < p->parameters.size; i++) {
        is_valid = is_valid &&
                    context_expression_is_valid(ctx,
                                                vector_get(&p->parameters, i));
    }

    return is_valid;
}
