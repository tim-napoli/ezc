#include <string.h>
#include <stdlib.h>
#include "ez-parser.h"
#include "ez-lang.h"

int main(int argc, char** argv) {
    expression_t* expr = NULL;
    FILE* f = NULL;

    if (argc < 2) {
        printf("usage: test-ez-expr <expression>\n");
        return 1;
    }

    f = fmemopen(argv[1], strlen(argv[1]), "r");
    if (expression_parser(f, NULL, &expr) != PARSER_SUCCESS) {
        printf("invalid expression\n");
    } else {
        expression_print(stdout, NULL, expr);
        printf("\n");
        expression_delete(expr);
    }
    fclose(f);

    return 0;
}

