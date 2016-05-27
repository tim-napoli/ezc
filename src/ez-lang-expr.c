#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ez-lang.h"

int expression_type_predecence[EXPRESSION_TYPE_SIZE] = {
    [EXPRESSION_TYPE_VALUE] = 0,

    [EXPRESSION_TYPE_CMP_OP_EQUALS]             = 7,
    [EXPRESSION_TYPE_CMP_OP_DIFFERENT]          = 7,
    [EXPRESSION_TYPE_CMP_OP_LOWER_OR_EQUALS]    = 6,
    [EXPRESSION_TYPE_CMP_OP_GREATER_OR_EQUALS]  = 6,
    [EXPRESSION_TYPE_CMP_OP_LOWER]              = 6,
    [EXPRESSION_TYPE_CMP_OP_GREATER]            = 6,

    [EXPRESSION_TYPE_BOOL_OP_AND]               = 11,
    [EXPRESSION_TYPE_BOOL_OP_OR]                = 12,
    [EXPRESSION_TYPE_BOOL_OP_NOT]               = 2,

    [EXPRESSION_TYPE_ARITHMETIC_OP_PLUS]        = 4,
    [EXPRESSION_TYPE_ARITHMETIC_OP_MINUS]       = 4,
    [EXPRESSION_TYPE_ARITHMETIC_OP_MUL]         = 3,
    [EXPRESSION_TYPE_ARITHMETIC_OP_DIV]         = 3,
    [EXPRESSION_TYPE_ARITHMETIC_OP_MOD]         = 3,
};

expression_t* expression_new(expression_type_t type) {
    expression_t* expr = malloc(sizeof(expression_t));
    if (!expr) {
        fprintf(stderr, "couldn't allocate new expression\n");
        return NULL;
    }

    memset(expr, 0, sizeof(expression_t));
    expr->type = type;
    return expr;
}

void expression_delete(expression_t* expr) {
    if (!expr) {
        return;
    }

    switch (expr->type) {
      case EXPRESSION_TYPE_BOOL_OP_NOT:
        expression_delete(expr->right);
        break;

      case EXPRESSION_TYPE_VALUE:
        value_wipe(&expr->value);
        break;

      default:
        expression_delete(expr->left);
        expression_delete(expr->right);
        break;
    }

    free(expr);
}

int expression_predecence(const expression_t* expr) {
    return expression_type_predecence[expr->type];
}

static char* expression_type_symbols[EXPRESSION_TYPE_SIZE] = {
    [EXPRESSION_TYPE_VALUE] = "",

    [EXPRESSION_TYPE_CMP_OP_EQUALS]             = "==",
    [EXPRESSION_TYPE_CMP_OP_DIFFERENT]          = "!=",
    [EXPRESSION_TYPE_CMP_OP_LOWER_OR_EQUALS]    = "<=",
    [EXPRESSION_TYPE_CMP_OP_GREATER_OR_EQUALS]  = ">=",
    [EXPRESSION_TYPE_CMP_OP_LOWER]              = "<",
    [EXPRESSION_TYPE_CMP_OP_GREATER]            = ">",

    [EXPRESSION_TYPE_BOOL_OP_AND]               = "&&",
    [EXPRESSION_TYPE_BOOL_OP_OR]                = "||",
    [EXPRESSION_TYPE_BOOL_OP_NOT]               = "!",

    [EXPRESSION_TYPE_ARITHMETIC_OP_PLUS]        = "+",
    [EXPRESSION_TYPE_ARITHMETIC_OP_MINUS]       = "-",
    [EXPRESSION_TYPE_ARITHMETIC_OP_MUL]         = "*",
    [EXPRESSION_TYPE_ARITHMETIC_OP_DIV]         = "/",
    [EXPRESSION_TYPE_ARITHMETIC_OP_MOD]         = "%",
};

void expression_print(FILE* output, const context_t* ctx,
                      const expression_t* expr)
{
    if (!expr) {
        return;
    }

    if (expr->type == EXPRESSION_TYPE_VALUE) {
        value_print(output, ctx, &expr->value);
    } else {
        if (expr->left) {
            fputc('(', output);
            expression_print(output, ctx, expr->left);
            fputc(')', output);
        }

        fprintf(output, " %s ", expression_type_symbols[expr->type]);
        if (expr->type == EXPRESSION_TYPE_VALUE) {
            value_print(output, ctx, &expr->value);
        }

        if (expr->right) {
            fputc('(', output);
            expression_print(output, ctx, expr->right);
            fputc(')', output);
        }
    }
}

const type_t* expression_get_type(const context_t* ctx,
                                  const expression_t* expr)
{
    if (expr->type == EXPRESSION_TYPE_VALUE) {
        return value_get_type(ctx, &expr->value);
    } else if (expr->type == EXPRESSION_TYPE_BOOL_OP_NOT) {
        return type_boolean;
    }

    if (expr->type >= EXPRESSION_TYPE_CMP_OP_EQUALS
    &&  expr->type <= EXPRESSION_TYPE_BOOL_OP_OR)
    {
        return type_boolean;
    }

    const type_t* left_type = expression_get_type(ctx, expr->left);
    const type_t* right_type = expression_get_type(ctx, expr->right);

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

