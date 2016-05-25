#include "ez-parser.h"
#include "ez-lang.h"
#include "ez-lang-errors.h"

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
                                function_arg_modifier_t* function_arg_modifier)
{
    if (TRY(input, word_parser(input, "in", NULL)) == PARSER_SUCCESS) {
        *function_arg_modifier = FUNCTION_ARG_MODIFIER_IN;

        return PARSER_SUCCESS;
    }

    if (TRY(input, word_parser(input, "out", NULL)) == PARSER_SUCCESS) {
        *function_arg_modifier = FUNCTION_ARG_MODIFIER_OUT;

        return PARSER_SUCCESS;
    }

    if (TRY(input, word_parser(input, "inout", NULL)) == PARSER_SUCCESS) {
        *function_arg_modifier = FUNCTION_ARG_MODIFIER_INOUT;

        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t function_args_parser(FILE* input, const context_t* ctx,
                                     vector_t* arguments)
{
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    symbol_t* symbol;
    type_t* is = NULL;
    identifier_t arg_id;
    function_arg_t* arg = NULL;
    function_arg_modifier_t modifier;

    if (TRY(input, modifier_parser(input, NULL, &modifier)) == PARSER_SUCCESS)
    {
        PARSE_ERR(space_parser(input, NULL, NULL),
                  "expected spaces");

        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(identifier_parser(input, NULL, &arg_id),
                  "a valid identifier must follow a modifier in function "
                  "arguments");

        PARSE(space_parser(input, NULL, NULL));
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(word_parser(input, "is", NULL),
                  "expected 'is'");

        PARSE(space_parser(input, NULL, NULL));
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(type_parser(input, ctx, &is),
                  "expected valid type");

        symbol = symbol_new(&arg_id, is);
        arg = function_arg_new(modifier, symbol);
        vector_push(arguments, arg);

        SKIP_MANY(input, space_parser(input, NULL, NULL));

        if (TRY(input, char_parser(input, ",", NULL)) == PARSER_SUCCESS) {
            PARSE(function_args_parser(input, ctx, arguments));
        }
    }

    return PARSER_SUCCESS;
}

parser_status_t local_parser(FILE* input, const context_t* ctx,
                             symbol_t** symbol)
{
    PARSE(word_parser(input, "local", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a 'local' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(variable_tail_parser(input, ctx, symbol));

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after a local declaration");

    return PARSER_SUCCESS;
}

parser_status_t function_parser(FILE* input, context_t* ctx,
                                function_t** function)
{
    identifier_t function_id;

    PARSE(word_parser(input, "function", NULL));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(identifier_parser(input, NULL, &function_id),
              "a function must have a valid identifier");

    *function = function_new(&function_id);

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(char_parser(input, "(", NULL), "missing '('");

    vector_t* args = vector_new(0);

    PARSE(function_args_parser(input, ctx, args));

    function_set_args(*function, args);

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(char_parser(input, ")", NULL), "missing ')'");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "return", NULL), "missing 'return'");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    type_t* return_type = NULL;

    PARSE_ERR(type_parser(input, ctx, &return_type),
              "unknown return type");

    function_set_return_type(*function, return_type);

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after a function head");

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    while (TRY(input, word_parser(input, "begin", NULL)) == PARSER_FAILURE) {
        symbol_t* local = NULL;

        PARSE(local_parser(input, ctx, &local));

        function_add_local(*function, local);

        SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));
    }

    /* Push the current function inside the context. */
    context_set_function(ctx, *function);

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a newline is expected after the 'begin' keyword");

    vector_t* instructions = vector_new(0);

    PARSE(instructions_parser(input, ctx, instructions));

    function_set_instructions(*function, instructions);

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "end", NULL),
              "A function must be ended with 'end' keyword");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a newline is expected after the 'end' keyword");

    /* Remove the function from the context */
    context_set_function(ctx, NULL);

    return PARSER_SUCCESS;
}

parser_status_t procedure_parser(FILE* input, context_t* ctx,
                                 function_t** function)
{
    identifier_t procedure_id;

    PARSE(word_parser(input, "procedure", NULL));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(identifier_parser(input, NULL, &procedure_id),
              "a procedure must have a valid identifier");

    *function = function_new(&procedure_id);

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(char_parser(input, "(", NULL), "missing '('");

    vector_t* args = vector_new(0);

    PARSE(function_args_parser(input, ctx, args));

    function_set_args(*function, args);

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(char_parser(input, ")", NULL), "missing ')'");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after a procedure head");

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    while (TRY(input, word_parser(input, "begin", NULL)) == PARSER_FAILURE) {
        symbol_t* local = NULL;

        PARSE(local_parser(input, ctx, &local));

        function_add_local(*function, local);

        SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));
    }

    /* Push the current function inside the context. */
    context_set_function(ctx, *function);

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a newline is expected after the 'begin' keyword");

    vector_t* instructions = vector_new(0);

    PARSE(instructions_parser(input, ctx, instructions));

    function_set_instructions(*function, instructions);

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "end", NULL),
              "A procedure must be ended with 'end' keyword");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a newline is expected after the 'end' keyword");

    context_set_function(ctx, NULL);

    return PARSER_SUCCESS;
}

parser_status_t constant_parser(FILE* input,
                                const context_t* ctx,
                                constant_t** constant)
{
    expression_t* expression = NULL;
    symbol_t* symbol = NULL;

    PARSE(word_parser(input, "constant", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a 'constant' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(variable_tail_parser(input, ctx, &symbol));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(char_parser(input, "=", NULL),
              "a '=' is expected to intialize constant");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, ctx, &expression),
              "a valid expression is expected to initialize a constant");

    *constant = constant_new(symbol, expression);

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after a constant declaration");

    return PARSER_SUCCESS;
}

parser_status_t global_parser(FILE* input,
                              const context_t* ctx,
                              symbol_t** symbol)
{
    PARSE(word_parser(input, "global", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a 'global' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(variable_tail_parser(input, ctx, symbol));

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after a global declaration");

    return PARSER_SUCCESS;
}

parser_status_t structure_member_parser(FILE* input,
                                        const void* unused_args,
                                        symbol_t** symbol)
{
    PARSE(variable_tail_parser(input, NULL, symbol));

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the member type");

    return PARSER_SUCCESS;
}

parser_status_t structure_parser(FILE* input,
                                 const context_t* ctx,
                                 structure_t* *structure)
{
    identifier_t id;

    PARSE(word_parser(input, "structure", NULL));

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow the 'structure' keyword");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(identifier_parser(input, NULL, &id));

    *structure = structure_new(&id);

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow the structure identifier");
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "is", NULL),
              "a 'is' keyword must follow the structure identifier");

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the structure 'is' keyword");

    SKIP_MANY(input, empty_parser(input, NULL, NULL));

    while (TRY(input, word_parser(input, "end", NULL)) == PARSER_FAILURE) {
        symbol_t* symbol;

        PARSE(structure_member_parser(input, NULL, &symbol));

        SKIP_MANY(input, empty_parser(input, NULL, NULL));

        structure_add_member(*structure, symbol);
    }

    PARSE_ERR(end_of_line_parser(input, NULL, NULL),
              "a new line is expected after the structure 'end' keyword");

    return PARSER_SUCCESS;
}

parser_status_t entity_parser(FILE* input, context_t* ctx,
                              program_t* program)
{
    function_t* func = NULL;
    structure_t* structure = NULL;
    constant_t* constant = NULL;
    symbol_t* global = NULL;

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    if (TRY(input, constant_parser(input, ctx, &constant)) == PARSER_SUCCESS)
    {
        if (context_has_identifier(ctx, &constant->symbol->identifier)) {
            error_identifier_exists(input, &constant->symbol->identifier);
            constant_delete(constant);
        } else {
            program_add_constant(program, constant);
        }

        return PARSER_SUCCESS;
    } else
    if (TRY(input, global_parser(input, ctx, &global)) == PARSER_SUCCESS) {
        if (context_has_identifier(ctx, &global->identifier)) {
            error_identifier_exists(input, &global->identifier);
            symbol_delete(global);
        } else {
            program_add_global(program, global);
        }

        return PARSER_SUCCESS;
    } else
    if (TRY(input, structure_parser(input, ctx, &structure))
        == PARSER_SUCCESS)
    {
        if (context_has_identifier(ctx, &structure->identifier)) {
            error_identifier_exists(input, &structure->identifier);
            structure_delete(structure);
        } else {
            program_add_structure(program, structure);
        }

        return PARSER_SUCCESS;
    } else
    if (TRY(input, function_parser(input, ctx, &func)) == PARSER_SUCCESS) {
        if (context_has_identifier(ctx, &func->identifier)) {
            error_identifier_exists(input, &func->identifier);
            function_delete(func);
        } else {
            program_add_function(program, func);
        }

        return PARSER_SUCCESS;
    } else
    if (TRY(input, procedure_parser(input, ctx, &func)) == PARSER_SUCCESS) {
        if (context_has_identifier(ctx, &func->identifier)) {
            error_identifier_exists(input, &func->identifier);
            function_delete(func);
        } else {
            program_add_procedure(program, func);
        }

        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t program_parser(FILE* input,
                               context_t* ctx,
                               program_t** program)
{
    identifier_t program_id;

    context_init(ctx);

    PARSE(header_parser(input, NULL, &program_id));

    *program = program_new(&program_id);

    context_set_program(ctx, *program);
    identifier_t prg_id = context_get_program_identifier(ctx);

    SKIP_MANY(input, comment_or_empty_parser(input, NULL, NULL));

    while (entity_parser(input, ctx, *program) == PARSER_SUCCESS) {}

    if (!program_has_function(*program, &prg_id)) {
        error_no_main_function(&prg_id);

        return PARSER_FATAL;
    }

    if (!program_main_function_is_valid(*program)) {
        error_invalid_main_function(&prg_id);

        return PARSER_FATAL;
    }

    return PARSER_SUCCESS;
}
