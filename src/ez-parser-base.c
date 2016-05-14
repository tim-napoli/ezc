#include "ez-parser.h"

parser_status_t comment_parser(FILE* input, const void* unused_args,
                               void* unused_output)
{
    if ((TRY(input, word_parser(input, "/*", NULL))) == PARSER_SUCCESS)
    {
        PARSE(until_word_parser(input, "*/", NULL));
        PARSE(word_parser(input, "*/", NULL));
        return PARSER_SUCCESS;
    } else {
        PARSE(word_parser(input, "//", NULL));
        PARSE(until_char_parser(input, "\n", NULL));
        PARSE(char_parser(input, "\n", NULL));
        return PARSER_SUCCESS;
    }
    return PARSER_FAILURE;
}

parser_status_t empty_parser(FILE* input, const void* unused_args,
                             void* unused_output)
{
    PARSE(char_parser(input, " \t\n\r", NULL));
    return PARSER_SUCCESS;
}

parser_status_t space_parser(FILE* input, const void* unused_args,
                             void* unused_output)
{
    PARSE(char_parser(input, " \t", NULL));
    return PARSER_SUCCESS;
}

parser_status_t comment_or_empty_parser(FILE* input,
                                        const void* unused_args,
                                        void* unused_output)
{
    if (TRY(input, comment_parser(input, NULL, NULL)) == PARSER_SUCCESS)
    {
        return PARSER_SUCCESS;
    }
    PARSE(empty_parser(input, NULL, NULL));
    return PARSER_SUCCESS;
}

parser_status_t end_of_line_parser(FILE* input, const void* args,
                                   void* unused_output)
{
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(char_parser(input, "\n", NULL));
    return PARSER_SUCCESS;
}

parser_status_t identifier_parser(FILE* input, const void* args,
                                  char** output)
{
    const char id_charset_first[] = "azertyuiopqsdfghjklmwxcvbn"
                                    "AZERTYUIOPQSDFGHJKLMWXCVBN"
                                    "_";
    const char id_charset[] = "azertyuiopqsdfghjklmwxcvbn"
                              "AZERTYUIOPQSDFGHJKLMWXCVBN"
                              "0123456789"
                              "_";
    PARSE(char_parser(input, id_charset_first, output));
    PARSE_MANY(input, char_parser(input, id_charset, output));
    return PARSER_SUCCESS;
}

parser_status_t type_parser(FILE* input, const void* args,
                            void* output)
{
    if (TRY(input, word_parser(input, "integer", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "natural", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "real", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "string", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "vector", NULL)) == PARSER_SUCCESS) {
        PARSE_ERR(space_parser(input, NULL, NULL),
                  "expected spaces after 'vector'");
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(word_parser(input, "of", NULL),
                  "expected 'of' after vector");

        PARSE_ERR(space_parser(input, NULL, NULL),
                  "expected spaces after 'of'");
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE(type_parser(input, NULL, NULL));

        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t variable_tail_parser(FILE* input, const void* args,
                                     void* output)
{
    PARSE(identifier_parser(input, NULL, NULL));
    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a variable identifier");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "is", NULL),
              "a variable declaration must have a 'is' keyword");
    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a variable 'is' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(type_parser(input, NULL, NULL),
              "a variable must have a valid type");
    return PARSER_SUCCESS;
}

parser_status_t range_parser(FILE* input, const void* args,
                             void* output)
{
    PARSE(integer_parser(input, NULL, NULL));
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(word_parser(input, "..", NULL));
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE_ERR(integer_parser(input, NULL, NULL),
              "an integer is expected after range '..'");

    return PARSER_SUCCESS;
}

