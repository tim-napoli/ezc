#include "ez-parser.h"

parser_status_t print_parser(FILE* input, const void* args,
                             void* output)
{
    parameters_t parameters;

    PARSE(word_parser(input, "print", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expected after 'print' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(parameters_parser(input, NULL, &parameters));

    PARSE(end_of_line_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t read_parser(FILE* input, const void* args,
                            void* output)
{
    identifier_t id;

    PARSE(word_parser(input, "read", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expected after 'read' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(identifier_parser(input, NULL, &id),
              "a single identifier must follow the 'read' keyword");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow the 'read' line");

    return PARSER_SUCCESS;
}

parser_status_t return_parser(FILE* input, const void* args,
                              void* output)
{
    expression_t* expression = NULL;
    PARSE(word_parser(input, "return", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'return' keyword");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, NULL, &expression),
              "bad return expression");

    PARSE(end_of_line_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t elsif_parser(FILE* input, const void* args,
                             elsif_instr_t** output)
{
    expression_t* coundition = NULL;

    PARSE(word_parser(input, "elsif", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'elsif' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(expression_parser(input, NULL, &coundition));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(word_parser(input, "then", NULL));

    PARSE(end_of_line_parser(input, NULL, NULL));

    *output = elsif_instr_new(coundition);

    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(instructions_parser(input, NULL, &(*output)->instructions));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t else_parser(FILE* input, const void* args, vector_t* output)
{
    PARSE(word_parser(input, "else", NULL));
    PARSE(end_of_line_parser(input, NULL, NULL));

    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(instructions_parser(input, NULL, output));
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t if_parser(FILE* input, const void* args,
                          if_instr_t** output)
{
    expression_t* coundition = NULL;

    PARSE(word_parser(input, "if", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'if' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE_ERR(expression_parser(input, NULL, &coundition),
              "'if' invalid expression");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "then", NULL),
              "missing 'then' keyword after 'if' expression");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "an end of line must follow the 'then' keyword");

    *output = if_instr_new(coundition);

    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(instructions_parser(input, NULL, &(*output)->instructions));

    SKIP_MANY(input, space_parser(input, NULL, NULL));
    elsif_instr_t* elsif = NULL;
    while (TRY(input, elsif_parser(input, NULL, &elsif)) == PARSER_SUCCESS) {
        vector_push(&(*output)->elsifs, elsif);
    }
    SKIP_MANY(input, space_parser(input, NULL, NULL));
    TRY(input, else_parser(input, NULL, &(*output)->else_instrs));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "endif", NULL),
              "a 'if' must be closed with the 'endif' keyword");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow the 'endif' keyword");

    return PARSER_SUCCESS;
}

parser_status_t on_parser(FILE* input, const void* args,
                          on_instr_t** output)
{
    expression_t* coundition = NULL;

    PARSE(word_parser(input, "on", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'on' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, NULL, &coundition),
              "an expression must follow a 'on' keyword");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "do", NULL),
              "a 'do' keyword must follow the 'on' boolean expression");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    *output = on_instr_new(coundition);
    PARSE(instruction_parser(input, NULL, &(*output)->instruction));

    return PARSER_SUCCESS;
}

parser_status_t while_parser(FILE* input, const void* args,
                             while_instr_t** output)
{
    expression_t* expr = NULL;

    PARSE(word_parser(input, "while", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'while' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, NULL, &expr),
              "a valid expression is expected after the 'while' keyword");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "do", NULL),
              "a 'do' keyword must follow the 'while' expression");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the while 'do' keyword");

    *output = while_instr_new(expr);

    PARSE(instructions_parser(input, NULL, &(*output)->instructions));
    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "endwhile", NULL),
              "a 'endwhile' keyword is expected to close a 'while' block");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow the 'endwhile' keyword");
    return PARSER_SUCCESS;
}

parser_status_t for_parser(FILE* input, const void* args,
                           for_instr_t** output)
{
    identifier_t id;

    PARSE(word_parser(input, "for", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'for' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(identifier_parser(input, NULL, &id),
              "a valid identifier is expected after the 'for' keyword");

    *output = for_instr_new(&id);

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after for identifier");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "in", NULL),
              "a 'in' keyword must follow the 'for' identifier");

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after for 'in' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(range_parser(input, NULL, &(*output)->range),
              "a valid range is expected after 'for' 'in' keyword");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "do", NULL),
              "a 'do' keyword must follow the 'for' range");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the for 'do' keyword");

    PARSE(instructions_parser(input, NULL, &(*output)->instructions));

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "endfor", NULL),
              "a 'endfor' keyword is expected to close a 'for' block");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow the 'endfor' keyword");

    return PARSER_SUCCESS;
}

parser_status_t loop_parser(FILE* input, const void* args,
                            loop_instr_t** output)
{
    expression_t* coundition = NULL;

    PARSE(word_parser(input, "loop", NULL));

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the 'loop' keyword");

    *output = loop_instr_new(NULL);

    PARSE(instructions_parser(input, NULL, &(*output)->instructions));

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "until", NULL),
              "a 'until' keyword is expected to close a 'loop' block");

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space is expected after 'until' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, NULL, &coundition),
              "a valid expression is expected after the 'until' keyword");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow the 'until' expression");

    return PARSER_SUCCESS;
}

parser_status_t flowcontrol_parser(FILE* input, const void* args,
                                   flowcontrol_t* output)
{
    if (TRY(input, if_parser(input, NULL, &output->if_instr))
        == PARSER_SUCCESS)
    {
        output->type = FLOWCONTROL_TYPE_IF;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, on_parser(input, NULL, &output->on_instr))
        == PARSER_SUCCESS)
    {
        output->type = FLOWCONTROL_TYPE_ON;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, while_parser(input, NULL, &output->while_instr))
        == PARSER_SUCCESS)
    {
        output->type = FLOWCONTROL_TYPE_WHILE;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, for_parser(input, NULL, &output->for_instr))
        == PARSER_SUCCESS)
    {
        output->type = FLOWCONTROL_TYPE_FOR;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, loop_parser(input, NULL, &output->loop_instr))
        == PARSER_SUCCESS)
    {
        output->type = FLOWCONTROL_TYPE_LOOP;
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t affectation_parser(FILE* input, const void* args,
                                   void* output)
{
    valref_t* left = NULL;
    expression_t *right = NULL;

    PARSE(valref_parser(input, NULL, &left));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(char_parser(input, "=", NULL));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, NULL, &right),
              "a valid expression must be provided after an affectation '='");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow affaction's expression");

    return PARSER_SUCCESS;
}

parser_status_t instruction_parser(FILE* input, const void* args,
                                   void* output)
{
    expression_t* expression = NULL;

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    if (TRY(input, flowcontrol_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, affectation_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, print_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, read_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, return_parser(input, NULL, NULL)) == PARSER_SUCCESS) {
        return PARSER_SUCCESS;
    } else
    if (TRY(input, expression_parser(input, NULL, &expression)) == PARSER_SUCCESS) {
        expression_print(stdout, expression);
        printf("\n");
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t instructions_parser(FILE* input, const void* args,
                                    void* output)
{
    while (TRY(input, instruction_parser(input, NULL, NULL)) == PARSER_SUCCESS)
    {
    }

    return PARSER_SUCCESS;
}
