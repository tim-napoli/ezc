#include <string.h>
#include "parser.h"

static int char_is_allowed(const char* allowed, char c) {
    if (allowed == NULL || *allowed == '\0') {
        return 1;
    }

    while (*allowed != '\0') {
        if (c == *allowed) {
            return 1;
        }
        allowed++;
    }
    return 0;
}

parser_status_t char_parser(FILE* input, const char* allowed, char** output)
{
    char c = fgetc(input);

    if (!char_is_allowed(allowed, c)) {
        return PARSER_FAILURE;
    } else {
        if (output) {
            **output = c;
            (*output)++;
            **output = '\0';
        }
        return PARSER_SUCCESS;
    }
}

parser_status_t word_parser(FILE* input, const char* word, char** output) {
    char allowed[2];

    while (*word != '\0') {
        if (feof(input)) {
            return PARSER_FAILURE;
        }

        allowed[0] = *word;
        allowed[1] = '\0';
        if (char_parser(input, allowed, output) == PARSER_FAILURE) {
            return PARSER_FAILURE;
        }

        word++;
    }
    return PARSER_SUCCESS;
}

parser_status_t until_char_parser(FILE* input, const char* c, char** output)
{
    while (CHECK_NEXT(input, char_parser(input, c, NULL)) == PARSER_FAILURE) {
        PARSE(char_parser(input, NULL, output));
        if (feof(input)) {
            return PARSER_FAILURE;
        }
    }
    return PARSER_SUCCESS;
}

parser_status_t until_word_parser(FILE* input, const char* word, char** output)
{
    while (CHECK_NEXT(input, word_parser(input, word, NULL)) == PARSER_FAILURE)
    {
        PARSE(char_parser(input, NULL, output));
        if (feof(input)) {
            return PARSER_FAILURE;
        }
    }
    return PARSER_SUCCESS;
}


void get_file_coordinates(FILE* f, int* line, int* column, char* c) {
    long offset = ftell(f);

    *line = 1;
    *column = 1;
    fseek(f, 0, SEEK_SET);
    while (ftell(f) < offset) {
        *c = fgetc(f);
        if (*c == '\n') {
            (*line)++;
            *column = 1;
        } else {
            (*column)++;
        }
    }

    fseek(f, offset, SEEK_SET);
}

#if 0

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



parser_status_t digit_parser_func(FILE* input,
                                  const void* unused,
                                  char* output,
                                  int* read)
{
    return char_parser_func(input, "0123456789", output, read);
}

#endif

