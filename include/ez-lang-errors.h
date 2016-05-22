#ifndef _ez_errors_h_
#define _ez_errors_h_

#include "ez-lang.h"

void error_print(FILE* input);

void error_identifier_is_keyword(FILE* input, const identifier_t* id);

void error_symbol_exists(FILE* input, const symbol_t* s);

void error_symbol_not_found(FILE* input, const identifier_t* id);

void error_structure_exists(FILE* input, const structure_t* s);

void error_structure_not_found(FILE* input, const identifier_t* id);

#endif /* end of include guard: _ez_errors_h_ */
