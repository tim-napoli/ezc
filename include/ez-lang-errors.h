#ifndef _ez_errors_h_
#define _ez_errors_h_

#include "ez-lang.h"

void error_print(FILE* input);

void error_identifier_is_keyword(FILE* input, const identifier_t* id);

void error_identifier_exists(FILE* input, const identifier_t* id);

void error_identifier_not_found(FILE* input, const identifier_t* id);

void error_no_main_function(const identifier_t* id);

void error_invalid_main_function(const identifier_t* id);

#endif /* end of include guard: _ez_errors_h_ */
