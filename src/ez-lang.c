#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
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

bool function_arg_is(const function_arg_t* arg, const identifier_t* id) {
    return strcmp(arg->symbol->identifier.value, id->value) == 0;
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
    if (arg->modifier == FUNCTION_ARG_MODIFIER_IN) {
        fprintf(output, "const ");
    }
    type_print(output, arg->symbol->is);
    fprintf(output, "&");

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

bool function_has_arg(const function_t* func, const identifier_t* arg) {
    return vector_contains(&func->args, arg, (cmp_func_t)&function_arg_is);
}

function_arg_t* function_find_arg(const function_t* func, const identifier_t* id) {
    return vector_find(&func->args, id, (cmp_func_t)&function_arg_is);
}

bool function_has_local(const function_t* func, const identifier_t* arg) {
    return vector_contains(&func->locals, arg, (cmp_func_t)&symbol_is);
}

symbol_t* function_find_local(const function_t* func, const identifier_t* id) {
    return vector_find(&func->locals, id, (cmp_func_t)&symbol_is);
}

bool function_is(const function_t* func, const identifier_t* id) {
    return strcmp(func->identifier.value, id->value) == 0;
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

bool constant_is(const constant_t* constant, const identifier_t* id) {
    return symbol_is(constant->symbol, id);
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
                    "#include <string>\n"
                    "#include <ctime>\n"
                    "#include <cstdlib>\n"
                    "#include <vector>\n"
                    "\n"
    );

    for (int i = 0; i < prg->structures.size; i++) {
        structure_print(output, prg->structures.elements[i]);
    }
    for (int i = 0; i < prg->constants.size; i++) {
        constant_print(output, prg->constants.elements[i]);
    }
    for (int i = 0; i < prg->globals.size; i++) {
        symbol_print(output, prg->globals.elements[i]);
        fprintf(output, ";\n");
    }

    /* TODO print functions & procedures prototypes after structures
     *      definitions */
    for (int i = 0; i < prg->functions.size; i++) {
        function_print(output, prg->functions.elements[i]);
    }
    for (int i = 0; i < prg->procedures.size; i++) {
        function_print(output, prg->procedures.elements[i]);
    }

    fprintf(output, "int main(int argc, char** argv) {\n"
                    "    std::vector<std::string> args;\n"
                    "    for (int i = 0; i < argc; i++) {\n"
                    "        args.push_back(std::string(argv[i]));\n"
                    "    }\n"
                    "    return %s(args);\n"
                    "}\n",
            prg->identifier.value);
}

bool program_has_global(const program_t* prg, const identifier_t* id) {
    return vector_contains(&prg->globals, id, (cmp_func_t)&symbol_is);
}

symbol_t* program_find_global(const program_t* prg, const identifier_t* id) {
    return vector_find(&prg->globals, id, (cmp_func_t)&symbol_is);
}

bool program_has_constant(const program_t* prg, const identifier_t* id) {
    return vector_contains(&prg->constants, id, (cmp_func_t)&constant_is);
}

constant_t* program_find_constant(const program_t* prg, const identifier_t* id) {
    return vector_find(&prg->constants, id, (cmp_func_t)&constant_is);
}

bool program_has_structure(const program_t* prg, const identifier_t* id) {
    return vector_contains(&prg->structures, id, (cmp_func_t)&structure_is);
}

structure_t* program_find_structure(const program_t* prg, const identifier_t* id) {
    return vector_find(&prg->structures, id, (cmp_func_t)&structure_is);
}

bool program_has_function(const program_t* prg, const identifier_t* id) {
    return vector_contains(&prg->functions, id, (cmp_func_t)&function_is);
}

bool program_has_procedure(const program_t* prg, const identifier_t* id) {
    return vector_contains(&prg->procedures, id, (cmp_func_t)&function_is);
}

bool program_main_function_is_valid(const program_t* prg) {
    const function_t* func = vector_find(&prg->functions, &prg->identifier,
                                         (cmp_func_t)&function_is);
    assert (func);

    if (func->return_type->type != TYPE_TYPE_INTEGER) {
        return false;
    }

    if (func->args.size != 1) {
        return false;
    }

    const function_arg_t* arg = func->args.elements[0];
    if (arg->modifier != FUNCTION_ARG_MODIFIER_IN) {
        return false;
    }
    if (arg->symbol->is->type != TYPE_TYPE_VECTOR) {
        return false;
    }
    if (arg->symbol->is->vector_type->type != TYPE_TYPE_STRING) {
        return false;
    }

    return true;
}
