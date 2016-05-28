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

bool context_funccall_is_valid(const context_t* ctx,
                               const function_signature_t* func,
                               const parameters_t* params,
                               char* error_msg)
{
    if (params->parameters.size != func->args_types.size) {
        sprintf(error_msg,
                "calling a function with wrong number of arguments");
        return false;
    }

    for (int i = 0; i < func->args_types.size; i++) {
        const type_t* func_arg_type = func->args_types.elements[i];
        const type_t* param_arg_type =
                context_expression_get_type(ctx,
                                            params->parameters.elements[i]);

        if (!types_are_equivalent(func_arg_type, param_arg_type)) {
            char expected_type[512] = "";
            char given_type[512] = "";

            type_print_ez(func_arg_type, expected_type);
            type_print_ez(param_arg_type, given_type);

            sprintf(error_msg,
                    "invalid type for argument %d, expected '%s', got '%s'",
                    i + 1, expected_type, given_type);
            return false;
        }
    }

    return true;
}

static bool _context_valref_is_valid(const context_t* ctx,
                                     const valref_t* valref,
                                     const type_t* type,
                                     char* error_msg)
{
    if (!valref) {
        return true;
    }

    if (!type) {
        if (!context_has_identifier(ctx, &valref->identifier)) {
            sprintf(error_msg, "identifier '%s' doesn't exist",
                    valref->identifier.value);
            return false;
        }

        if (valref->is_funccall) {
            function_t* func = program_find_function(ctx->program,
                                                     &valref->identifier);
            function_t* proc = program_find_procedure(ctx->program,
                                                      &valref->identifier);
            function_signature_t* lambda = context_find_lambda_function(ctx, &valref->identifier);
            const function_signature_t* signature = NULL;

            if (!func && !proc && !lambda) {
                sprintf(error_msg, "'%s' is not a function or a procedure",
                        valref->identifier.value);
                return false;
            } else if (func) {
                type = func->return_type;
                signature = function_get_signature(func);
            } else if (proc) {
                type = proc->return_type;
                signature = function_get_signature(proc);
            } else if (lambda) {
                type = lambda->return_type;
                signature = lambda;
            }

            char suberr[512];
            if (!context_funccall_is_valid(ctx, signature, &valref->parameters,
                                           suberr))
            {
                sprintf(error_msg, "invalid function '%s' call: %s",
                        valref->identifier.value, suberr);
                return false;
            }

        } else {
            type = context_find_identifier_type(ctx, &valref->identifier);
        }
        return _context_valref_is_valid(ctx, valref->next, type, error_msg);
    } else {
        if (valref->is_funccall) {
            if (type->type == TYPE_TYPE_VECTOR) {
                if (!vector_function_exists(&valref->identifier)) {
                    sprintf(error_msg, "vector has no method called '%s'",
                            valref->identifier.value);
                    return false;
                }
                if (!vector_function_call_is_valid(ctx, valref, type))
                {
                    sprintf(error_msg, "invalid vector function '%s' call",
                            valref->identifier.value);
                    return false;
                }
                type = vector_function_get_type(valref, type);
                if (!type && valref->next) {
                    sprintf(error_msg, "trying to access member of something "
                                       "that is not a structure or an object");
                    return false;
                }
                return _context_valref_is_valid(ctx, valref->next, type,
                                                error_msg);
            } else
            if (type->type == TYPE_TYPE_OPTIONAL) {
                if (!optional_function_exists(&valref->identifier)) {
                    sprintf(error_msg, "optional has no method called '%s'",
                            valref->identifier.value);
                    return false;
                }
                if (!optional_function_call_is_valid(ctx, valref, type))
                {
                    sprintf(error_msg, "invalid optional function '%s' call",
                            valref->identifier.value);
                    return false;
                }
                type = optional_function_get_type(valref, type);
                /* XXX what if type is not NULL but primitive ? */
                if (!type && valref->next) {
                    sprintf(error_msg, "trying to access member of something "
                                       "that is not a structure or an object");
                    return false;
                }
                return _context_valref_is_valid(ctx, valref->next, type,
                                                error_msg);
            }
            return false;
        } else {
            if (type->type != TYPE_TYPE_STRUCTURE) {
                sprintf(error_msg, "trying to access member of something "
                                   "that is not a structure or an object");
                return false;
            }

            structure_t* structure = type->structure_type;
            /* XXX not sure this could happen */
            if (!structure) {
                return false;
            }

            symbol_t* member = structure_find_member(structure,
                                                     &valref->identifier);

            if (!member) {
                sprintf(error_msg, "structure '%s' has no member called '%s'",
                        structure->identifier.value, valref->identifier.value);
                return false;
            }

            return _context_valref_is_valid(ctx, valref->next, member->is,
                                            error_msg);
        }
    }

    return true;
}

bool context_valref_is_valid(const context_t* ctx, const valref_t* valref,
                             char* error_msg)
{
    return _context_valref_is_valid(ctx, valref, NULL, error_msg);
}

bool context_value_is_valid(const context_t* ctx, const value_t* value,
                            char* error_msg)
{
    if (value->type == VALUE_TYPE_VALREF) {
        return context_valref_is_valid(ctx, value->valref, error_msg);
    }

    return true;
}

bool context_expression_is_valid(const context_t* ctx, const expression_t* e,
                                 char* error_msg)
{

    if (e->type == EXPRESSION_TYPE_VALUE) {
        return context_value_is_valid(ctx, &e->value, error_msg);
    } else
    if (e->type == EXPRESSION_TYPE_LAMBDA) {
        /* XXX maube have some checks here... */
        return true;
    }

    bool left = true;
    bool right = true;
    char err_msg_left[512];
    char err_msg_right[512];

    if (e->left) {
        left = context_expression_is_valid(ctx, e->left, err_msg_left);
    }

    if (e->right) {
        right = context_expression_is_valid(ctx, e->right, err_msg_right);
    }

    if (!left) {
        sprintf(error_msg, err_msg_left);
        return false;
    } else
    if (!right) {
        sprintf(error_msg, err_msg_right);
        return false;
    }

    const type_t* ltype = NULL;
    const type_t* rtype = NULL;

    if (e->left) {
        ltype = context_expression_get_type(ctx, e->left);
    }

    if (e->right) {
        rtype = context_expression_get_type(ctx, e->right);
    }

    if (ltype && !types_are_equivalent(ltype, rtype)) {
        sprintf(error_msg, "types are not equivalent");
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
        if (!type_is_number(ltype)) {
            sprintf(error_msg, "comparing something that is not a number");
            return false;
        }
        return true;
    }

    if (e->type == EXPRESSION_TYPE_BOOL_OP_AND
    ||  e->type == EXPRESSION_TYPE_BOOL_OP_OR
    ||  e->type == EXPRESSION_TYPE_BOOL_OP_NOT) {
        if (rtype->type != TYPE_TYPE_BOOLEAN) {
            sprintf(error_msg, "'and', 'or' or 'not' with something that is "
                               "not a boolean expression");
            return false;
        }
        return true;
    }

    if (e->type == EXPRESSION_TYPE_ARITHMETIC_OP_PLUS) {
        if (!type_is_number(ltype) && ltype->type != TYPE_TYPE_STRING) {
            sprintf(error_msg, "operator '+' could only be used with strings "
                               "and numbers");
            return false;
        }
        return true;
    }

    if (e->type == EXPRESSION_TYPE_ARITHMETIC_OP_MINUS
    ||  e->type == EXPRESSION_TYPE_ARITHMETIC_OP_MUL
    ||  e->type == EXPRESSION_TYPE_ARITHMETIC_OP_DIV) {
        if (!type_is_number(ltype)) {
            sprintf(error_msg, "'*', '-' and '/' operators are only usable "
                               "between numbers");
            return false;
        }
        return true;
    }

    if (e->type == EXPRESSION_TYPE_ARITHMETIC_OP_MOD)
    {
        if (!type_is_integer(ltype) || !type_is_integer(rtype)) {
            sprintf(error_msg, "'%%' is only usable with integers.");
            return false;
        }
        return true;
    }

    sprintf(error_msg, "the expression is invalid");
    return false;
}

bool context_parameters_are_valid(const context_t* ctx,
                                  const parameters_t* p,
                                  char* error_msg)
{
    char expr_err_msg[512];

    for (size_t i = 0; i < p->parameters.size; i++) {
        if (!context_expression_is_valid(ctx, vector_get(&p->parameters, i),
                                         expr_err_msg))
        {
            sprintf(error_msg, "invalid parameter '%zu': %s",
                                i, expr_err_msg);
            return false;
        }
    }

    return true;
}

bool context_affectation_is_valid(const context_t* ctx,
                                  const affectation_instr_t* affectation,
                                  char* error_msg)
{
    char sub_err_msg[512];

    // NOTE : We must RE-check the expression ...
    if (!context_expression_is_valid(ctx, affectation->expression,
        sub_err_msg))
    {
        sprintf(error_msg, "invalid affectation expression : %s",
                           sub_err_msg);
        return false;
    }

    if (!context_valref_is_valid(ctx, affectation->lvalue, sub_err_msg)) {
        sprintf(error_msg, "invalid affectation left-value : %s",
                           sub_err_msg);
        return false;
    }

    if (!types_are_equivalent(
                context_valref_get_type(ctx, affectation->lvalue),
                context_expression_get_type(ctx, affectation->expression)))
    {
        sprintf(error_msg, "expression has not the same type the left-value");
        return false;
    }

    return true;
}

static const type_t* _context_valref_get_type(const context_t* ctx,
                                      const valref_t* valref,
                                      const type_t* type)
{
    if (!valref) {
        return type;
    }

    if (!type) {
        if (valref->is_funccall) {
            function_t* func = program_find_function(ctx->program,
                                                     &valref->identifier);
            function_t* proc = program_find_procedure(ctx->program,
                                                      &valref->identifier);
            function_signature_t* lambda = context_find_lambda_function(ctx,
                                                &valref->identifier);

            if (func) {
                type = func->return_type;
            } else
            if (proc) {
                type = proc->return_type;
            } else {
                type = lambda->return_type;
            }
        } else {
            type = context_find_identifier_type(ctx, &valref->identifier);
        }
        return _context_valref_get_type(ctx, valref->next, type);
    } else {
        if (valref->is_funccall) {
            if (type->type == TYPE_TYPE_VECTOR) {
                type = vector_function_get_type(valref, type);
            } else
            if (type->type == TYPE_TYPE_OPTIONAL) {
                type = optional_function_get_type(valref, type);
            }
            return _context_valref_get_type(ctx, valref->next, type);
        } else {
            structure_t* structure = type->structure_type;
            symbol_t* member = structure_find_member(structure,
                                                     &valref->identifier);
            return _context_valref_get_type(ctx, valref->next, member->is);
        }
    }

    return type;
}

const type_t* context_valref_get_type(const context_t* ctx,
                                      const valref_t* valref)
{
    return _context_valref_get_type(ctx, valref, NULL);
}

const type_t* context_value_get_type(const context_t* ctx,
                                     const value_t* value) {
    if (!value) {
        return NULL;
    }

    switch (value->type) {
      case VALUE_TYPE_CHAR:
        return type_char;

      case VALUE_TYPE_STRING:
        return type_string;

      case VALUE_TYPE_REAL:
        return type_real;

      case VALUE_TYPE_INTEGER:
        return type_integer;

      case VALUE_TYPE_NATURAL:
        return type_natural;

      case VALUE_TYPE_BOOLEAN:
        return type_boolean;

      case VALUE_TYPE_VALREF:
        return context_valref_get_type(ctx, value->valref);
    }

    return NULL;
}

const type_t* context_expression_get_type(const context_t* ctx,
                                  const expression_t* expr)
{
    if (!expr) {
        return NULL;
    }

    if (expr->type == EXPRESSION_TYPE_VALUE) {
        return context_value_get_type(ctx, &expr->value);
    } else if (expr->type == EXPRESSION_TYPE_LAMBDA) {
        return function_get_type((function_t*)expr->lambda);
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

access_type_t context_value_get_access_type(const context_t* ctx,
                                                  const value_t* v) {

    if (v->type != VALUE_TYPE_VALREF) {
        return ACCESS_TYPE_INPUT;
    }

    return context_valref_get_access_type(ctx, v->valref);
}

access_type_t context_valref_get_access_type(const context_t* ctx,
                                                   const valref_t* v) {

    if (ctx->function) {
        function_arg_t* arg = function_find_arg(ctx->function,
                                                &v->identifier);

        if (!arg) {
            symbol_t* local = function_find_local(ctx->function,
                                                  &v->identifier);

            if (local) {
                return ACCESS_TYPE_INPUT_OUTPUT;
            }
        } else {
            return arg->access_type;
        }
    }

    if (ctx->program) {
        constant_t* constant = program_find_constant(ctx->program,
                                                &v->identifier);
        if (!constant) {
            return ACCESS_TYPE_INPUT_OUTPUT;
        }
    }

    return ACCESS_TYPE_INPUT;
}

function_signature_t* context_find_lambda_function(const context_t* ctx,
                                                   const identifier_t* id)
{
    if (ctx->function) {
        function_arg_t* arg = function_find_arg(ctx->function, id);
        if (arg && arg->symbol->is->type == TYPE_TYPE_FUNCTION) {
            return arg->symbol->is->signature;
        }

        symbol_t* sym = function_find_local(ctx->function, id);
        if (sym && sym->is->type == TYPE_TYPE_FUNCTION) {
            return sym->is->signature;
        }
    }

    symbol_t* sym = program_find_global(ctx->program, id);
    if (sym && sym->is->type == TYPE_TYPE_FUNCTION) {
        return sym->is->signature;
    }

    return NULL;
}

