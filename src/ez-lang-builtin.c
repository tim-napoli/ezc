#include <string.h>
#include <assert.h>
#include "ez-lang.h"

enum {
    VECTOR_FUNC_PUSH,
    VECTOR_FUNC_INSERT,
    VECTOR_FUNC_REMOVE,
    VECTOR_FUNC_POP,
    VECTOR_FUNC_CLEAR,
    VECTOR_FUNC_SIZE,
    VECTOR_FUNC_AT,
    VECTOR_FUNC_NFUNCTIONS,
};

static const char* vector_functions[VECTOR_FUNC_NFUNCTIONS] = {
    [VECTOR_FUNC_PUSH]   = "push",
    [VECTOR_FUNC_INSERT] = "insert",
    [VECTOR_FUNC_REMOVE] = "remove",
    [VECTOR_FUNC_POP]    = "pop",
    [VECTOR_FUNC_CLEAR]  = "clear",
    [VECTOR_FUNC_SIZE]   = "size",
    [VECTOR_FUNC_AT]     = "at",
};

static int vector_get_function(const identifier_t* func) {
    for (int i = 0; i < VECTOR_FUNC_NFUNCTIONS; i++) {
        if (strcmp(func->value, vector_functions[i]) == 0) {
            return i;
        }
    }
    return -1;
}

bool vector_function_exists(const identifier_t* func) {
    return vector_get_function(func) >= 0;
}

bool vector_function_call_is_valid(const context_t* ctx,
                                   const valref_t* valref,
                                   const type_t* vector_type)
{
    assert (valref->is_funccall);
    assert (vector_function_exists(&valref->identifier));

    int func = vector_get_function(&valref->identifier);
    switch (func) {
      case VECTOR_FUNC_PUSH: {
        if (valref->parameters.parameters.size != 1) {
            return false;
        }
        const type_t* arg_type =
            context_expression_get_type(ctx,
                                valref->parameters.parameters.elements[0]);
        bool res = types_are_equals(arg_type, vector_type->vector_type);

        if (!res) {
            return false;
        }
        break;
      }

      case VECTOR_FUNC_INSERT: {
        if (valref->parameters.parameters.size != 2) {
            return false;
        }
        const type_t* arg_type_1 =
            context_expression_get_type(ctx,
                                valref->parameters.parameters.elements[0]);
        const type_t* arg_type_2 =
            context_expression_get_type(ctx,
                                valref->parameters.parameters.elements[1]);
        bool res =  type_is_number(arg_type_1)
                 && types_are_equals(arg_type_2, vector_type->vector_type);

        if (!res) {
            return false;
        }
        break;
      }

      case VECTOR_FUNC_REMOVE: {
        if (valref->parameters.parameters.size != 1) {
            return false;
        }
        const type_t* arg_type =
            context_expression_get_type(ctx,
                                valref->parameters.parameters.elements[0]);
        bool res = type_is_number(arg_type);

        if (!res) {
            return false;
        }
        break;
      }

      case VECTOR_FUNC_POP: {
        if (valref->parameters.parameters.size != 0) {
            return false;
        }
        break;
      }

      case VECTOR_FUNC_CLEAR: {
        if (valref->parameters.parameters.size != 0) {
            return false;
        }
        break;
      }

      case VECTOR_FUNC_SIZE: {
        if (valref->parameters.parameters.size != 0) {
            return false;
        }
        break;
      }

      case VECTOR_FUNC_AT: {
        if (valref->parameters.parameters.size != 1) {
            return false;
        }
        const type_t* arg_type =
            context_expression_get_type(ctx,
                                valref->parameters.parameters.elements[0]);
        /* TODO type_is_number */
        bool res = type_is_number(arg_type);
        if (!res) {
            return false;
        }
        break;
      }
    }

    return true;
}

const type_t* vector_function_get_type(const valref_t* valref,
                                       const type_t* vector_type)
{
    int func = vector_get_function(&valref->identifier);

    switch (func) {
      case VECTOR_FUNC_AT:
        return vector_type->vector_type;

      case VECTOR_FUNC_SIZE:
        return type_natural;

      default:
        return NULL;
    }

    return NULL;
}






enum {
    OPTIONAL_FUNC_IS_SET,
    OPTIONAL_FUNC_SET,
    OPTIONAL_FUNC_GET,
    OPTIONAL_FUNC_NFUNCTIONS,
};

static const char* optional_functions[OPTIONAL_FUNC_NFUNCTIONS] = {
    [OPTIONAL_FUNC_IS_SET] = "is_set",
    [OPTIONAL_FUNC_GET]    = "get",
    [OPTIONAL_FUNC_SET]    = "set",
};

static int optional_get_function(const identifier_t* func) {
    for (int i = 0; i < OPTIONAL_FUNC_NFUNCTIONS; i++) {
        if (strcmp(func->value, optional_functions[i]) == 0) {
            return i;
        }
    }
    return -1;
}

bool optional_function_exists(const identifier_t* func) {
    return optional_get_function(func) >= 0;
}

bool optional_function_call_is_valid(const context_t* ctx,
                                     const valref_t* valref,
                                     const type_t* optional_type)
{
    assert (valref->is_funccall);
    assert (optional_function_exists(&valref->identifier));

    int func = optional_get_function(&valref->identifier);
    switch (func) {
      case OPTIONAL_FUNC_IS_SET: {
        if (valref->parameters.parameters.size != 0) {
            return false;
        }
        break;
      }

      case OPTIONAL_FUNC_SET: {
        if (valref->parameters.parameters.size != 1) {
            return false;
        }
        const type_t* arg_type =
            context_expression_get_type(ctx,
                                valref->parameters.parameters.elements[0]);
        bool res = types_are_equals(arg_type, optional_type->optional_type);
        if (!res) {
            return false;
        }
        break;
      }

      case OPTIONAL_FUNC_GET: {
        if (valref->parameters.parameters.size != 0) {
            return false;
        }
        break;
      }
    }

    return true;
}

const type_t* optional_function_get_type(const valref_t* valref,
                                         const type_t* optional_type)
{
    int func = optional_get_function(&valref->identifier);

    switch (func) {
      case OPTIONAL_FUNC_IS_SET:
        return type_boolean;

      case OPTIONAL_FUNC_GET:
        return optional_type->optional_type;

      default:
        return NULL;
    }

    return NULL;
}

