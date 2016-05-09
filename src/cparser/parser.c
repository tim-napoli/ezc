#include "cparser/parser.h"

parser_status_t parser_parse(parser_t* parser,
                             FILE* input,
                             const void* args,
                             void* output,
                             int* read)
{
    long original_pos = ftell(input);
    parser_status_t status = parser->parse_func(input, args, output, read);

    if (status == PARSER_FAILURE && (parser->flags & PARSER_FLAG_TRY)) {
        /* If parser failed but has the try flag, we reset to original pos.
         * This will be useful to do combinators. For example, if we have a
         * rule S -> A | B | C, A and B parsers will have the TRY flag, but
         * not the last, so if the last fail, S will fail so.
         */
        fseek(input, original_pos, SEEK_SET);
        return PARSER_FAILURE;
    }

    return status;
}


static int char_is_allowed(const char* allowed, char c) {
    while (*allowed != '\0') {
        if (c == *allowed) {
            return 1;
        }
        allowed++;
    }
    return 0;
}

parser_status_t char_parser_func(FILE* input,
                                 const char* allowed,
                                 char* output,
                                 int *read)
{
    char c = fgetc(input);

    *read = 1;
    if (!char_is_allowed(allowed, c)) {
        return PARSER_FAILURE;
    } else {
        *output = c;
        return PARSER_SUCCESS;
    }
}

parser_status_t digit_parser_func(FILE* input,
                                  const void* unused,
                                  char* output,
                                  int* read)
{
    return char_parser_func(input, "0123456789", output, read);
}

