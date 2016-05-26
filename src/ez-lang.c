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

void function_signature_init(function_signature_t* signature) {
    signature->return_type = NULL;
    vector_init(&signature->args_types, 0);
}

function_signature_t* function_signature_new(void) {
    function_signature_t* signature = malloc(sizeof(function_signature_t));
    if (!signature) {
        fprintf(stderr, "couldn't allocate function signature\n");
        return NULL;
    }
    function_signature_init(signature);
    return signature;
}

void function_signature_wipe(function_signature_t* signature) {
    if (signature->return_type) {
        type_delete(signature->return_type);
    }
    vector_wipe(&signature->args_types, (delete_func_t)&type_delete);
}

bool function_signature_is_equals(const function_signature_t* a,
                                  const function_signature_t* b)
{
    if (!type_is_equals(a->return_type, b->return_type)) {
        return false;
    } else
    if (a->args_types.size != b->args_types.size) {
        return false;
    }

    for (int i = 0; i < a->args_types.size; i++) {
        if (!type_is_equals(a->args_types.elements[i],
                            b->args_types.elements[i]))
        {
            return false;
        }
    }

    return true;
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

void function_set_args(function_t* func, vector_t* args) {
    memcpy(&func->args, args, sizeof(vector_t));
}

bool function_has_arg(const function_t* func, const identifier_t* arg) {
    return vector_contains(&func->args, arg, (cmp_func_t)&function_arg_is);
}

function_arg_t* function_find_arg(const function_t* func, const identifier_t* id) {
    return vector_find(&func->args, id, (cmp_func_t)&function_arg_is);
}

void function_add_local(function_t* func, symbol_t* local) {
    vector_push(&func->locals, local);
}

bool function_has_local(const function_t* func, const identifier_t* arg) {
    return vector_contains(&func->locals, arg, (cmp_func_t)&symbol_is);
}

symbol_t* function_find_local(const function_t* func, const identifier_t* id) {
    return vector_find(&func->locals, id, (cmp_func_t)&symbol_is);
}

void function_set_return_type(function_t* func, type_t* return_type) {
    func->return_type = return_type;
}

void function_set_instructions(function_t* func, vector_t* instructions) {
    memcpy(&func->instructions, instructions, sizeof(vector_t));
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

void program_add_global(program_t* prg, symbol_t* global) {
    vector_push(&prg->globals, global);
}

bool program_has_global(const program_t* prg, const identifier_t* id) {
    return vector_contains(&prg->globals, id, (cmp_func_t)&symbol_is);
}

symbol_t* program_find_global(const program_t* prg, const identifier_t* id) {
    return vector_find(&prg->globals, id, (cmp_func_t)&symbol_is);
}


void program_add_constant(program_t* prg, constant_t* constant) {
    vector_push(&prg->constants, constant);
}

bool program_has_constant(const program_t* prg, const identifier_t* id) {
    return vector_contains(&prg->constants, id, (cmp_func_t)&constant_is);
}

constant_t* program_find_constant(const program_t* prg, const identifier_t* id) {
    return vector_find(&prg->constants, id, (cmp_func_t)&constant_is);
}


void program_add_structure(program_t* prg, structure_t* structure) {
    vector_push(&prg->structures, structure);
}

bool program_has_structure(const program_t* prg, const identifier_t* id) {
    return vector_contains(&prg->structures, id, (cmp_func_t)&structure_is);
}

structure_t* program_find_structure(const program_t* prg,
                                    const identifier_t* id) {
    return vector_find(&prg->structures, id, (cmp_func_t)&structure_is);
}

void program_add_function(program_t* prg, function_t* function) {
    vector_push(&prg->functions, function);
}

bool program_has_function(const program_t* prg, const identifier_t* id) {
    return vector_contains(&prg->functions, id, (cmp_func_t)&function_is);
}

function_t* program_find_function(const program_t* prg,
                                  const identifier_t* id)
{
    return vector_find(&prg->functions, id, (cmp_func_t)&function_is);
}

void program_add_procedure(program_t* prg, function_t* procedure) {
    vector_push(&prg->procedures, procedure);
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

const type_t* context_find_identifier_type(const context_t* ctx,
                                           const identifier_t* id)
{
    if (ctx->function) {
        function_arg_t* arg = function_find_arg(ctx->function, id);
        symbol_t* sym = function_find_local(ctx->function, id);

        if (arg) {
            return arg->symbol->is;
        } else
        if (sym) {
            return sym->is;
        }
    }

    symbol_t* global = program_find_global(ctx->program, id);
    constant_t* constant = program_find_constant(ctx->program, id);

    if (global) {
        return global->is;
    } else
    if (constant) {
        return constant->symbol->is;
    }

    return NULL;
}
