#include "ez-parser.h"

parser_status_t print_parser(FILE* input, const void* args,
                             void* output)
{
    PARSE(word_parser(input, "print", NULL));
    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'print' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(parameters_parser(input, NULL, NULL));
    PARSE(end_of_line_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t return_parser(FILE* input, const void* args,
                              void* output)
{
    PARSE(word_parser(input, "return", NULL));
    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'return' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE_ERR(expression_parser(input, NULL, NULL),
              "bad return expression");
    PARSE(end_of_line_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t if_parser(FILE* input, const void* args,
                          void* output)
{
    PARSE(word_parser(input, "if", NULL));
    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'if' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(expression_parser(input, NULL, NULL));
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(word_parser(input, "then", NULL));
    PARSE(end_of_line_parser(input, NULL, NULL));

    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(instructions_parser(input, NULL, NULL));

    /* TODO elsif / else parser */

    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(word_parser(input, "endif", NULL));
    PARSE(end_of_line_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t on_parser(FILE* input, const void* args,
                          void* output)
{
    PARSE(word_parser(input, "on", NULL));
    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'on' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE_ERR(expression_parser(input, NULL, NULL),
              "an expression must follow a 'on' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE_ERR(word_parser(input, "do", NULL),
              "a 'do' keyword must follow the 'on' boolean expression");
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(instruction_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t flowcontrol_parser(FILE* input, const void* args,
                                   void* output)
{
    if (TRY(input, if_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, on_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t affectation_parser(FILE* input, const void* args,
                                   void* output)
{
    PARSE(varref_parser(input, NULL,  NULL));
    PARSE(space_parser(input, NULL, NULL));
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(char_parser(input, "=", NULL));
    PARSE(space_parser(input, NULL, NULL));
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(expression_parser(input, NULL, NULL));
    PARSE(end_of_line_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t instruction_parser(FILE* input, const void* args,
                                   void* output)
{
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    if (TRY(input, flowcontrol_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, affectation_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, print_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, return_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t instructions_parser(FILE* input, const void* args,
                                    void* output)
{
    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));
    if (TRY(input, instruction_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        PARSE(instructions_parser(input, NULL, NULL));
        return PARSER_SUCCESS;
    }

    return PARSER_SUCCESS;
}

