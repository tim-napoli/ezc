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
    return PARSER_SUCCESS;
}

