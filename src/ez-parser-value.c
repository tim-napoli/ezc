#include <string.h>
#include <stdlib.h>
#include "ez-parser.h"

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
    return PARSER_FAILURE;
#if 0
    char buf[512];
    char *buf_ptr = buf;

    /* XXX bad parsing, berk ! */
    PARSE(integer_parser(input, args, &buf_ptr));
    PARSE(char_parser(input, ".", &buf_ptr));
    PARSE(chars_parser(input, "0123456789", &buf_ptr));

    if (sscanf(buf, "%f", output) != 1) {
        PARSER_LANG_ERR("invalid real '%s'", buf);
    }

    return PARSER_SUCCESS;
#endif
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

parser_status_t parameters_parser(FILE* input, const void* args,
                                  parameters_t* parameters)
{
    expression_t** expr = &parameters->parameters[parameters->nparameters];

    if (TRY(input, expression_parser(input, NULL, expr)) == PARSER_SUCCESS) {
        parameters->nparameters++;

        SKIP_MANY(input, space_parser(input, NULL, NULL));
        if (TRY(input, char_parser(input, ",", NULL)) == PARSER_SUCCESS) {
            SKIP_MANY(input, space_parser(input, NULL, NULL));
            PARSE_ERR(parameters_parser(input, NULL, parameters),
                      "invalid parameter");
        }
    }

    return PARSER_SUCCESS;
}

parser_status_t valref_parser(FILE* input, const void* args,
                              valref_t** output)
{
    identifier_t id;

    PARSE(identifier_parser(input, NULL, &id));
    *output = valref_new(&id);

    /* XXX find a better way to do this function... */
    if (TRY(input, char_parser(input, ".", NULL)) == PARSER_SUCCESS) {
        /* Continue with a valref */
        PARSE_ERR(valref_parser(input, NULL, &(*output)->next),
                  "expected identifier after '.'");
    } else {
        /* Check if this is a function call */
        SKIP_MANY(input, space_parser(input, NULL, NULL));
        if (TRY(input, char_parser(input, "(", NULL)) == PARSER_SUCCESS) {
            SKIP_MANY(input, space_parser(input, NULL, NULL));
            PARSE(parameters_parser(input, NULL, &(*output)->parameters));
            SKIP_MANY(input, space_parser(input, NULL, NULL));
            PARSE_ERR(char_parser(input, ")", NULL),
                      "unclosed function call");
            (*output)->is_funccall = true;
            /* TODO check if 'identifier' is a function and not a procedure */
            if (TRY(input, char_parser(input, ".", NULL)) == PARSER_SUCCESS) {
                /* Continue with a valref */
                PARSE_ERR(valref_parser(input, NULL, &(*output)->next),
                          "expected identifier after '.'");
            }
        }
        (*output)->nindexings = 0;
        while (TRY(input, char_parser(input, "[", NULL)) == PARSER_SUCCESS) {
            /* Check is this is a array/vector/map indexing */
            /* TODO check if 'identifier' is a array/vector value */
            SKIP_MANY(input, space_parser(input, NULL, NULL));
            PARSE_ERR(expression_parser(input, NULL,
                      &(*output)->indexings[(*output)->nindexings]),
                      "a valid expression must follow '['");
            (*output)->has_indexing = true;
            (*output)->nindexings++;
            SKIP_MANY(input, space_parser(input, NULL, NULL));
            PARSE_ERR(char_parser(input, "]", NULL),
                      "missing ']'");
            SKIP_MANY(input, space_parser(input, NULL, NULL));
            if (TRY(input, char_parser(input, ".", NULL)) == PARSER_SUCCESS) {
                /* Continue with a valref */
                PARSE_ERR(valref_parser(input, NULL, &(*output)->next),
                          "expected identifier after '.'");
            }
            SKIP_MANY(input, space_parser(input, NULL, NULL));
        }
    }

    return PARSER_SUCCESS;
}

parser_status_t value_parser(FILE* input, const void* args,
                             value_t* value)
{
    if (TRY(input, string_parser(input, NULL, &value->string))
        == PARSER_SUCCESS)
    {
        value->type = VALUE_TYPE_STRING;
        return PARSER_SUCCESS;
    } else
    if (TRY(input, real_parser(input, NULL, &value->real))
        == PARSER_SUCCESS)
    {
        value->type = VALUE_TYPE_REAL;
        return PARSER_SUCCESS;
    } else
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
    if (TRY(input, valref_parser(input, NULL, &value->valref))
        == PARSER_SUCCESS)
    {
        value->type = VALUE_TYPE_VALREF;
        return PARSER_SUCCESS;
    }

    return PARSER_FAILURE;
}

