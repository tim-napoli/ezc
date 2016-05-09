/* Module       : cparser.parser
 * Description  : C simple macros to do parsing.
 * Copyright    : (c) Timothée Napoli, Kevin Hivert, 2016
 * License      : WTFPL
 * Maintainer   : meg@caca.paris
 * Stability    : burn with it or don't try.
 * Portability  : POSIX
 *
 * This should be enough to parse simple grammars.
 */
#ifndef _cparser_parser_h_
#define _cparser_parser_h_

#include <stdio.h>

/* Enumeration for parser function return code. */
typedef enum {
    PARSER_SUCCESS,
    PARSER_FAILURE,
} parser_status_t;

/* A parser is a simple function that, taking as input a FILE* will read a
 * sequence of chars. It returns the number of chars read. */
typedef parser_status_t (*parser_func_t)(FILE*, const void*, void*, int*);

/* Parse using a parser func. If the function failed, return NULL. */
#define PARSE(_parser) \
    if ((_parser) == PARSER_FAILURE) { \
        return PARSER_FAILURE; \
    }

/* Try to parse using a parser func. If the function failed, reset the
 * offset of `input` stream to its original seek. Return the parser status.
 */
#define TRY(_input, _parser) \
    ({ \
        long _offset = ftell(_input); \
        parser_status_t _try_status = (_parser); \
        if (_try_status == PARSER_FAILURE) { \
            fseek(_input, _offset, SEEK_SET); \
        } \
        _try_status; \
    })

#define CHECK_NEXT(_input, _parser) \
    ({ \
        long _offset = ftell(_input); \
        parser_status_t _try_status = (_parser); \
        fseek(_input, _offset, SEEK_SET); \
        _try_status; \
    })

/* Try the parser until it return PARSER_FAILURE. */
#define SKIP_MANY(_input, _parser) \
    { \
        while ((TRY(_input, _parser)) != PARSER_FAILURE) { } \
    }

#define PARSE_MANY  SKIP_MANY

parser_status_t char_parser(FILE* input, const char* allowed, char** output);

parser_status_t word_parser(FILE* input, const char* word, char** output);

parser_status_t until_char_parser(FILE* input, const char* c, char** output);

parser_status_t until_word_parser(FILE* input, const char* word, char** output);


#endif
