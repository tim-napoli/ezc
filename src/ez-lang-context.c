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
        ||  function_has_local(ctx->function, id))
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

static bool _context_valref_is_valid(const context_t* ctx,
                                     const valref_t* valref,
                                     const type_t* type)
{
    if (!valref) {
        return true;
    }

    if (!type) {
        if (!context_has_identifier(ctx, &valref->identifier)) {
            return false;
        }

        if (valref->is_funccall) {
            function_t* func = program_find_function(ctx->program,
                                                     &valref->identifier);
            if (!func) {
                func = program_find_procedure(ctx->program,
                                              &valref->identifier);
                if (!func) {
                    return false;
                }
            }
            type = func->return_type;
        } else {
            type = context_find_identifier_type(ctx, &valref->identifier);
        }
        return _context_valref_is_valid(ctx, valref->next, type);
    } else {
        if (valref->is_funccall) {
            if (type->type != TYPE_TYPE_VECTOR) {
                return false;
            }
            if (!vector_function_exists(&valref->identifier)) {
                return false;
            }
            if (!vector_function_call_is_valid(ctx, valref, type))
            {
                return false;
            }
            type = vector_function_get_type(valref, type);
            if (!type && valref->next) {
                return false;
            }
            return _context_valref_is_valid(ctx, valref->next, type);
        } else {
            if (type->type != TYPE_TYPE_STRUCTURE) {
                return false;
            }
            structure_t* structure = type->structure_type;
            symbol_t* member = structure_find_member(structure,
                                                     &valref->identifier);
            if (!member) {
                return false;
            }

            return _context_valref_is_valid(ctx, valref->next, member->is);
        }
    }

    return true;
}

bool context_valref_is_valid(const context_t* ctx, const valref_t* valref) {
    return _context_valref_is_valid(ctx, valref, NULL);
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
        /* TODO type validation */
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

