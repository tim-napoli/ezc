#include <string.h>
#include "ez-parser.h"
#include <ez-lang-errors.h>

parser_status_t print_parser(FILE* input, const context_t* ctx,
                             parameters_t* output)
{
    PARSE(word_parser(input, "print", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expected after 'print' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    parameters_init(output);

    PARSE(parameters_parser(input, ctx, &output));

    PARSE(end_of_line_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t read_parser(FILE* input, const context_t* ctx,
                            valref_t** valref)
{
    PARSE(word_parser(input, "read", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expected after 'read' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(valref_parser(input, ctx, valref),
              "a single value reference must follow the 'read' keyword");

    if (!context_valref_is_valid(ctx, *valref)) {
        error_valref_not_found(input, ctx, *valref);
    }

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow the 'read' line");

    return PARSER_SUCCESS;
}

parser_status_t return_parser(FILE* input, const context_t* ctx,
                              expression_t** expression)
{
    PARSE(word_parser(input, "return", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'return' keyword");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, ctx, expression),
              "bad return expression");

    PARSE(end_of_line_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t elsif_parser(FILE* input, const context_t* ctx,
                             elsif_instr_t** elsif_intr)
{
    expression_t* coundition = NULL;

    PARSE(word_parser(input, "elsif", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'elsif' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(expression_parser(input, ctx, &coundition));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(word_parser(input, "then", NULL));

    PARSE(end_of_line_parser(input, NULL, NULL));

    *elsif_intr = elsif_instr_new(coundition);

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    // XXX
    PARSE(instructions_parser(input, ctx, &(*elsif_intr)->instructions));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t else_parser(FILE* input, const context_t* ctx,
                            vector_t* vector)
{
    PARSE(word_parser(input, "else", NULL));
    PARSE(end_of_line_parser(input, NULL, NULL));

    SKIP_MANY(input, space_parser(input, NULL, NULL));
    PARSE(instructions_parser(input, ctx, vector));
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t if_parser(FILE* input, const context_t* ctx,
                          if_instr_t** if_instr)
{
    expression_t* coundition = NULL;

    PARSE(word_parser(input, "if", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'if' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, ctx, &coundition),
              "'if' invalid expression");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "then", NULL),
              "missing 'then' keyword after 'if' expression");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "an end of line must follow the 'then' keyword");

    *if_instr = if_instr_new(coundition);

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(instructions_parser(input, ctx, &(*if_instr)->instructions)); // XXX

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    elsif_instr_t* elsif = NULL;
    while (TRY(input, elsif_parser(input, ctx, &elsif)) == PARSER_SUCCESS) {
        vector_push(&(*if_instr)->elsifs, elsif); // XXX
    }

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    TRY(input, else_parser(input, ctx, &(*if_instr)->else_instrs)); // XXX

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "endif", NULL),
              "a 'if' must be closed with the 'endif' keyword");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow the 'endif' keyword");

    return PARSER_SUCCESS;
}

parser_status_t on_parser(FILE* input, const context_t* ctx,
                          on_instr_t** on_instr)
{
    expression_t* coundition = NULL;

    PARSE(word_parser(input, "on", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'on' keyword");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, ctx, &coundition),
              "an expression must follow a 'on' keyword");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "do", NULL),
              "a 'do' keyword must follow the 'on' boolean expression");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    *on_instr = on_instr_new(coundition);

    PARSE(instruction_parser(input, ctx, &(*on_instr)->instruction)); // XXX

    return PARSER_SUCCESS;
}

parser_status_t while_parser(FILE* input, const context_t* ctx,
                             while_instr_t** while_instr)
{
    expression_t* expr = NULL;

    PARSE(word_parser(input, "while", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'while' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, ctx, &expr),
              "a valid expression is expected after the 'while' keyword");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "do", NULL),
              "a 'do' keyword must follow the 'while' expression");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the while 'do' keyword");

    *while_instr = while_instr_new(expr);

    // XXX
    PARSE(instructions_parser(input, ctx, &(*while_instr)->instructions));

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "endwhile", NULL),
              "a 'endwhile' keyword is expected to close a 'while' block");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow the 'endwhile' keyword");
    return PARSER_SUCCESS;
}

parser_status_t for_parser(FILE* input, const context_t* ctx,
                           for_instr_t** for_instr)
{
    identifier_t id;

    PARSE(word_parser(input, "for", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after 'for' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(identifier_parser(input, NULL, &id),
              "a valid identifier is expected after the 'for' keyword");

    *for_instr = for_instr_new(&id);

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after for identifier");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "in", NULL),
              "a 'in' keyword must follow the 'for' identifier");

    PARSE_ERR(space_parser(input, NULL, NULL),
          "a space is expcted after for 'in' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(range_parser(input, ctx, &(*for_instr)->range), // XXX
              "a valid range is expected after 'for' 'in' keyword");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "do", NULL),
              "a 'do' keyword must follow the 'for' range");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the for 'do' keyword");

    PARSE(instructions_parser(input, ctx, &(*for_instr)->instructions)); // XXX

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "endfor", NULL),
              "a 'endfor' keyword is expected to close a 'for' block");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow the 'endfor' keyword");

    return PARSER_SUCCESS;
}

parser_status_t loop_parser(FILE* input, const context_t* ctx,
                            loop_instr_t** loop_instr)
{
    PARSE(word_parser(input, "loop", NULL));

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the 'loop' keyword");

    *loop_instr = loop_instr_new(NULL);

    // XXX
    PARSE(instructions_parser(input, ctx, &(*loop_instr)->instructions));

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "until", NULL),
              "a 'until' keyword is expected to close a 'loop' block");

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space is expected after 'until' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, ctx, &(*loop_instr)->coundition), // XXX
              "a valid expression is expected after the 'until' keyword");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow the 'until' expression");

    return PARSER_SUCCESS;
}

parser_status_t flowcontrol_parser(FILE* input, const context_t* ctx,
                                   flowcontrol_t* flowcontrol)
{
    // XXX XXX XXX
    if (TRY(input, if_parser(input, ctx, &flowcontrol->if_instr))
        == PARSER_SUCCESS)
    {
        flowcontrol->type = FLOWCONTROL_TYPE_IF;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, on_parser(input, ctx, &flowcontrol->on_instr))
        == PARSER_SUCCESS)
    {
        flowcontrol->type = FLOWCONTROL_TYPE_ON;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, while_parser(input, ctx, &flowcontrol->while_instr))
        == PARSER_SUCCESS)
    {
        flowcontrol->type = FLOWCONTROL_TYPE_WHILE;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, for_parser(input, ctx, &flowcontrol->for_instr))
        == PARSER_SUCCESS)
    {
        flowcontrol->type = FLOWCONTROL_TYPE_FOR;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, loop_parser(input, ctx, &flowcontrol->loop_instr))
        == PARSER_SUCCESS)
    {
        flowcontrol->type = FLOWCONTROL_TYPE_LOOP;
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t affectation_parser(FILE* input, const context_t* ctx,
                                   affectation_instr_t* affectation_instr)
{

    PARSE(valref_parser(input, ctx, &affectation_instr->lvalue)); // XXX

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(char_parser(input, "=", NULL));

    if (!context_valref_is_valid(ctx, affectation_instr->lvalue)) { // XXX
        error_valref_not_found(input, ctx, affectation_instr->lvalue); // XXX
    }

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    // XXX
    PARSE_ERR(expression_parser(input, ctx, &affectation_instr->expression),
              "a valid expression must be provided after an affectation '='");

    if (!context_affectation_is_valid(ctx, affectation_instr)) {
        error_affectation_not_valid(input);
    }

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line must follow affaction's expression");

    return PARSER_SUCCESS;
}

parser_status_t instruction_parser(FILE* input, const context_t* ctx,
                                   instruction_t** instruction)
{
    valref_t* valref = NULL;
    flowcontrol_t flowcontrol;
    parameters_t parameters;
    expression_t* expression = NULL;
    affectation_instr_t affectation;

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    if (TRY(input, flowcontrol_parser(input, ctx, &flowcontrol))
        == PARSER_SUCCESS)
    {
        *instruction = instruction_new(INSTRUCTION_TYPE_FLOWCONTROL);
        memcpy(&(*instruction)->flowcontrol, &flowcontrol,
               sizeof(flowcontrol_t)); // XXX XXX

        return PARSER_SUCCESS;
    } else
    if (TRY(input, affectation_parser(input, ctx, &affectation))
        == PARSER_SUCCESS) {
        *instruction = instruction_new(INSTRUCTION_TYPE_AFFECTATION);
        memcpy(&(*instruction)->affectation, &affectation, // XXX XXX
               sizeof(affectation_instr_t));

        return PARSER_SUCCESS;
    } else
    if (TRY(input, print_parser(input, ctx, &parameters)) == PARSER_SUCCESS) {
        *instruction = instruction_new(INSTRUCTION_TYPE_PRINT);
        // XXX XXX
        memcpy(&(*instruction)->parameters, &parameters, sizeof(parameters_t));

        // XXX should we do this check here ?
        //if (!context_parameters_are_valid(ctx, &parameters)) {
        //    error_parameters_not_valid(input, ctx, &parameters);
        //}

        return PARSER_SUCCESS;
    } else
    if (TRY(input, read_parser(input, ctx, &valref)) == PARSER_SUCCESS) {
        *instruction = instruction_new(INSTRUCTION_TYPE_READ);
        (*instruction)->valref = valref; // XXX

        return PARSER_SUCCESS;
    } else
    if (TRY(input, return_parser(input, ctx, &expression)) == PARSER_SUCCESS) {
        *instruction = instruction_new(INSTRUCTION_TYPE_RETURN);
        (*instruction)->expression = expression; // XXX

        return PARSER_SUCCESS;
    } else
    if (TRY(input, expression_parser(input, ctx, &expression))
        == PARSER_SUCCESS) {

        *instruction = instruction_new(INSTRUCTION_TYPE_EXPRESSION);
        (*instruction)->expression = expression; // XXX

        PARSE_ERR(end_of_line_parser(input, NULL, NULL),
                  "a new line must follow each instructions");

        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t instructions_parser(FILE* input, const context_t* ctx,
                                    vector_t* instructions)
{
    instruction_t* instr = NULL;

    while (TRY(input, instruction_parser(input, ctx, &instr))
          == PARSER_SUCCESS) {
        vector_push(instructions, instr);
    }

    return PARSER_SUCCESS;
}
