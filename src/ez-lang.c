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

constant_t* constant_new(symbol_t* symbol, expression_t* value) {
    constant_t* constant = malloc(sizeof(constant_t));
    if (!constant) {
        fprintf(stderr, "couldn't allocate a constant\n");
        return NULL;
    }

    constant->symbol = symbol;
    constant->value = value;

    return constant;
}

void constant_delete(constant_t* constant) {
    symbol_delete(constant->symbol);
    expression_delete(constant->value);
}

program_t* program_new(const identifier_t* id) {
    program_t* prg = malloc(sizeof(program_t));
    if (!prg) {
        fprintf(stderr, "couldn't allocate program\n");
        return NULL;
    }

    memcpy(&prg->identifier, id, sizeof(identifier_t));
    vector_init(&prg->globals, 0);
    vector_init(&prg->constants, 0);
    vector_init(&prg->structures, 0);
    vector_init(&prg->functions, 0);
    vector_init(&prg->procedures, 0);

    return prg;
}

void program_delete(program_t* prg) {
    vector_wipe(&prg->globals, (delete_func_t)&symbol_delete);
    vector_wipe(&prg->constants, (delete_func_t)&constant_delete);
    vector_wipe(&prg->structures, (delete_func_t)&structure_delete);
    vector_wipe(&prg->functions, (delete_func_t)&function_delete);
    vector_wipe(&prg->procedures, (delete_func_t)&function_delete);
    free(prg);
}

