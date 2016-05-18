#include "ez-parser.h"

parser_status_t cmp_op_parser(FILE* input, const void* args,
                              void* output)
{
    if (TRY(input, word_parser(input, "==", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "!=", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "<=", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, ">=", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, ">", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "<", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t bool_op_parser(FILE* input, const void* args,
                               void* output)
{
    if (TRY(input, word_parser(input, "and", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "or", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t arithmetic_op_parser(FILE* input, const void* args,
                                     void* output)
{
    if (TRY(input, word_parser(input, "+", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "-", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "*", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "/", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "%", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t expression_next_parser(FILE* input, const void* args,
                                       void* output)
{
    if (TRY(input, arithmetic_op_parser(input, NULL, NULL)) == PARSER_SUCCESS)
    {
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_parser(input, NULL, NULL));
    } else
    if (TRY(input, bool_op_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_parser(input, NULL, NULL));
    } else
    if (TRY(input, cmp_op_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_parser(input, NULL, NULL));
    }

    return PARSER_SUCCESS;
}

parser_status_t expression_parser(FILE* input, const void* args,
                                  void* output)
{
    value_t value;

    if (TRY(input, char_parser(input, "(", NULL)) == PARSER_SUCCESS) {
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_parser(input, NULL, NULL));
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(char_parser(input, ")", NULL));
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_next_parser(input, NULL, NULL));
        return PARSER_SUCCESS;
    } else
    if (TRY(input, value_parser(input, NULL, &value)) == PARSER_SUCCESS) {
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_next_parser(input, NULL, NULL));
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "not", NULL)) == PARSER_SUCCESS) {
        PARSE_ERR(space_parser(input, NULL, NULL),
                  "a space must follow a 'not' keyword");
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE(expression_parser(input, NULL, NULL));
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

