#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ez-lang.h"

function_arg_t* function_arg_new(access_type_t access_type,
                                 symbol_t* symbol)
{
    function_arg_t* func_arg = malloc(sizeof(function_arg_t));
    if (!func_arg) {
        fprintf(stderr, "couldn't allocate function argument\n");
        return NULL;
    }

    func_arg->access_type = access_type;
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
    vector_init(&signature->args_access, 0);
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

function_signature_t*
function_signature_copy(const function_signature_t* signature)
{
    function_signature_t* copy = function_signature_new();

    for (int i = 0; i < signature->args_types.size; i++) {
        vector_push(&copy->args_types,
                    type_copy(signature->args_types.elements[i]));
        vector_push(&copy->args_access,
                    (void*)signature->args_access.elements[i]);
    }
    if (signature->return_type) {
        copy->return_type = type_copy(signature->return_type);
    }

    return copy;
}

void function_signature_wipe(function_signature_t* signature) {
    if (signature->return_type) {
        type_delete(signature->return_type);
    }
    vector_wipe(&signature->args_types, (delete_func_t)&type_delete);
    vector_wipe(&signature->args_access, NULL);
}

void function_signature_delete(function_signature_t* signature) {
    function_signature_wipe(signature);
    free(signature);
}

bool function_signature_is_equals(const function_signature_t* a,
                                  const function_signature_t* b)
{
    if (!types_are_equals(a->return_type, b->return_type)) {
        return false;
    } else
    if (a->args_types.size != b->args_types.size) {
        return false;
    }

    for (int i = 0; i < a->args_types.size; i++) {
        if (!types_are_equals(a->args_types.elements[i],
                              b->args_types.elements[i]))
        {
            return false;
        }

        access_type_t a_at = (access_type_t)a->args_access.elements[i];
        access_type_t b_at = (access_type_t)b->args_access.elements[i];
        if (a_at != b_at) {
            return false;
        }
    }

    return true;
}

const char* access_type_print_ez(access_type_t at, char* buf) {
    switch (at) {
      case ACCESS_TYPE_INPUT:
        strcat(buf, "in");
        break;

      case ACCESS_TYPE_OUTPUT:
        strcat(buf, "out");
        break;

      case ACCESS_TYPE_INPUT_OUTPUT:
        strcat(buf, "inout");
        break;
    }
    return buf;
}

const char* function_signature_print_ez(const function_signature_t* signature,
                                        char* buf)
{
    strcat(buf, "(");
    for (int i = 0; i < signature->args_types.size; i++) {
        access_type_print_ez((access_type_t)signature->args_access.elements[i],
                             buf);
        strcat(buf, " ");
        type_print_ez(signature->args_types.elements[i], buf);
        if (i + 1 < signature->args_types.size) {
            strcat(buf, ", ");
        }
    }
    strcat(buf, ")");

    if (signature->return_type) {
        strcat(buf, " return ");
        type_print_ez(signature->return_type, buf);
    }

    return buf;
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
    f->function_type = NULL;

    return f;
}

void function_arg_print(FILE* output, const context_t* ctx,
                        const function_arg_t* arg)
{
    if (arg->access_type == ACCESS_TYPE_INPUT) {
        fprintf(output, "const ");
    }
    type_print(output, ctx, arg->symbol->is);
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

    if (func->function_type) {
        type_delete(func->function_type);
    }

    free(func);
}

void function_print(FILE* output, const context_t* ctx,
                    const function_t* function)
{
    if (function->return_type) {
        type_print(output, ctx, function->return_type);
        fprintf(output, " ");
    } else {
        fprintf(output, "void ");
    }

    fprintf(output, "%s(", function->identifier.value);

    for (int i = 0; i < function->args.size; i++) {
        function_arg_print(output, ctx, function->args.elements[i]);
        if (i + 1 < function->args.size) {
            fprintf(output, ", ");
        }
    }

    fprintf(output, ") {\n");

    for (int i = 0; i < function->locals.size; i++) {
        symbol_print(output, ctx, function->locals.elements[i]);
        fprintf(output, ";\n");
    }

    instructions_print(output, ctx, &function->instructions);
    fprintf(output, "}\n\n");
}

void function_prototype_print(FILE* output, const context_t* ctx,
                              const function_t* function)
{
    if (function->return_type) {
        type_print(output, ctx, function->return_type);
        fprintf(output, " ");
    } else {
        fprintf(output, "void ");
    }

    fprintf(output, "%s(", function->identifier.value);

    for (int i = 0; i < function->args.size; i++) {
        function_arg_print(output, ctx, function->args.elements[i]);
        if (i + 1 < function->args.size) {
            fprintf(output, ", ");
        }
    }

    fprintf(output, ");\n");
}

void function_set_args(function_t* func, vector_t* args) {
    memcpy(&func->args, args, sizeof(vector_t));
}

bool function_has_arg(const function_t* func, const identifier_t* arg) {
    return vector_contains(&func->args, arg, (cmp_func_t)&function_arg_is);
}

function_arg_t* function_find_arg(const function_t* func,
                                  const identifier_t* id) {
    return vector_find(&func->args, id, (cmp_func_t)&function_arg_is);
}

bool access_type_is_input(const access_type_t access_type) {
    return access_type == ACCESS_TYPE_INPUT
        || access_type == ACCESS_TYPE_INPUT_OUTPUT;
}

bool access_type_is_output(const access_type_t access_type) {
    return access_type == ACCESS_TYPE_OUTPUT
        || access_type == ACCESS_TYPE_INPUT_OUTPUT;
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

function_signature_t* function_get_signature(function_t* func) {
    if (func->function_type) {
        return func->function_type->signature;
    }

    function_signature_t* signature = function_signature_new();
    if (func->return_type) {
        signature->return_type = type_copy(func->return_type);
    }

    for (int i = 0; i < func->args.size; i++) {
        const function_arg_t* arg = func->args.elements[i];
        vector_push(&signature->args_types, type_copy(arg->symbol->is));
        vector_push(&signature->args_access, (void*)arg->access_type);
    }

    func->function_type = type_function_new(signature);
    return signature;
}

const type_t* function_get_type(function_t* func) {
    if (func->function_type == NULL) {
        /* XXX this is ugly */
        function_get_signature(func);
    }
    return func->function_type;
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
    free(constant);
}

void constant_print(FILE* output, const context_t* ctx,
                    const constant_t* constant)
{
    fprintf(output, "const ");
    symbol_print(output, ctx, constant->symbol);
    fprintf(output, " = ");
    expression_print(output, ctx, constant->value);
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

    vector_init(&prg->builtin_functions, 0);
    vector_init(&prg->builtin_procedures, 0);
    vector_init(&prg->builtin_structures, 0);

    return prg;
}

void program_delete(program_t* prg) {
    vector_wipe(&prg->globals, (delete_func_t)&symbol_delete);
    vector_wipe(&prg->constants, (delete_func_t)&constant_delete);
    vector_wipe(&prg->structures, (delete_func_t)&structure_delete);
    vector_wipe(&prg->functions, (delete_func_t)&function_delete);
    vector_wipe(&prg->procedures, (delete_func_t)&function_delete);

    vector_wipe(&prg->builtin_functions, (delete_func_t)&function_delete);
    vector_wipe(&prg->builtin_procedures, (delete_func_t)&function_delete);
    vector_wipe(&prg->builtin_structures, (delete_func_t)&structure_delete);

    free(prg);
}

void program_print(FILE* output, const program_t* prg) {
    fprintf(output, "#include <iostream>\n"
                    "#include <string>\n"
                    "#include <ctime>\n"
                    "#include <cstdlib>\n"
                    "#include <functional>\n"
                    "#include \"ez/vector.hpp\"\n"
                    "#include \"ez/optional.hpp\"\n"
                    "#include \"ez/functions.hpp\"\n"
                    "\n"
    );

    const context_t ctx = (context_t){
        .program = (program_t*)prg,
        .function = NULL
    };

    for (int i = 0; i < prg->structures.size; i++) {
        structure_print(output, &ctx, prg->structures.elements[i]);
    }
    fprintf(output, "\n");

    for (int i = 0; i < prg->functions.size; i++) {
        function_prototype_print(output, &ctx, prg->functions.elements[i]);
    }
    for (int i = 0; i < prg->procedures.size; i++) {
        function_prototype_print(output, &ctx, prg->procedures.elements[i]);
    }
    fprintf(output, "\n");

    for (int i = 0; i < prg->constants.size; i++) {
        constant_print(output, &ctx, prg->constants.elements[i]);
    }
    for (int i = 0; i < prg->globals.size; i++) {
        symbol_print(output, &ctx, prg->globals.elements[i]);
        fprintf(output, ";\n");
    }

    for (int i = 0; i < prg->functions.size; i++) {
        function_print(output, &ctx, prg->functions.elements[i]);
    }
    for (int i = 0; i < prg->procedures.size; i++) {
        function_print(output, &ctx, prg->procedures.elements[i]);
    }

    fprintf(output, "int main(int argc, char** argv) {\n"
                    "    ez::vector<std::string> args;\n"
                    "    for (int i = 0; i < argc; i++) {\n"
                    "        args.push(std::string(argv[i]));\n"
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

constant_t* program_find_constant(const program_t* prg,
                                  const identifier_t* id) {
    return vector_find(&prg->constants, id, (cmp_func_t)&constant_is);
}


void program_add_structure(program_t* prg, structure_t* structure) {
    vector_push(&prg->structures, structure);
}

bool program_has_structure(const program_t* prg, const identifier_t* id) {
    return vector_contains(&prg->structures, id, (cmp_func_t)&structure_is)
        || vector_contains(&prg->builtin_structures, id,
                           (cmp_func_t)&structure_is);
}

structure_t* program_find_structure(const program_t* prg,
                                    const identifier_t* id)
{
    structure_t* structure =
                  vector_find(&prg->structures, id, (cmp_func_t)&structure_is);
    if (!structure) {
        structure = vector_find(&prg->builtin_structures, id,
                                (cmp_func_t)&structure_is);
    }
    return structure;
}

void program_add_function(program_t* prg, function_t* function) {
    vector_push(&prg->functions, function);
}

bool program_has_function(const program_t* prg, const identifier_t* id) {
    return vector_contains(&prg->functions, id, (cmp_func_t)&function_is)
        || program_has_builtin_function(prg, id);
}

function_t* program_find_function(program_t* prg,
                                  const identifier_t* id)
{
    function_t* func = vector_find(&prg->functions, id,
                                   (cmp_func_t)&function_is);
    if (!func) {
        func = program_find_builtin_function(prg, id);
    }

    return func;
}

void program_add_procedure(program_t* prg, function_t* procedure) {
    vector_push(&prg->procedures, procedure);
}

bool program_has_procedure(const program_t* prg, const identifier_t* id) {
    return vector_contains(&prg->procedures, id, (cmp_func_t)&function_is)
        || program_has_builtin_procedure(prg, id);
}

function_t* program_find_procedure(program_t* prg, const identifier_t* id)
{
    function_t* func = vector_find(&prg->procedures, id,
                                   (cmp_func_t)&function_is);
    if (!func) {
        func = program_find_builtin_procedure(prg, id);
    }
    return func;
}

void program_add_builtin_function(program_t* prg, function_t* function) {
    vector_push(&prg->builtin_functions, function);
}

bool program_has_builtin_function(const program_t* prg, const identifier_t* id)
{
    return vector_contains(&prg->builtin_functions, id,
                           (cmp_func_t)&function_is);
}

function_t* program_find_builtin_function(program_t* prg,
                                          const identifier_t* id)
{
    return vector_find(&prg->builtin_functions, id, (cmp_func_t)&function_is);
}


void program_add_builtin_procedure(program_t* prg, function_t* function) {
    vector_push(&prg->builtin_procedures, function);
}

bool program_has_builtin_procedure(const program_t* prg, const identifier_t* id)
{
    return vector_contains(&prg->builtin_procedures, id,
                           (cmp_func_t)&function_is);
}


function_t* program_find_builtin_procedure(program_t* prg,
                                          const identifier_t* id)
{
    return vector_find(&prg->builtin_procedures, id, (cmp_func_t)&function_is);
}


void program_add_builtin_structure(program_t* prg, structure_t* structure) {
    vector_push(&prg->builtin_structures, structure);
}

bool program_has_builtin_structure(const program_t* prg, const identifier_t* id)
{
    return vector_contains(&prg->builtin_structures, id,
                           (cmp_func_t)&structure_is);
}

structure_t* program_find_builtin_structure(program_t* prg,
                                          const identifier_t* id)
{
    return vector_find(&prg->builtin_structures, id, (cmp_func_t)&structure_is);
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
    if (arg->access_type != ACCESS_TYPE_INPUT) {
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
