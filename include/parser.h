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
    PARSER_FATAL,
} parser_status_t;

/* A parser is a simple function that, taking as input a FILE* will read a
 * sequence of chars. It returns the number of chars read. */
typedef parser_status_t (*parser_func_t)(FILE*, const void*, void*, int*);

void get_file_coordinates(FILE* f, int* line, int* column, char* c);

/* Parse using a parser func. */
#define PARSE(_parser) \
    { \
        long _status = (_parser); \
        if (_status == PARSER_FAILURE) { \
            return PARSER_FAILURE; \
        } else if (_status == PARSER_FATAL) { \
            return PARSER_FATAL; \
        } \
    }

/* Parse using a parser func. If the function failed, return NULL and print
 * the given error message.
 */
#define PARSE_ERR(_parser, _err_msg) \
    if ((_parser) == PARSER_FAILURE) { \
        int _line, _column; \
        char _c; \
        /* TODO input in PARSE_ERR args */ \
        get_file_coordinates(input, &_line, &_column, &_c); \
        fprintf(stderr, "error (line %d column %d at '%c'): " _err_msg "\n", \
                _line, _column, _c); \
        return PARSER_FATAL; \
    }

#define PARSER_LANG_ERR(_fmt, ...) \
    { \
        int _line, _column; \
        char _c; \
        get_file_coordinates(input, &_line, &_column, &_c); \
        fprintf(stderr, "error (line %d column %d at '%c'): " _fmt "\n", \
                _line, _column, _c, __VA_ARGS__); \
        return PARSER_FATAL; \
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
        } else if (_try_status == PARSER_FATAL) { \
            return PARSER_FATAL; \
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

#define PARSE_MANY SKIP_MANY

parser_status_t char_parser(FILE* input, const char* allowed, char** output);

parser_status_t chars_parser(FILE* input, const char* allowed, char** output);

parser_status_t word_parser(FILE* input, const char* word, char** output);

parser_status_t until_char_parser(FILE* input, const char* c, char** output);

parser_status_t until_word_parser(FILE* input, const char* word,
                                  char** output);


#endif
