#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ez-parser.h"
#include "ez-lang.h"

static void help(void) {
    printf( "usage: ezc [options] source\n"
            "options are:\n"
            " -h        see this help\n"
          );
}

int main(int argc, char** argv) {
    int opt = 0;
    char* input_path = NULL;
    char* output_path = NULL;
    context_t ctx;

    while ((opt = getopt(argc, argv, "h")) >= 0) {
        switch (opt) {
            case 'h':
                help();
                return 0;
        }
    }

    if (argc < optind + 1) {
        help();
        return 1;
    }

    input_path = argv[optind];
    output_path = argv[optind + 1];

    program_t* prg = NULL;
    FILE* input = fopen(input_path, "r");
    if (program_parser(input, &ctx, &prg) != PARSER_SUCCESS) {
        fprintf(stderr, "Program has invalid syntax\n");
        goto error;
    } else if (ctx.error_prg) {
        fprintf(stderr, "Program has semantix error\n");
        goto error;
    } else {
        program_print(stdout, prg);
    }

    program_delete(prg);
    fclose(input);
    return 0;

  error:
    fclose(input);
    program_delete(prg);
    return 1;
}
