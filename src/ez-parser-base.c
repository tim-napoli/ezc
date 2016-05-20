#include <string.h>
#include "ez-parser.h"
#include "ez-lang.h"

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
                                  identifier_t* id)
{
    const char id_charset_first[] = "azertyuiopqsdfghjklmwxcvbn"
                                    "AZERTYUIOPQSDFGHJKLMWXCVBN"
                                    "_";
    const char id_charset[] = "azertyuiopqsdfghjklmwxcvbn"
                              "AZERTYUIOPQSDFGHJKLMWXCVBN"
                              "0123456789"
                              "_";
    char value[1024];
    char* value_ptr = value;

    PARSE(char_parser(input, id_charset_first, &value_ptr));

    PARSE_MANY(input, char_parser(input, id_charset, &value_ptr));

    if (strlen(value) >= IDENTIFIER_SIZE) {
        PARSER_LANG_ERR("identifier %s is too long (%d max chars)",
                        value, IDENTIFIER_SIZE);
    }

    strcpy(id->value, value);

    if (identifier_is_reserved(id)) {
        return PARSER_FAILURE;
    }

    return PARSER_SUCCESS;
}

parser_status_t type_parser(FILE* input, const void* args,
                            type_t **output)
{
    if (TRY(input, word_parser(input, "integer", NULL)) == PARSER_SUCCESS) {
        *output = type_integer_new();
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "natural", NULL)) == PARSER_SUCCESS) {
        *output = type_natural_new();
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "boolean", NULL)) == PARSER_SUCCESS) {
        *output = type_boolean_new();
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "real", NULL)) == PARSER_SUCCESS) {
        *output = type_real_new();
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "string", NULL)) == PARSER_SUCCESS) {
        *output = type_string_new();
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

        type_t *of;
        PARSE(type_parser(input, NULL, &of));

        *output = type_vector_new(of);

        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t variable_tail_parser(FILE* input, const void* args,
                                     symbol_t **output)
{
    identifier_t id;
    type_t *is = NULL;

    PARSE(identifier_parser(input, NULL, &id));

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a variable identifier");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "is", NULL),
              "a variable declaration must have a 'is' keyword");

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a variable 'is' keyword");

    SKIP_MANY(input, space_parser(input, NULL, &is));

    PARSE_ERR(type_parser(input, NULL, &is),
              "a variable must have a valid type");

    *output = symbol_new(&id, is);

    return PARSER_SUCCESS;
}

parser_status_t range_parser(FILE* input, const void* args,
                             void* output)
{
    expression_t* begin = NULL;
    expression_t* end = NULL;

    PARSE(expression_parser(input, NULL, &begin));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(word_parser(input, "..", NULL));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, NULL, &end),
              "a valid expression is expected after range '..'");

    return PARSER_SUCCESS;
}
