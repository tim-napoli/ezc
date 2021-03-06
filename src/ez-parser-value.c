#include <string.h>
#include <stdlib.h>
#include "ez-parser.h"
#include "ez-lang-errors.h"

parser_status_t character_parser(FILE* input, const void* args,
                                 char* output)
{
    char chars[512];
    char* chars_ptr = chars;

    PARSE(char_parser(input, "'", NULL));
    /*  TODO handling escaping */
    PARSE_ERR(char_parser(input, NULL, &chars_ptr),
              "couldn't parse character");
    PARSE_ERR(char_parser(input, "'", NULL),
              "unclosed character");

    *output = chars[0];
    return PARSER_SUCCESS;
}

parser_status_t string_parser(FILE* input, const void* args,
                              char** output)
{
    char string[512];
    char *string_ptr = string;

    PARSE(char_parser(input, "\"", NULL));
    PARSE(until_char_parser(input, "\"", &string_ptr));

    /* Handling " escaping */
    while (*(string_ptr - 1) == '\\') {
        PARSE(char_parser(input, "\"", &string_ptr));
        PARSE(until_char_parser(input, "\"", &string_ptr));
    }
    PARSE_ERR(char_parser(input, "\"", NULL),
              "unclosed string");

    int string_len = strlen(string);
    *output = malloc((string_len + 1) * sizeof(char));
    strcpy(*output, string);

    return PARSER_SUCCESS;
}

parser_status_t natural_parser(FILE* input, const void* args,
                               unsigned int* output)
{
    char buf[512];
    char *buf_ptr = buf;

    PARSE(chars_parser(input, "0123456789", &buf_ptr));

    if (sscanf(buf, "%u", output) != 1) {
        PARSER_LANG_ERR("invalid natural number '%s'", buf);
    }

    return PARSER_SUCCESS;
}

parser_status_t integer_parser(FILE* input, const void* args,
                               int* output)
{
    int signe = 1;
    unsigned int natural = 0;

    if (TRY(input, char_parser(input, "-", NULL)) == PARSER_SUCCESS) {
        signe = -1;
    }
    PARSE(natural_parser(input, args, &natural));

    *output = signe * (int) natural;

    return PARSER_SUCCESS;
}

parser_status_t real_parser(FILE* input, const void* args,
                            double* output)
{
    double signe = 1.0;
    unsigned int integer = 0;
    unsigned int decimal = 0;
    double true_decimal = 0.0;

    if (TRY(input, char_parser(input, "-", NULL)) == PARSER_SUCCESS) {
        signe = -1.0;
    }

    PARSE(natural_parser(input, NULL, &integer));
    PARSE(char_parser(input, ".", NULL));
    PARSE(natural_parser(input, NULL, &decimal));

    true_decimal = decimal;
    while (true_decimal > 1.0) {
        true_decimal *= 0.1;
    }

    *output = signe * (integer + true_decimal);

    return PARSER_SUCCESS;
}

parser_status_t bool_parser(FILE* input, const void* args,
                            bool* output)
{
    if (TRY(input, word_parser(input, "true", NULL)) == PARSER_SUCCESS) {
        *output = true;
        return PARSER_SUCCESS;
    }
    PARSE(word_parser(input, "false", NULL));
    *output = false;
    return PARSER_SUCCESS;
}

parser_status_t parameters_parser(FILE* input, context_t* ctx,
                                  parameters_t** parameters)
{
    expression_t* expr = NULL;

    if (TRY(input, expression_parser(input, ctx, &expr)) == PARSER_SUCCESS) {
        parameters_add(*parameters, expr);

        SKIP_MANY(input, space_parser(input, NULL, NULL));

        if (TRY(input, char_parser(input, ",", NULL)) == PARSER_SUCCESS) {
            SKIP_MANY(input, space_parser(input, NULL, NULL));

            PARSE_ERR(parameters_parser(input, ctx, parameters),
                      "invalid parameter");
        }
    }

    return PARSER_SUCCESS;
}

parser_status_t valref_parser(FILE* input, context_t* ctx,
                              valref_t** valref)
{
    identifier_t id;

    PARSE(identifier_parser(input, NULL, &id));

    *valref = valref_new(&id);

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    if (TRY(input, char_parser(input, "(", NULL)) == PARSER_SUCCESS) {
        SKIP_MANY(input, space_parser(input, NULL, NULL));

        parameters_t* parameters = valref_get_parameters(*valref);
        PARSE(parameters_parser(input, ctx, &parameters));

        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(char_parser(input, ")", NULL),
                  "unclosed function call");

        valref_set_is_funccall(*valref, true);

        /* TODO check if 'identifier' is a function and not a procedure */
    }

    while (TRY(input, char_parser(input, "[", NULL)) == PARSER_SUCCESS) {
        identifier_t id_at = {.value = "at"};
        valref_t* valref_at = valref_new(&id_at);
        expression_t* expr = NULL;

        SKIP_MANY(input, space_parser(input, NULL, NULL));

        parameters_t* parameters = valref_get_parameters(valref_at);
        PARSE_ERR(expression_parser(input, ctx, &expr),
                  "a valid expression must follow '['");
        parameters_add(parameters, expr);

        valref_set_is_funccall(valref_at, true);

        SKIP_MANY(input, space_parser(input, NULL, NULL));

        PARSE_ERR(char_parser(input, "]", NULL),
                  "missing ']'");

        (*valref)->next = valref_at;
        valref = &(*valref)->next;

        SKIP_MANY(input, space_parser(input, NULL, NULL));
    }

    if (TRY(input, char_parser(input, ".", NULL)) == PARSER_SUCCESS) {
        valref_t* next = NULL;

        PARSE_ERR(valref_parser(input, ctx, &next),
                  "expected identifier after '.'");

        valref_set_next(*valref, next);
    }

    SKIP_MANY(input, space_parser(input, NULL, NULL));

    return PARSER_SUCCESS;
}

parser_status_t empty_value_parser(FILE* input, context_t* ctx,
                                   type_t** empty_type)
{
    type_t* optional_type = NULL;

    PARSE(word_parser(input, "empty ", NULL));
    SKIP_MANY(input, space_parser(input, NULL, NULL));

    PARSE_ERR(type_parser(input, ctx, &optional_type),
              "a valid type must be given after 'empty' keyword");
    *empty_type = type_optional_new(optional_type);

    return PARSER_SUCCESS;
}

parser_status_t value_parser(FILE* input, context_t* ctx,
                             value_t* value)
{
    // XXX (->)
    if (TRY(input, string_parser(input, NULL, &value->string))
        == PARSER_SUCCESS)
    {
        value->type = VALUE_TYPE_STRING;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, character_parser(input, NULL, &value->character))
        == PARSER_SUCCESS)
    {
        value->type = VALUE_TYPE_CHAR;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, real_parser(input, NULL, &value->real))
        == PARSER_SUCCESS)
    {
        value->type = VALUE_TYPE_REAL;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, natural_parser(input, NULL, &value->natural))
        == PARSER_SUCCESS)
    {
        value->type = VALUE_TYPE_NATURAL;
        return PARSER_SUCCESS;
    }  else
    if (TRY(input, integer_parser(input, NULL, &value->integer))
        == PARSER_SUCCESS)
    {
        value->type = VALUE_TYPE_INTEGER;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, bool_parser(input, NULL, &value->boolean))
        == PARSER_SUCCESS)
    {
        value->type = VALUE_TYPE_BOOLEAN;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, valref_parser(input, ctx, &value->valref))
        == PARSER_SUCCESS)
    {
        value->type = VALUE_TYPE_VALREF;

        return PARSER_SUCCESS;
    } else
    if (TRY(input, empty_value_parser(input, ctx, &value->empty_type))
        == PARSER_SUCCESS)
    {
        value->type = VALUE_TYPE_EMPTY;
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

