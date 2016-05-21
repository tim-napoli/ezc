#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

int main(void) {
    FILE* f = NULL;
    char *output = malloc(512 * sizeof(char));
    char *output_ptr;

    char char_ok[] = "a";
    char char_nok[] = "b";

    char words[] = "bob justin";
    char try[] = "boa";

    char skip_many[] = "    bob";

    char skip_until_char[] = "xyze bob";
    char skip_until_word[] = "this is a comment* /bob";

    f = fmemopen(char_ok, sizeof(char_ok), "r");
    *output = '\0';
    output_ptr = output;
    assert (char_parser(f, "a", &output_ptr) == PARSER_SUCCESS);
    assert (strcmp(output, "a") == 0);
    fclose(f);

    f = fmemopen(char_nok, sizeof(char_nok), "r");
    *output = '\0';
    output_ptr = output;
    assert (char_parser(f, "a", &output_ptr) == PARSER_FAILURE);
    assert (strlen(output) == 0);
    fclose(f);

    f = fmemopen(words, sizeof(words), "r");
    *output = '\0';
    output_ptr = output;
    assert (word_parser(f, "bob", &output_ptr) == PARSER_SUCCESS);
    assert (strcmp(output, "bob") == 0);
    assert (char_parser(f, " ", NULL) == PARSER_SUCCESS);
    assert (word_parser(f, "justin", NULL) == PARSER_SUCCESS);
    fclose(f);

    f = fmemopen(try, sizeof(try), "r");
    assert (TRY(f, word_parser(f, "bob", NULL) == PARSER_FAILURE));
    assert (TRY(f, word_parser(f, "boa", NULL) == PARSER_SUCCESS));
    fclose(f);

    f = fmemopen(skip_many, sizeof(skip_many), "r");
    SKIP_MANY(f, char_parser(f, " ", NULL));
    assert (word_parser(f, "bob", NULL) == PARSER_SUCCESS);
    fclose(f);

    f = fmemopen(skip_until_char, sizeof(skip_until_char), "r");
    assert (until_char_parser(f, "b", NULL) == PARSER_SUCCESS);
    assert (word_parser(f, "bob", NULL) == PARSER_SUCCESS);
    assert (until_char_parser(f, "b", NULL) == PARSER_FAILURE);
    fclose(f);

    f = fmemopen(skip_until_word, sizeof(skip_until_word), "r");
    assert (until_word_parser(f, "*/", NULL) == PARSER_SUCCESS);
    assert (word_parser(f, "*/", NULL) == PARSER_SUCCESS);
    assert (word_parser(f, "bob", NULL) == PARSER_SUCCESS);
    assert (until_word_parser(f, "*/", NULL) == PARSER_FAILURE);
    fclose(f);

    free(output);
    return 0;
}

