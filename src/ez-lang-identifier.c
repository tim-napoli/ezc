#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ez-lang.h"

bool identifier_set_value(identifier_t* id, char *value) {
    if (strlen(value) >= IDENTIFIER_SIZE) {
        return false;
    }

    strcpy(id->value, value);

    return true;
}

bool identifier_is_reserved(const identifier_t* id) {

    static char* reserved_keywords[] = {
        "program",
        "structure"
        "constant",
        "local",
        "global",
        "is",
        "boolean",
        "integer",
        "natural",
        "real",
        "char",
        "string",
        "vector",
        "of",
        "function",
        "procedure",
        "begin",
        "end",
        "for",
        "in",
        "do",
        "endfor",
        "while",
        "endwhile",
        "loop",
        "until",
        "on",
        "if",
        "elsif",
        "else",
        "endif",
        "print",
        "read",
        "lambda",
        "empty",
    };

    unsigned int nreserved = sizeof(reserved_keywords) / sizeof(char*);

    for (unsigned int i = 0; i < nreserved; i++) {
        if (strcmp(id->value, reserved_keywords[i]) == 0) {
            return true;
        }
    }

    return false;
}
