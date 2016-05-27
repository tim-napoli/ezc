#include "ez-lang-errors.h"
#include "parser.h"
#include <stdio.h>

void error_print(FILE *input) {
  int line, column;
  char c;
  get_file_coordinates(input, &line, &column, &c);

  fprintf(stderr, "error (line %d): ", line);
}

void error_identifier_is_keyword(FILE* input, const identifier_t* id) {
  error_print(input);
  fprintf(stderr, "symbol %s is a keyword\n", id->value);
}

void error_identifier_exists(FILE* input, const identifier_t* id) {
    error_print(input);
    fprintf(stderr, "identifier %s already exists in this context\n",
            id->value);
}

void error_identifier_not_found(FILE* input, const identifier_t* id) {
    error_print(input);
    fprintf(stderr, "identifier %s not found in this context\n", id->value);
}

void error_valref_not_found(FILE* input, const context_t* ctx,
                            const valref_t* valref) {
    error_print(input);
    fprintf(stderr, "valref ");
    valref_print(stderr, ctx, valref);
    fprintf(stderr, " not found in this context\n");
}

void error_no_main_function(const identifier_t* id) {
    fprintf(stderr, "missing main function "
                    "'function %s(in args is vector of string) : integer'\n",
            id->value);
}

void error_invalid_main_function(const identifier_t* id) {
    fprintf(stderr, "main function signature must be "
                    "'function %s(in args is vector of string) : integer'\n",
            id->value);
}

void error_expression_not_valid(FILE* input, const context_t* ctx,
                                const expression_t* expr)
{
    error_print(input);
    fprintf(stderr, "expression ");
    expression_print(stderr, ctx, expr);
    fprintf(stderr, " is not valid in this context\n");
}

void error_parameters_not_valid(FILE* input, const context_t* ctx,
                                const parameters_t* parameters)
{
    error_print(input);
    fprintf(stderr, "parameters \n");
    parameters_print(stderr, ctx, parameters);
    fprintf(stderr, " are not valid in this context\n");
};

void error_value_not_valid(FILE* input, const context_t* ctx,
                           const value_t* value)
{
    error_print(input);
    fprintf(stderr, "value ");
    value_print(stderr, ctx, value);
    fprintf(stderr, " is not valid in this context\n");
}

void error_decleration_not_valid(FILE* input) {
    error_print(input);
    fprintf(stderr, "declaration is not valid\n");

}

void error_affectation_not_valid(FILE* input) {
    error_print(input);
    fprintf(stderr, "affectation is not valid (bad casting)\n");
}

void error_bad_access_left_value(FILE* input, const valref_t* v) {
    error_print(input);
    fprintf(stderr, "bad access type of left value (%s) on affectation\n", v->identifier.value);
}

void error_bad_access_expr_value(FILE* input, const value_t* v) {
    error_print(input);
    fprintf(stderr, "bad access type of value (%s) on expression\n", v->valref->identifier.value);
}
