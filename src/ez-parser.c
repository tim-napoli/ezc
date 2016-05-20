#include "ez-parser.h"
#include "ez-lang.h"

parser_status_t header_parser(FILE* input,
                              const void* unused_args,
                              identifier_t* program_id)
{
    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));
    PARSE_ERR(word_parser(input, "program", NULL),
              "the program must begin with `program` keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE_ERR(identifier_parser(input, NULL, program_id),
              "the program name must be a valid identifier");
    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow the program name");

    return PARSER_SUCCESS;
}

parser_status_t modifier_parser(FILE* input, const void* args,
                                void* output)
{
    if (TRY(input, word_parser(input, "in", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    }

    if (TRY(input, word_parser(input, "out", NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    }

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

parser_status_t local_parser(FILE* input, const void* args,
                                       symbol_t **output)
{
    PARSE(word_parser(input, "local", NULL));
    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a 'global' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(variable_tail_parser(input, NULL, output));
    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after a global declaration");

    return PARSER_SUCCESS;
}

parser_status_t function_parser(FILE* input, const void* args,
                                void* output)
{
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

    while (TRY(input, word_parser(input, "begin", NULL)) == PARSER_FAILURE) {
        PARSE(local_parser(input, NULL, NULL));
        SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));
    }

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

parser_status_t procedure_parser(FILE* input, const void* args,
                                 void* output)
{
    PARSE(word_parser(input, "procedure", NULL));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(identifier_parser(input, NULL, NULL),
              "a procedure must have a valid identifier");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(char_parser(input, "(", NULL), "missing '('");

    PARSE(function_args_parser(input, NULL, NULL));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(char_parser(input, ")", NULL), "missing ')'");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after a procedure head");

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    while (TRY(input, word_parser(input, "begin", NULL)) == PARSER_FAILURE) {
        PARSE(local_parser(input, NULL, NULL));
        SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));
    }

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a newline is expected after the 'begin' keyword");

    PARSE(instructions_parser(input, NULL, NULL));

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "end", NULL),
              "A procedure must be ended with 'end' keyword");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a newline is expected after the 'end' keyword");

    return PARSER_SUCCESS;
}

parser_status_t constant_parser(FILE* input,
                                const void* unused_args,
                                void* unused_output)
{
    PARSE(word_parser(input, "constant", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a 'constant' keyword");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(variable_tail_parser(input, NULL, NULL));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(char_parser(input, "=", NULL),
              "a '=' is expected to intialize constant");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, NULL, NULL),
              "a valid expression is expected to initialize a constant");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after a constant declaration");

    return PARSER_SUCCESS;
}

parser_status_t global_parser(FILE* input,
                              const void* unused_args,
                              symbol_t **output)
{
    PARSE(word_parser(input, "global", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a 'global' keyword");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(variable_tail_parser(input, NULL, output));

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after a global declaration");

    return PARSER_SUCCESS;
}

parser_status_t structure_member_parser(FILE* input,
                                        const void* unused_args,
                                        symbol_t** output)
{
    PARSE(variable_tail_parser(input, NULL, output));

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the member type");

    return PARSER_SUCCESS;
}

parser_status_t structure_parser(FILE* input,
                                 const void* unused_args,
                                 structure_t **output)
{
    identifier_t id;

    PARSE(word_parser(input, "structure", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow the 'structure' keyword");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(identifier_parser(input, NULL, &id));

    *output = structure_new(&id);

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow the structure identifier");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "is", NULL),
              "a 'is' keyword must follow the structure identifier");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the structure 'is' keyword");

    SKIP_MANY(input, empty_parser(input, NULL, NULL));

    while (TRY(input, word_parser(input, "end", NULL)) == PARSER_FAILURE) {
        symbol_t *symbol;

        PARSE(structure_member_parser(input, NULL, &symbol));

        SKIP_MANY(input, empty_parser(input, NULL, NULL));

        structure_add_member(*output, symbol);
    }

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the structure 'end' keyword");

    return PARSER_SUCCESS;
}

parser_status_t entity_parser(FILE* input, const void* unused_args,
                              void* output)
{
    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));
    if (TRY(input, constant_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, global_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, structure_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, function_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, procedure_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t program_parser(FILE* input,
                               const void* unused_args,
                               void* unused_output)
{
    identifier_t program_id;
    context_t *context;

    PARSE(header_parser(input, unused_args, &program_id));
    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    context = context_new(program_id, NULL);
    printf("Program name: %s\n", program_id.value);

    while (entity_parser(input, NULL, context) == PARSER_SUCCESS) {

    }

    return PARSER_SUCCESS;
}
