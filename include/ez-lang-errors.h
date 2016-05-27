#ifndef _ez_errors_h_
#define _ez_errors_h_

#include "ez-lang.h"

void error_print(FILE* input);

void error_identifier_is_keyword(FILE* input, const identifier_t* id);

void error_identifier_exists(FILE* input, const identifier_t* id);

void error_identifier_not_found(FILE* input, const identifier_t* id);

void error_valref_not_found(FILE* input, const context_t* ctx,
                            const valref_t* valref);

void error_valref_not_valid(FILE* input, const context_t* ctx,
                            const valref_t* valref, const char* suberr);

void error_no_main_function(const identifier_t* id);

void error_invalid_main_function(const identifier_t* id);

void error_expression_not_valid(FILE* input, const context_t* ctx,
                                const expression_t* expr, const char* suberr);

void error_parameters_not_valid(FILE* input, const context_t* ctx,
                                const parameters_t* parameters);

void error_value_not_valid(FILE* input, const context_t* ctx,
                           const value_t* value, const char* suberr);

void error_decleration_not_valid(FILE* input);

void error_affectation_not_valid(FILE* input, const char* suberr);

void error_bad_access_left_value(FILE* input, const valref_t* v);

void error_bad_access_expr_value(FILE* input, const value_t* v);

#endif /* end of include guard: _ez_errors_h_ */
