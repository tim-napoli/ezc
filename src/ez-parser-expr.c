#include <string.h>
#include <assert.h>
#include "ez-lang.h"
#include "ez-parser.h"
#include "ez-lang-errors.h"

#define EXPR_STACK_MAX_SYMBOLS      128
#define EXPR_STACK_MAX_OPERATORS    128

typedef struct expr_stacks {
    int nleaves;
    expression_t* leaves[EXPR_STACK_MAX_SYMBOLS];

    int noperators;
    expression_type_t operators[EXPR_STACK_MAX_OPERATORS];
} expr_stacks_t;

static parser_status_t cmp_op_parser(FILE* input, const void* args,
                                     expression_type_t* type)
{
    if (TRY(input, word_parser(input, "==", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_CMP_OP_EQUALS;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "!=", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_CMP_OP_DIFFERENT;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "<=", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_CMP_OP_LOWER_OR_EQUALS;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, ">=", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_CMP_OP_GREATER_OR_EQUALS;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, ">", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_CMP_OP_GREATER;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "<", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_CMP_OP_LOWER;
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

static parser_status_t bool_op_parser(FILE* input, const void* args,
                                      expression_type_t* type)
{
    if (TRY(input, word_parser(input, "and", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_BOOL_OP_AND;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "or", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_BOOL_OP_OR;
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

static parser_status_t arithmetic_op_parser(FILE* input, const void* args,
                                            expression_type_t* type)
{
    if (TRY(input, word_parser(input, "+", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_ARITHMETIC_OP_PLUS;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "-", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_ARITHMETIC_OP_MINUS;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "*", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_ARITHMETIC_OP_MUL;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "/", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_ARITHMETIC_OP_DIV;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "%", NULL)) == PARSER_SUCCESS) {
        *type = EXPRESSION_TYPE_ARITHMETIC_OP_MOD;
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

static parser_status_t lambda_parser(FILE* input, const context_t* ctx,
                                     function_t** lambda)
{
    context_t sub_ctx = (context_t){
        .program = ctx->program,
        .function = NULL,
    };

    PARSE(word_parser(input, "lambda", NULL));
    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space is expected after 'lambda' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(char_parser(input, "(", NULL),
              "a '(' is expected after 'lambda' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    identifier_t id = {.value = ""};
    *lambda = function_new(&id);
    sub_ctx.function = *lambda;

    PARSE_ERR(function_args_parser(input, ctx, &(*lambda)->args),
              "invalid lambda parameters");
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE_ERR(char_parser(input, ")", NULL),
              "a ')' is expected after 'lambda' arguments");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "return", NULL),
              "a lambda function must have a return type");
    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space is expected after lambda return keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(type_parser(input, &sub_ctx, &(*lambda)->return_type),
              "invalid lambda return type");

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space is expected after lambda return type");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "is", NULL),
              "a 'is' keyword is expected after lambda return type");
    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space is expected after lambda 'is' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    expression_t* return_expr = NULL;
    instruction_t* instr = NULL;
    PARSE_ERR(return_parser(input, &sub_ctx, &return_expr),
              "a 'return' instruction is expected for lambda function");
    instr = instruction_new(INSTRUCTION_TYPE_RETURN);
    instr->expression = return_expr;
    vector_push(&(*lambda)->instructions, instr);

    return PARSER_SUCCESS;
}

static parser_status_t expression_in_parser(FILE* input, const context_t* ctx,
                                            expr_stacks_t* stacks);

static parser_status_t expression_next_parser(FILE* input, const context_t* ctx,
                                              expr_stacks_t* stacks)
{
    expression_type_t expr_type;

    if (TRY(input, arithmetic_op_parser(input, NULL, &expr_type))
        == PARSER_SUCCESS)
    {
        stacks->operators[stacks->noperators++] = expr_type;

        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_in_parser(input, ctx, stacks));
    } else
    if (TRY(input, bool_op_parser(input, NULL, &expr_type)) == PARSER_SUCCESS)
    {
        stacks->operators[stacks->noperators++] = expr_type;

        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_in_parser(input, ctx, stacks));
    } else
    if (TRY(input, cmp_op_parser(input, NULL, &expr_type)) == PARSER_SUCCESS)
    {
        stacks->operators[stacks->noperators++] = expr_type;

        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_in_parser(input, ctx, stacks));
    }

    return PARSER_SUCCESS;
}

static parser_status_t expression_in_parser(FILE* input, const context_t* ctx,
                                            expr_stacks_t* stacks)
{
    value_t value;
    char sub_err_msg[512];
    function_t* lambda = NULL;

    if (TRY(input, char_parser(input, "(", NULL)) == PARSER_SUCCESS) {
        expression_t* subexpr = NULL;

        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_parser(input, ctx, &subexpr));
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(char_parser(input, ")", NULL));

        stacks->leaves[stacks->nleaves++] = subexpr;

        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_next_parser(input, ctx, stacks));

        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "not ", NULL)) == PARSER_SUCCESS) {
        stacks->operators[stacks->noperators++] = EXPRESSION_TYPE_BOOL_OP_NOT;
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_in_parser(input, ctx, stacks));
        return PARSER_SUCCESS;
    } else
    if (TRY(input, lambda_parser(input, ctx, &lambda)) == PARSER_SUCCESS) {
        expression_t* expr = expression_new(EXPRESSION_TYPE_LAMBDA);
        expr->lambda = lambda;
        stacks->leaves[stacks->nleaves++] = expr;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, value_parser(input, ctx, &value)) == PARSER_SUCCESS) {

        /* NOTE CHECK */
        if (!context_value_is_valid(ctx, &value, sub_err_msg)) {
            error_value_not_valid(input, ctx, &value, sub_err_msg);
        } else
        if (!access_type_is_input(context_value_get_access_type(ctx, &value)))
        {
            error_bad_access_expr_value(input, &value);
        }

        SKIP_MANY(input, space_parser(input, NULL, NULL));

        expression_t* expr = expression_new(EXPRESSION_TYPE_VALUE);
        memcpy(&expr->value, &value, sizeof(value_t));
        stacks->leaves[stacks->nleaves++] = expr;

        PARSE(expression_next_parser(input, ctx, stacks));
        /* TODO wipe expression on error */

        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

static int expression_insert(expression_t** tree, expression_t* node)
{
    if (*tree == NULL) {
        *tree = node;
        return 0;
    } else
    if (expression_predecence(node) > expression_predecence(*tree)) {
        node->right = *tree;
        *tree = node;
        return 1;
    } else {
        return expression_insert(&(*tree)->left, node);
    }
}

static void expression_insert_at_left(expression_t** tree, expression_t* v)
{
    while (*tree != NULL) {
        tree = &(*tree)->left;
    }
    *tree = v;
}

static expression_t* expression_from_stack(expr_stacks_t* stacks)
{
    expression_t* tree = NULL;
    expression_t* expr = NULL;

    while (stacks->noperators > 0) {
        expression_type_t op = stacks->operators[--stacks->noperators];

        /* TODO if lambda here, then error (a lambda must be alone in the expression) */
        if (op == EXPRESSION_TYPE_BOOL_OP_NOT) {
            expression_t* prev_expr = expr;
            expr = expression_new(op);
            expr->right = stacks->leaves[stacks->nleaves - 1];
            stacks->leaves[stacks->nleaves - 1] = expr;
            expr = prev_expr;
        } else {
            expr = expression_new(op);
            if (expression_insert(&tree, expr) == 0) {
                expr->right = stacks->leaves[--stacks->nleaves];
            } else {
                expression_insert_at_left(&expr->right,
                                          stacks->leaves[--stacks->nleaves]);
            }
        }
    }

    assert (stacks->nleaves == 1);
    if (expr == NULL) {
        tree = stacks->leaves[--stacks->nleaves];
    } else {
        expr->left = stacks->leaves[--stacks->nleaves];
    }

    return tree;
}


parser_status_t expression_parser(FILE* input, const context_t* ctx,
                                  expression_t** expression)
{
    char sub_err_msg[512];
    expr_stacks_t stacks = {
        .nleaves = 0,
        .noperators = 0
    };

    PARSE(expression_in_parser(input, ctx, &stacks));
    *expression = expression_from_stack(&stacks);

    /* NOTE Check */
    if (!context_expression_is_valid(ctx, *expression, sub_err_msg)) {
        error_expression_not_valid(input, ctx, *expression, sub_err_msg);
    }

    return PARSER_SUCCESS;
}
