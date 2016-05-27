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
            if (type->type == TYPE_TYPE_VECTOR) {
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
            } else
            if (type->type == TYPE_TYPE_OPTIONAL) {
                if (!optional_function_exists(&valref->identifier)) {
                    return false;
                }
                if (!optional_function_call_is_valid(ctx, valref, type))
                {
                    return false;
                }
                type = optional_function_get_type(valref, type);
                /* XXX what if type is not NULL but primitive ? */
                if (!type && valref->next) {
                    return false;
                }
                return _context_valref_is_valid(ctx, valref->next, type);
            }
            return false;
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
    }

    bool left = true;
    bool right = true;

    const type_t* ltype = NULL;
    const type_t* rtype = NULL;

    if (e->left) {
        left = context_expression_is_valid(ctx, e->left);
        ltype = context_expression_get_type(ctx, e->left);
    }

    if (e->right) {
        right = context_expression_is_valid(ctx, e->right);
        rtype = context_expression_get_type(ctx, e->right);
    }

    if (!left || !right) {
        return false;
    }

    if (!types_are_equivalent(ltype, rtype)) {
        return false;
    }

    if (e->type == EXPRESSION_TYPE_CMP_OP_EQUALS
    ||  e->type == EXPRESSION_TYPE_CMP_OP_DIFFERENT) {
        return true;
    }

    if (e->type == EXPRESSION_TYPE_CMP_OP_LOWER_OR_EQUALS
    ||  e->type == EXPRESSION_TYPE_CMP_OP_GREATER_OR_EQUALS
    ||  e->type == EXPRESSION_TYPE_CMP_OP_LOWER
    ||  e->type == EXPRESSION_TYPE_CMP_OP_GREATER)
    {
        return type_is_number(ltype);
    }

    if (e->type == EXPRESSION_TYPE_BOOL_OP_AND
    ||  e->type == EXPRESSION_TYPE_BOOL_OP_OR
    ||  e->type == EXPRESSION_TYPE_BOOL_OP_NOT) {
        return rtype->type == TYPE_TYPE_BOOLEAN;
    }

    if (e->type == EXPRESSION_TYPE_ARITHMETIC_OP_PLUS) {
        return type_is_number(ltype) || ltype->type == TYPE_TYPE_STRING;
    }

    if (e->type == EXPRESSION_TYPE_ARITHMETIC_OP_MINUS
    ||  e->type == EXPRESSION_TYPE_ARITHMETIC_OP_MUL
    ||  e->type == EXPRESSION_TYPE_ARITHMETIC_OP_DIV) {
        return type_is_number(ltype);
    }

    if (e->type == EXPRESSION_TYPE_ARITHMETIC_OP_MOD)
    {
        return type_is_integer(ltype) && type_is_integer(rtype);
    }

    return false;
}

bool context_parameters_are_valid(const context_t* ctx,
                                  const parameters_t* p) {
    bool is_valid = true;

    for (size_t i = 0; i < p->parameters.size; i++) {
        is_valid = is_valid &&
                    context_expression_is_valid(ctx,
                                                vector_get(&p->parameters, i));
    }

    return is_valid;
}

bool context_affectation_is_valid(const context_t* ctx,
                                  const affectation_instr_t* affectation) {
    return types_are_equivalent(context_valref_get_type(ctx, affectation->lvalue),
                                context_expression_get_type(ctx,
                                                     affectation->expression));
}


const type_t* context_expression_get_type(const context_t* ctx,
                                  const expression_t* expr)
{
    if (expr->type == EXPRESSION_TYPE_VALUE) {
        return context_value_get_type(ctx, &expr->value);
    } else if (expr->type >= EXPRESSION_TYPE_CMP_OP_EQUALS
           &&  expr->type <= EXPRESSION_TYPE_BOOL_OP_OR) {
        return type_boolean;
    }

    const type_t* left_type = context_expression_get_type(ctx, expr->left);
    const type_t* right_type = context_expression_get_type(ctx, expr->right);

    /* Arithmetic operator :
     * if we have only naturals, return naturals,
     * if we have an integer, return integer,
     * if we have a real, return real,
     * if we have something else, ohoh, something is wrong during parsing !
     */
    const type_t* ret_type = left_type;
    if (right_type->type == TYPE_TYPE_REAL) {
        ret_type = right_type;
    } else
    if (right_type->type == TYPE_TYPE_INTEGER
    &&  left_type->type == TYPE_TYPE_NATURAL)
    {
        ret_type = right_type;
    }

    return ret_type;
}
