#ifndef _ez_errors_h_
#define _ez_errors_h_

#include "ez-lang.h"

void error_print(FILE* input);

void error_identifier_is_keyword(FILE* input, const identifier_t* id);

void error_identifier_exists(FILE* input, const identifier_t* id);

void error_identifier_not_found(FILE* input, const identifier_t* id);

void error_valref_not_found(FILE* input, const valref_t* valref);

void error_no_main_function(const identifier_t* id);

void error_invalid_main_function(const identifier_t* id);

void error_expression_not_valid(FILE* input, const expression_t* expr);

void error_parameters_not_valid(FILE* input, const parameters_t* parameters);

#endif /* end of include guard: _ez_errors_h_ */
