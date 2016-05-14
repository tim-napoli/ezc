#pragma once

#include <stdbool.h>
#include <stdint.h>

#define NAME_SIZE 32
#define MAX_GLOBALS 32
#define MAX_LOCALS 32
#define MAX_CONSTANTS 32
#define MAX_FUNCTIONS 32
#define MAX_PROCEDURES 32
#define STRUCT_SIZE 32
#define MAX_ARGS 8
#define MAX_INSTRUCTIONS 150;

typedef enum { BOOLEAN, INTEGER, REAL, CHAR, VECTOR } types;

/**
 * DECLARATIONS
 */

struct program_t {
  uint8_t name[NAME_SIZE];
  struct variable_t globals[MAX_GLOBALS]; // TODO : variable OR vector
  struct constant_t constants[MAX_CONSTANTS];
  struct function_t function[MAX_FUNCTIONS];
  struct procedure_t procedures[MAX_PROCEDURES];
};

struct variable_t {
  uint8_t name[NAME_SIZE];
  enum types;
};

struct constant_t {
  uint8_t name[NAME_SIZE];
  enum types;
  union value {
    bool b, uint32_t i, float r, char c
  };
}

struct structure_t {
  uint8_t name[NAME_SIZE];
  struct t_vars vars[STRUCT_SIZE];
};

struct vector_t {
  uint8_t name[NAME_SIZE];
  // TODO : type. Is enum types OR struct structure_t.
};

struct function_t {
  uint8_t name[NAME_SIZE];
  struct variable_t arguments[MAX_ARGS];
  struct variable_t locals[MAX_LOCALS];
  struct instruction_t instructions[MAX_INSTRUCTIONS];
};

struct procedure_t {
  uint8_t name[NAME_SIZE];
  struct variable_t locals[MAX_LOCALS];
  struct instruction_t instructions[MAX_INSTRUCTIONS];
}

/**
 * INSTRUCTIONS
 */

struct while_t {
  struct expression_node condition;
  struct instruction_t instructions[MAX_INSTRUCTIONS];
};

struct repeat_t {
  struct expression_node condition;
  struct instruction_t instructions[MAX_INSTRUCTIONS];
};

struct for_t {
  struct variable_t i;
  struct constant_t begin;
  struct constant_t end;
  struct instruction_t instructions[MAX_INSTRUCTIONS];
};

struct on_t {
  struct expression_node condition;
  struct instruction_t instructions[MAX_INSTRUCTIONS];
};

struct if_t {
  struct expression_node condition;
  struct instruction_t instructions[MAX_INSTRUCTIONS];
};

struct elsif_t {
  struct expression_node condition;
  struct instruction_t instructions[MAX_INSTRUCTIONS];
};

struct else_t {
  struct instruction_t instructions[MAX_INSTRUCTIONS];
};

struct affectation_t {
  struct variable_t left;
  struct expression_node right;
};

struct function_call_t {
  uint8_t name[NAME_SIZE];
  struct expression_node parameters[MAX_ARGS];
}

// instruction_t => affectation | function_call | on | while | for.
union instruction_t {
  struct repeat_t repeat_loop;
  struct while_t while_loop;
  struct for_t for_loop;
  struct on_t on_condition;
  struct if_t if_condition;
  struct else_t else_condition;
  struct elsif_t elsif_condition;
  struct affectation_t affectation;
  struct function_call_t function_call;
};

/**
 * EXPRESSIONS
 */

union value_t {
  struct constant_t constant;
  struct variable_t variable;
  struct function_call_t function_call;
}

struct expression_node {
  // TODO : TYPE. value or SYMBOL (+, -, *, /, =, <, > etc ... AND ., [])
  union value_t value;
  struct expression_node *left;
  struct expression_node *right;
};
