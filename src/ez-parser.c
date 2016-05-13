#include "ez-parser.h"

parser_status_t header_parser(FILE* input,
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

parser_status_t modifier_parser(FILE* input, const void* args,
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

parser_status_t function_args_parser(FILE* input, const void* args,
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

parser_status_t local_variables_parser(FILE* input, const void* args,
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

parser_status_t function_parser(FILE* input, const void* args,
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
    PARSE_ERR(word_parser(input, "return", NULL), "missing 'return'");
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE_ERR(type_parser(input, NULL, NULL),
              "unknown return type");
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


    PARSE(instructions_parser(input, NULL, NULL));
    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "end", NULL),
              "A function must be ended with 'end' keyword");
    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a newline is expected after the 'end' keyword");

    return PARSER_SUCCESS;
}

parser_status_t structure_member_parser(FILE* input,
                                        const void* unused_args,
                                        void* unused_output)
{
    PARSE(identifier_parser(input, NULL, NULL));
    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a member identifier");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(word_parser(input, "is", NULL));
    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a member 'is' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(type_parser(input, NULL, NULL),
              "a structure member must have a valid type");
    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the member type");

    return PARSER_SUCCESS;
}

parser_status_t structure_parser(FILE* input,
                                 const void* unused_args,
                                 void* unused_output)
{
    PARSE(word_parser(input, "structure", NULL));
    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow the 'structure' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(identifier_parser(input, NULL, NULL));
    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow the structure identifier");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "is", NULL),
              "a 'is' keyword must follow the structure identifier");
    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the structure 'is' keyword");

    SKIP_MANY(input, empty_parser(input, NULL, NULL));
    while (TRY(input, word_parser(input, "end", NULL)) == PARSER_FAILURE) {
        PARSE(structure_member_parser(input, NULL, NULL));
        SKIP_MANY(input, empty_parser(input, NULL, NULL));
    }

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the structure 'end' keyword");

    return PARSER_SUCCESS;
}

parser_status_t program_parser(FILE* input,
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

