#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ez-lang.h"

function_arg_t* function_arg_new(function_arg_modifier_t modifier,
                                 symbol_t* symbol)
{
    function_arg_t* func_arg = malloc(sizeof(function_arg_t));
    if (!func_arg) {
        fprintf(stderr, "couldn't allocate function argument\n");
        return NULL;
    }

    func_arg->modifier = modifier;
    func_arg->symbol = symbol;

    return func_arg;
}

void function_arg_delete(function_arg_t* arg) {
    symbol_delete(arg->symbol);
    free(arg);
}

function_t* function_new(const identifier_t* id) {
    function_t* f = malloc(sizeof(function_t));
    if (!f) {
        fprintf(stderr, "couldn't allocate function\n");
        return NULL;
    }

    memcpy(&f->identifier, id, sizeof(identifier_t));
    vector_init(&f->args, 0);
    vector_init(&f->locals, 0);
    vector_init(&f->instructions, 0);

    f->return_type = NULL;

    return f;
}

void function_delete(function_t* func) {
    if (func->return_type) {
        type_delete(func->return_type);
    }

    vector_wipe(&func->args, (delete_func_t)&function_arg_delete);
    vector_wipe(&func->locals, (delete_func_t)&symbol_delete);
    vector_wipe(&func->instructions, (delete_func_t)&instruction_delete);

    free(func);
}

