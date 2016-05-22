#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ez-parser.h"
#include "ez-lang.h"

static void help(void) {
    printf( "usage: ezc [options] source target\n"
            "options are:\n"
            " -h        see this help\n"
          );
}

int main(int argc, char** argv) {
    int opt = 0;
    char* input_path = NULL;
    char* output_path = NULL;

    while ((opt = getopt(argc, argv, "h")) >= 0) {
        switch (opt) {
            case 'h':
                help();
                return 0;
        }
    }

    if (argc < optind + 2) {
        help();
        return 1;
    }

    input_path = argv[optind];
    output_path = argv[optind + 1];

    program_t* prg = NULL;
    FILE* input = fopen(input_path, "r");
    if (program_parser(input, NULL, &prg) == PARSER_FAILURE) {
        fprintf(stderr, "parser failure\n");
    }
    program_delete(prg);

    fclose(input);

    return 0;
}
