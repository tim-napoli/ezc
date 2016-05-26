#ifndef _ez_lang_h_
#define _ez_lang_h_

#include <stdio.h>
#include <stdbool.h>
#include "vector.h"

#define IDENTIFIER_SIZE             32
#define INSTRUCTIONS_SIZE           128
#define ELSIF_SIZE                  32

typedef struct expression expression_t;

/* ----------------------------- bases ------------------------------------- */

typedef struct identifier {
    char value[IDENTIFIER_SIZE];
} identifier_t;

bool identifier_set_value(identifier_t* id, char *value);
bool identifier_is_reserved(const identifier_t* id);

typedef struct symbol symbol_t;
typedef struct structure structure_t;
typedef struct type type_t;
typedef struct context context_t;

/* ---------------------------- values ------------------------------------- */

typedef struct parameters {
    vector_t      parameters;    /* of expression_t* */
} parameters_t;

void parameters_init(parameters_t* params);

void parameters_add(parameters_t* params, expression_t* expr);

void parameters_wipe(parameters_t* params);

void parameters_print(FILE* output, const parameters_t* params);

typedef struct valref {
    identifier_t  identifier;

    bool         is_funccall;
    parameters_t parameters;

    struct valref* next;
} valref_t;

valref_t* valref_new(const identifier_t* identifier);

void valref_delete(valref_t* valref);

void valref_print(FILE* output, const valref_t* value);

const type_t* valref_get_type(const context_t* ctx, const valref_t* valref);

typedef enum {
    VALUE_TYPE_STRING,
    VALUE_TYPE_REAL,
    VALUE_TYPE_INTEGER,
    VALUE_TYPE_NATURAL,
    VALUE_TYPE_BOOLEAN,
    VALUE_TYPE_VALREF,
} value_type_t;

typedef struct value {
    value_type_t type;
    union {
        char*        string;
        double       real;
        int          integer;
        unsigned int natural;
        bool         boolean;
        valref_t*    valref;
    };
} value_t;

void value_wipe(value_t* value);

parameters_t* valref_get_parameters(valref_t* v);

void valref_set_next(valref_t* v, valref_t* n);
void valref_add_parameter(valref_t* v, expression_t* p);
void valref_set_is_funccall(valref_t* v, bool is_funccall);
void valref_set_has_indexing(valref_t* v, bool has_indexing);
void valref_add_index(valref_t* v, expression_t* index);

void value_print(FILE* output, const value_t* value);

const type_t* value_get_type(const context_t* ctx, const value_t* value);

/* ---------------------------- expressions --------------------------------- */

typedef enum {
    EXPRESSION_TYPE_VALUE,

    EXPRESSION_TYPE_CMP_OP_EQUALS,
    EXPRESSION_TYPE_CMP_OP_DIFFERENT,
    EXPRESSION_TYPE_CMP_OP_LOWER_OR_EQUALS,
    EXPRESSION_TYPE_CMP_OP_GREATER_OR_EQUALS,
    EXPRESSION_TYPE_CMP_OP_LOWER,
    EXPRESSION_TYPE_CMP_OP_GREATER,

    EXPRESSION_TYPE_BOOL_OP_AND,
    EXPRESSION_TYPE_BOOL_OP_OR,
    EXPRESSION_TYPE_BOOL_OP_NOT,

    EXPRESSION_TYPE_ARITHMETIC_OP_PLUS,
    EXPRESSION_TYPE_ARITHMETIC_OP_MINUS,
    EXPRESSION_TYPE_ARITHMETIC_OP_MUL,
    EXPRESSION_TYPE_ARITHMETIC_OP_DIV,
    EXPRESSION_TYPE_ARITHMETIC_OP_MOD,

    EXPRESSION_TYPE_SIZE
} expression_type_t;

extern int expression_type_predecence[EXPRESSION_TYPE_SIZE];

struct expression {
    expression_type_t type;
    union {
        value_t value;
        struct {
            struct expression *left, *right;
        };
    };
};

expression_t* expression_new(expression_type_t type);

void expression_delete(expression_t* expr);

int expression_predecence(const expression_t* expr);

void expression_print(FILE* output, const expression_t* expr);

const type_t* expression_get_type(const context_t* ctx,
                                  const expression_t* expr);

/* -------------------------- types & symbols ------------------------------ */

typedef enum {
    TYPE_TYPE_BOOLEAN,
    TYPE_TYPE_INTEGER,
    TYPE_TYPE_NATURAL,
    TYPE_TYPE_REAL,
    TYPE_TYPE_CHAR,
    TYPE_TYPE_STRING,
    TYPE_TYPE_VECTOR,
    TYPE_TYPE_STRUCTURE,
} type_type_t;

struct symbol {
    identifier_t identifier;
    type_t* is;
};

struct structure {
    identifier_t identifier;
    vector_t members;   /* of symbol_t* */
};

struct type {
    type_type_t type;
    union {
        structure_t* structure_type;
        type_t* vector_type;
    };
};

type_t *type_new(type_type_t type);
void type_delete(type_t *type);
type_t *type_boolean_new();
type_t *type_integer_new();
type_t *type_natural_new();
type_t *type_real_new();
type_t *type_char_new();
type_t *type_string_new();
type_t *type_vector_new(type_t *of);
type_t* type_structure_new(structure_t* s);

void type_print(FILE* output, const type_t* type);

bool type_are_equals(const type_t* a, const type_t* b);

bool type_are_equaivalent(const type_t* a, const type_t* b);

bool type_is_number(const type_t* type);

type_t* type_copy(const type_t* type);

extern type_t* type_boolean;
extern type_t* type_integer;
extern type_t* type_natural;
extern type_t* type_real;
extern type_t* type_char;
extern type_t* type_string;

symbol_t *symbol_new(const identifier_t *identifier, type_t *is);
void symbol_delete(symbol_t *symbol);

void symbol_print(FILE* output, const symbol_t* symbol);

bool symbol_is(const symbol_t* sym, const identifier_t* id);

structure_t* structure_new(const identifier_t *identifier);
void structure_delete(structure_t *structure);

void structure_add_member(structure_t *structure, symbol_t *member);
bool structure_has_member(const structure_t* structure, const identifier_t* id);
symbol_t* structure_find_member(const structure_t* structure,
                                const identifier_t* id);

void structure_print(FILE* output, const structure_t* structure);

bool structure_is(const structure_t* structure, const identifier_t* id);

/* ---------------------------- instructions ------------------------------- */

typedef struct instruction instruction_t;

typedef struct elsif_instr {
    expression_t*  coundition;
    vector_t instructions;  /* of instruction_t* */
} elsif_instr_t;

elsif_instr_t* elsif_instr_new(expression_t* coundition);

void elsif_instr_delete(elsif_instr_t* elsif);

void elsif_instr_print(FILE* output, const elsif_instr_t* elsif);

typedef struct if_instr {
    expression_t* coundition;
    vector_t instructions;      /* of instruction_t* */
    vector_t elsifs;            /* of elsif_instr_t* */
    vector_t else_instrs;       /* of instruction_t* */
} if_instr_t;

if_instr_t* if_instr_new(expression_t* coundition);

void if_instr_delete(if_instr_t* if_instr);

void if_instr_print(FILE* output, const if_instr_t* if_instr);

typedef struct loop_instr {
    expression_t* coundition;
    vector_t      instructions;     /* of instruction_t* */
} loop_instr_t;

loop_instr_t* loop_instr_new(expression_t* coundition);

void loop_instr_delete(loop_instr_t* loop);

void loop_instr_print(FILE* output, const loop_instr_t* loop_instr);

typedef struct while_instr {
    expression_t* coundition;
    vector_t      instructions;     /* of instruction_t* */
} while_instr_t;

while_instr_t* while_instr_new(expression_t* coundition);

void while_instr_delete(while_instr_t* while_instr);

void while_instr_print(FILE* output, const while_instr_t* while_instr);

typedef struct on_instr {
    expression_t* coundition;
    instruction_t* instruction;
} on_instr_t;

on_instr_t* on_instr_new(expression_t* coundition);

void on_instr_delete(on_instr_t* on_instr);

void on_instr_print(FILE* output, const on_instr_t* on_instr);

typedef struct range {
    expression_t* from;
    expression_t* to;
} range_t;

typedef struct for_instr {
    identifier_t subject;
    range_t      range;
    vector_t     instructions;  /* of instruction_t */
} for_instr_t;

for_instr_t* for_instr_new(const identifier_t* subject);


void range_set_from(range_t* range, expression_t* from);

void range_set_to(range_t* range, expression_t* to);

void for_instr_delete(for_instr_t* for_instr);

void for_instr_print(FILE* output, const for_instr_t* for_instr);

typedef enum {
    FLOWCONTROL_TYPE_IF,
    FLOWCONTROL_TYPE_WHILE,
    FLOWCONTROL_TYPE_LOOP,
    FLOWCONTROL_TYPE_ON,
    FLOWCONTROL_TYPE_FOR,
} flowcontrol_type_t;

typedef struct flowcontrol {
    flowcontrol_type_t type;
    union {
        if_instr_t*     if_instr;
        while_instr_t*  while_instr;
        loop_instr_t*   loop_instr;
        on_instr_t*     on_instr;
        for_instr_t*    for_instr;
    };
} flowcontrol_t;

void flowcontrol_wipe(flowcontrol_t* fc);

void flowcontrol_print(FILE* output, const flowcontrol_t* fc_instr);

typedef struct affectation_instr {
    valref_t*       lvalue;
    expression_t*   expression;
} affectation_instr_t;

void affectation_instr_wipe(affectation_instr_t* affectation);

void affectation_instr_print(FILE* output, const affectation_instr_t* aff);

typedef enum {
    INSTRUCTION_TYPE_PRINT,
    INSTRUCTION_TYPE_READ,
    INSTRUCTION_TYPE_RETURN,
    INSTRUCTION_TYPE_FLOWCONTROL,
    INSTRUCTION_TYPE_EXPRESSION,
    INSTRUCTION_TYPE_AFFECTATION,
} instruction_type_t;

typedef struct instruction {
    instruction_type_t type;
    union {
        parameters_t  parameters;
        valref_t*     valref;
        flowcontrol_t flowcontrol;
        expression_t* expression;
        affectation_instr_t affectation;
    };
} instruction_t;

instruction_t* instruction_new(instruction_type_t type);

void instruction_delete(instruction_t* instr);

void instruction_print(FILE* output, const instruction_t* instr);

void instructions_print(FILE* output, const vector_t* instrs);

/* ------------------------------ functions -------------------------------- */

typedef enum {
    FUNCTION_ARG_MODIFIER_IN,
    FUNCTION_ARG_MODIFIER_OUT,
    FUNCTION_ARG_MODIFIER_INOUT,
} function_arg_modifier_t;

typedef struct function_arg {
    function_arg_modifier_t modifier;
    symbol_t*               symbol;
} function_arg_t;

function_arg_t* function_arg_new(function_arg_modifier_t modifier,
                                 symbol_t* symbol);

void function_arg_delete(function_arg_t* func_arg);

void function_arg_print(FILE* output, const function_arg_t* arg);

typedef struct function_signature {
    type_t*     return_type;
    vector_t    args_types;     /* of type_t* */
} function_signature_t;

void function_signature_init(function_signature_t* signature);

function_signature_t* function_signature_new(void);

void function_signature_wipe(function_signature_t* signature);

void function_signature_delete(function_signature_t* signature);

bool function_signature_is_equals(const function_signature_t* a,
                                  const function_signature_t* b);

typedef struct function {
    identifier_t identifier;

    type_t* return_type;

    vector_t args;          /* of function_arg_t* */
    vector_t locals;        /* of symbol_t* */
    vector_t instructions;  /* of instruction_t* */
} function_t;

function_t* function_new(const identifier_t* id);

void function_delete(function_t* function);

void function_print(FILE* output, const function_t* function);

void function_set_args(function_t* func, vector_t* args);
bool function_has_arg(const function_t* func, const identifier_t* arg);
function_arg_t* function_find_arg(const function_t* func,
                                  const identifier_t* arg);

void function_add_local(function_t* func, symbol_t* local);
bool function_has_local(const function_t* func, const identifier_t* arg);
symbol_t* function_find_local(const function_t* func, const identifier_t* arg);

void function_set_return_type(function_t* func, type_t* return_type);

void function_set_instructions(function_t* func, vector_t* instructions);

bool function_is(const function_t* func, const identifier_t* id);

/* ------------------------------ constants -------------------------------- */

typedef struct constant {
    symbol_t*     symbol;
    expression_t* value;
} constant_t;

constant_t* constant_new(symbol_t* symbol, expression_t* value);

void constant_delete(constant_t* constant);

void constant_print(FILE* output, const constant_t* constant);

bool constant_is(const constant_t* constant, const identifier_t* id);

/* ------------------------------ program ---------------------------------- */

typedef struct program {
    identifier_t    identifier;

    vector_t    globals;    /* of symbol_t* */
    vector_t    constants;  /* of constant_t* */
    vector_t    structures; /* of structure_t* */
    vector_t    functions;  /* of function_t* */
    vector_t    procedures; /* of function_t* */
} program_t;

program_t* program_new(const identifier_t* id);

void program_delete(program_t* prg);

void program_print(FILE* output, const program_t* prg);

void program_add_global(program_t* prg, symbol_t* global);
bool program_has_global(const program_t* prg, const identifier_t* id);
symbol_t* program_find_global(const program_t* prg, const identifier_t* id);

void program_add_constant(program_t* prg, constant_t* constant);
bool program_has_constant(const program_t* prg, const identifier_t* id);
constant_t* program_find_constant(const program_t* prg, const identifier_t* id);

void program_add_structure(program_t* prg, structure_t* structure);
bool program_has_structure(const program_t* prg, const identifier_t* id);
structure_t* program_find_structure(const program_t* prg,
                                    const identifier_t* id);

void program_add_function(program_t* prg, function_t* function);
bool program_has_function(const program_t* prg, const identifier_t* id);
function_t* program_find_function(const program_t* prg, const identifier_t* id);

void program_add_procedure(program_t* prg, function_t* procedure);
bool program_has_procedure(const program_t* prg, const identifier_t* id);
function_t* program_find_procedure(const program_t* prg, const identifier_t* id);

bool program_main_function_is_valid(const program_t* prg);

/* ------------------------------ contexts --------------------------------- */

struct context {
    const program_t* program;
    const function_t* function;
};

void context_init(context_t* ctx);
void context_set_program(context_t* ctx, program_t* prg);
void context_set_function(context_t* ctx, function_t* func);

bool context_has_function(const context_t* ctx, const identifier_t* id);

structure_t* context_find_structure(const context_t* ctx,
                                    const identifier_t* structure_id);

identifier_t context_get_program_identifier(context_t* ctx);

bool context_has_identifier(const context_t* ctx, const identifier_t* id);

bool context_valref_is_valid(const context_t* ctx, const valref_t* valref);
bool context_valref_next_is_valid(const valref_t* valref,
                                  const symbol_t* symbol);

bool context_value_is_valid(const context_t* ctx, const value_t* value);

bool context_expression_is_valid(const context_t* ctx, const expression_t* e);

bool context_parameters_is_valid(const context_t* ctx, const parameters_t* p);

const type_t* context_find_identifier_type(const context_t* ctx,
                                           const identifier_t* id);

/* ------------------------ Language builtins ------------------------------ */

bool vector_function_exists(const identifier_t* id);

bool vector_function_call_is_valid(const context_t* ctx,
                                   const valref_t* valref,
                                   const type_t* vector_type);

const type_t* vector_function_get_type(const valref_t* valref,
                                       const type_t* vector_type);

#endif
