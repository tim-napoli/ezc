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

void function_arg_print(FILE* output, const function_arg_t* arg) {
    type_print(output, arg->symbol->is);
    if (arg->modifier == FUNCTION_ARG_MODIFIER_OUT
    ||  arg->modifier == FUNCTION_ARG_MODIFIER_INOUT)
    {
        fprintf(output, "&");
    }

    fprintf(output, " %s", arg->symbol->identifier.value);
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

void function_print(FILE* output, const function_t* function) {
    if (function->return_type) {
        type_print(output, function->return_type);
        fprintf(output, " ");
    } else {
        fprintf(output, "void ");
    }

    fprintf(output, "%s(", function->identifier.value);

    for (int i = 0; i < function->args.size; i++) {
        function_arg_print(output, function->args.elements[i]);
        if (i + 1 < function->args.size) {
            fprintf(output, ", ");
        }
    }

    fprintf(output, ") {\n");

    for (int i = 0; i < function->locals.size; i++) {
        symbol_print(output, function->locals.elements[i]);
        fprintf(output, ";\n");
    }

    instructions_print(output, &function->instructions);
    fprintf(output, "}\n\n");
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

void constant_print(FILE* output, const constant_t* constant) {
    fprintf(output, "const ");
    symbol_print(output, constant->symbol);
    fprintf(output, " = ");
    expression_print(output, constant->value);
    fprintf(output, ";\n");
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

void program_print(FILE* output, const program_t* prg) {
    fprintf(output, "#include <iostream>\n"
                    "#include <vector>\n\n");

    for (int i = 0; i < prg->structures.size; i++) {
        structure_print(output, prg->structures.elements[i]);
    }
    for (int i = 0; i < prg->constants.size; i++) {
        constant_print(output, prg->constants.elements[i]);
    }
    for (int i = 0; i < prg->globals.size; i++) {
        symbol_print(output, prg->globals.elements[i]);
    }

    /* TODO print functions & procedures prototypes after structures
     *      definitions */
    for (int i = 0; i < prg->functions.size; i++) {
        function_print(output, prg->functions.elements[i]);
    }
    for (int i = 0; i < prg->procedures.size; i++) {
        function_print(output, prg->procedures.elements[i]);
    }
}

