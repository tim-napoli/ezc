#include <string.h>
#include "ez-parser.h"
#include "ez-lang.h"
#include "ez-lang-errors.h"

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

parser_status_t end_of_file_parser(FILE* input, const void* args,
                                   void* output) {
    char c = fgetc(input);

    if (c != EOF) return PARSER_FAILURE;

    return PARSER_SUCCESS;
}

parser_status_t identifier_parser(FILE* input, const context_t* ctx,
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

    if (!identifier_set_value(id, value)) {
        PARSER_LANG_ERR("identifier %s is too long (%d max chars)",
                            value, IDENTIFIER_SIZE);
    }

    if (identifier_is_reserved(id)) {
        return PARSER_FAILURE;
    }

    return PARSER_SUCCESS;
}

parser_status_t function_signature_parser(FILE* input, const context_t* ctx,
                                          function_signature_t** signature)
{
    type_t* type = NULL;

    PARSE(char_parser(input, "(", NULL));

    *signature = function_signature_new();
    while (TRY(input, type_parser(input, ctx, &type)) == PARSER_SUCCESS) {
        vector_push(&(*signature)->args_types, type);
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        if (TRY(input, char_parser(input, ",", NULL)) == PARSER_SUCCESS) {
            SKIP_MANY(input, space_parser(input, NULL, NULL));
        } else {
            break;
        }
    }
    PARSE_ERR(char_parser(input, ")", NULL),
              "a function signature must be closed with ')'");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    if (TRY(input, word_parser(input, "return", NULL)) == PARSER_SUCCESS) {
        PARSE_ERR(char_parser(input, " ", NULL),
                  "a space is required after function signature 'return'");
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(type_parser(input, ctx, &(*signature)->return_type),
                  "a valid return type is required for a function signature");
    }

    return PARSER_SUCCESS;
}

parser_status_t type_parser(FILE* input, const context_t* ctx,
                            type_t* *type)
{
    identifier_t structure_id;

    if (TRY(input, word_parser(input, "integer", NULL)) == PARSER_SUCCESS) {
        *type = type_integer_new();
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "natural", NULL)) == PARSER_SUCCESS) {
        *type = type_natural_new();
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "boolean", NULL)) == PARSER_SUCCESS) {
        *type = type_boolean_new();
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "real", NULL)) == PARSER_SUCCESS) {
        *type = type_real_new();
        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "string", NULL)) == PARSER_SUCCESS) {
        *type = type_string_new();
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

        type_t* of;
        PARSE_ERR(type_parser(input, ctx, &of),
                  "invalid type for 'vector'");

        *type = type_vector_new(of);

        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "optional", NULL)) == PARSER_SUCCESS) {
        PARSE_ERR(space_parser(input, NULL, NULL),
                  "expected spaces after 'optional'");
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        type_t* of;
        PARSE_ERR(type_parser(input, ctx, &of),
                  "invalid type for 'optional'");

        *type = type_optional_new(of);

        return PARSER_SUCCESS;
    } else
    if (TRY(input, word_parser(input, "function", NULL)) == PARSER_SUCCESS) {
        function_signature_t* signature = NULL;
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        PARSE_ERR(function_signature_parser(input, ctx, &signature),
                  "invalid function signature");
        *type = type_function_new(signature);
        return PARSER_SUCCESS;
    } else
    if (TRY(input, identifier_parser(input, NULL, &structure_id))
        == PARSER_SUCCESS) {
        structure_t* structure = context_find_structure(ctx, &structure_id);

        if (structure == NULL) {
            return PARSER_FAILURE;
        }

        /* XXX so type carry a const structure, it doesn't own it. */
        *type = type_structure_new(structure);

        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

parser_status_t variable_tail_parser(FILE* input, const context_t* ctx,
                                     symbol_t** symbol)
{
    identifier_t id;
    type_t* is = NULL;

    PARSE(identifier_parser(input, NULL, &id));

    if (identifier_is_reserved(&id)) {
        error_identifier_is_keyword(input, &id);

        return PARSER_FAILURE;
    }

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a variable identifier");

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(word_parser(input, "is", NULL),
              "a variable declaration must have a 'is' keyword");

    PARSE_ERR(space_parser(input, NULL, NULL),
              "a space must follow a variable 'is' keyword");

    SKIP_MANY(input, space_parser(input, NULL, &is));

    PARSE_ERR(type_parser(input, ctx, &is),
              "a variable must have a valid type");

    *symbol = symbol_new(&id, is);

    return PARSER_SUCCESS;
}

parser_status_t range_parser(FILE* input, const context_t* ctx,
                             range_t* range)
{
    expression_t* from = NULL;
    expression_t* to = NULL;

    PARSE(expression_parser(input, ctx, &from));

    range_set_from(range, from);

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE(word_parser(input, "..", NULL));

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(expression_parser(input, ctx, &to),
              "a valid expression is expected after range '..'");

    range_set_to(range, to);

    return PARSER_SUCCESS;
}

