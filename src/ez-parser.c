#include "ez-parser.h"

static parser_status_t comment_parser(FILE* input, const void* unused_args,
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

static parser_status_t empty_parser(FILE* input, const void* unused_args,
                                    void* unused_output)
{
    PARSE(char_parser(input, " \t\n\r", NULL));
    return PARSER_SUCCESS;
}

static parser_status_t space_parser(FILE* input, const void* unused_args,
                                    void* unused_output)
{
    PARSE(char_parser(input, " \t", NULL));
    return PARSER_SUCCESS;
}

static parser_status_t comment_or_empty_parser(FILE* input,
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

static parser_status_t end_of_line_parser(FILE* input, const void* args,
                                          void* unused_output)
{
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(char_parser(input, "\n", NULL));
    return PARSER_SUCCESS;
}

static parser_status_t identifier_parser(FILE* input, const void* args,
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

static parser_status_t header_parser(FILE* input,
                                     const void* unused_args,
                                     char** program_name)
{
    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));
    PARSE_ERR(word_parser(input, "program", NULL),
              "the program must begin with `program` keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(identifier_parser(input, NULL, program_name));
    PARSE(end_of_line_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

static parser_status_t modifier_parser(FILE* input, const void* args,
                                       void* output)
{
    if (TRY(input, word_parser(input, "in", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "out", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "inout", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

static parser_status_t type_parser(FILE* input, const void* args,
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


static parser_status_t function_args_parser(FILE* input, const void* args,
                                            void* output)
{
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    if (TRY(input, modifier_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        PARSE_ERR(space_parser(input, NULL, NULL),
                  "expected spaces");
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(identifier_parser(input, NULL, NULL),
                  "a valid identifier must follow a modifier in function "
                  "arguments");
        PARSE(space_parser(input, NULL, NULL));
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(word_parser(input, "is", NULL),
                  "expected 'is'");
        PARSE(space_parser(input, NULL, NULL));
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(type_parser(input, NULL, NULL),
                  "expected valid type");
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        if (TRY(input, char_parser(input, ",", NULL)) == PARSER_SUCCESS) {
            PARSE(function_args_parser(input, args, output));
        }
    }

    return PARSER_SUCCESS;
}

static parser_status_t local_variables_parser(FILE* input, const void* args,
                                              void* output)
{
    if (TRY(input, word_parser(input, "local", NULL)) == PARSER_SUCCESS) {
        PARSE_ERR(space_parser(input, NULL, NULL),
                  "expected space after 'local'");
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(identifier_parser(input, NULL, NULL),
                  "a valid identifier must follow a 'local' declaration");

        PARSE_ERR(space_parser(input, NULL, NULL),
                  "expected space after local variable identifier");
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(word_parser(input, "is", NULL),
                  "expected 'is' after local variable identifier");
        PARSE_ERR(space_parser(input, NULL, NULL),
                  "expected space after 'is'");
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(type_parser(input, NULL, NULL),
                  "expected valid type for variable ''");

        PARSE_ERR(end_of_line_parser(input, NULL, NULL),
                  "a new line must follow a local variable declaration");

        SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));
        PARSE(local_variables_parser(input, NULL, NULL));
    }

    return PARSER_SUCCESS;
}

static parser_status_t function_parser(FILE* input, const void* args,
                                       void* output)
{
    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));
    PARSE(word_parser(input, "function", NULL));

    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE_ERR(identifier_parser(input, NULL, NULL),
              "a function must have a valid identifier");

    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE_ERR(char_parser(input, "(", NULL), "missing '('");
    PARSE(function_args_parser(input, NULL, NULL));
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE_ERR(char_parser(input, ")", NULL), "missing ')'");

    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE_ERR(char_parser(input, ":", NULL), "missing ':'");
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(type_parser(input, NULL, NULL));
    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after a function head");

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));
    PARSE(local_variables_parser(input, NULL, NULL));

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));
    PARSE_ERR(word_parser(input, "begin", NULL),
              "function's variable declaration must be followed by a "
              "'begin' keyword");
    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a newline is expected after the 'begin' keyword");

    /* TODO instructions parser */

    PARSE(until_word_parser(input, "end", NULL));
    PARSE_ERR(word_parser(input, "end", NULL),
              "A function must be ended with 'end' keyword");
    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a newline is expected after the 'end' keyword");

    return PARSER_SUCCESS;
}

parser_status_t program_parser_func(FILE* input,
                                    const void* unused_args,
                                    void* unused_output)
{
    char program_name[256];
    char *output_ptr = program_name;

    PARSE(header_parser(input, unused_args, &output_ptr));
    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    printf("Program name: %s\n", program_name);

    /* TODO entity parser */
    PARSE(function_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

