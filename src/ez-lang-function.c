#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ez-lang.h"

argument_t* argument_new(type_modifier_t modifier, symbol_t* symbol) {
     context_t *a = calloc(1, sizeof(argument_t));

     if (!a) {
       fprintf(stderr, "couldn't allocate argument\n");
       exit(EXIT_FAILURE);
     }

     a->modifier = modifier;
     a->symbol = symbol;

     return a;
}

argument_t* argument_in_new(symbol_t* symbol) {
    return argument_new(MODIFIER_IN, symbol);
}

argument_t* argument_out_new(symbol_t* symbol) {
    return argument_new(MODIFIER_OUT, symbol);
}

argument_t* argument_inout_new(symbol_t* symbol) {
    return argument_new(MODIFIER_INOUT, symbol);
}
