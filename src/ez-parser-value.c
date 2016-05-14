#include "ez-parser.h"

parser_status_t string_parser(FILE* input, const void* args,
                              char** output)
{
    char string[512];
    char *string_ptr = string;

    PARSE(char_parser(input, "\"", NULL));
    PARSE(until_char_parser(input, "\"", &string_ptr));

    /* Handling " escaping */
    /* XXX find something else than this trick */
    while (*(string_ptr - 1) == '\\') {
        PARSE(char_parser(input, "\"", &string_ptr));
        PARSE(until_char_parser(input, "\"", &string_ptr));
    }
    PARSE_ERR(char_parser(input, "\"", NULL),
              "unclosed string");
    return PARSER_SUCCESS;
}

parser_status_t integer_parser(FILE* input, const void* args,
                               char** output)
{
    if (TRY(input, char_parser(input, "-", NULL)) == PARSER_SUCCESS) {
        // TODO negative integer
    }
    PARSE(chars_parser(input, "0123456789", output));
    return PARSER_SUCCESS;
}

parser_status_t bool_parser(FILE* input, const void* args,
                            char** output)
{
    if (TRY(input, word_parser(input, "true", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    }
    PARSE(word_parser(input, "false", NULL));
    return PARSER_SUCCESS;
}

parser_status_t varref_parser(FILE* input, const void* args,
                              void* output)
{
    PARSE(identifier_parser(input, NULL, NULL));
    if (TRY(input, char_parser(input, ".", NULL)) == PARSER_SUCCESS) {
        PARSE_ERR(varref_parser(input, NULL, NULL),
                  "expected identifier after '.'");
    }

    return PARSER_SUCCESS;
}

parser_status_t parameters_parser(FILE* input, const void* args,
                                  void* output)
{
    if (TRY(input, expression_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        if (TRY(input, char_parser(input, ",", NULL)) == PARSER_SUCCESS) {
            SKIP_MANY(input, space_parser(input, NULL, NULL));
            PARSE_ERR(parameters_parser(input, NULL, NULL),
                      "invalid parameter");
        }
    }

    return PARSER_SUCCESS;
}

parser_status_t valref_parser(FILE* input, const void* args,
                              void* output)
{
    PARSE(identifier_parser(input, NULL, NULL));

    /* XXX find a better way to do this function... */
    if (TRY(input, char_parser(input, ".", NULL)) == PARSER_SUCCESS) {
        /* Continue with a valref */
        PARSE_ERR(valref_parser(input, NULL, NULL),
                  "expected identifier after '.'");
    } else {
        /* Check if this is a function call */
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        if (TRY(input, char_parser(input, "(", NULL)) == PARSER_SUCCESS) {
            SKIP_MANY(input, space_parser(input, NULL, NULL));
            PARSE(parameters_parser(input, NULL, NULL));
            SKIP_MANY(input, space_parser(input, NULL, NULL));
            PARSE_ERR(char_parser(input, ")", NULL),
                      "unclosed function call");
            /* TODO check if 'identifier' is a function and not a procedure */
            if (TRY(input, char_parser(input, ".", NULL)) == PARSER_SUCCESS) {
                /* Continue with a valref */
                PARSE_ERR(valref_parser(input, NULL, NULL),
                          "expected identifier after '.'");
            }
        }
        while (TRY(input, char_parser(input, "[", NULL)) == PARSER_SUCCESS) {
            /* Check is this is a array/vector/map indexing */
            /* TODO check if 'identifier' is a array/vector value */
            SKIP_MANY(input, space_parser(input, NULL, NULL));
            PARSE_ERR(expression_parser(input, NULL, NULL),
                      "a valid expression must follow '['");
            SKIP_MANY(input, space_parser(input, NULL, NULL));
            PARSE_ERR(char_parser(input, "]", NULL),
                      "missing ']'");
            SKIP_MANY(input, space_parser(input, NULL, NULL));
            if (TRY(input, char_parser(input, ".", NULL)) == PARSER_SUCCESS) {
                /* Continue with a valref */
                PARSE_ERR(valref_parser(input, NULL, NULL),
                          "expected identifier after '.'");
            }
            SKIP_MANY(input, space_parser(input, NULL, NULL));
        }
    }

    return PARSER_SUCCESS;
}

parser_status_t funccall_parser(FILE* input, const void* args,
                                void* output)
{
    PARSE(varref_parser(input, NULL, NULL));
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(char_parser(input, "(", NULL));
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(parameters_parser(input, NULL, NULL));
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(char_parser(input, ")", NULL));

    return PARSER_SUCCESS;
}

parser_status_t value_parser(FILE* input, const void* args,
                             void* output)
{
    if (TRY(input, string_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, integer_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, bool_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, string_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, valref_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

