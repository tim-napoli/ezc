#include "ez-lang-errors.h"
#include "parser.h"
#include <stdio.h>

void error_print(FILE *input) {
  int line, column;
  char c;
  get_file_coordinates(input, &line, &column, &c);

  fprintf(stderr, "error (line %d column %d): ", line, column);
}

void error_identifier_is_keyword(FILE* input, const identifier_t* id) {
  error_print(input);
  fprintf(stderr, "symbol %s is a keyword\n", id->value);
}

void error_identifier_exists(FILE* input, const identifier_t* id) {
    error_print(input);
    fprintf(stderr, "identifier %s already exists in this context\n", id->value);
}

void error_identifier_not_found(FILE* input, const identifier_t* id) {
    error_print(input);
    fprintf(stderr, "identifier %s not found in this context\n", id->value);
}

void error_valref_not_found(FILE* input, const valref_t* valref) {
    error_print(input);
    fprintf(stderr, "valref ");
    valref_print(stderr, valref);
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
