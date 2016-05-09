/* Module       : cparser.parser
 * Description  : C haskell parser clone.
 * Copyright    : (c) Timothée Napoli, Kevin Hivert, 2016
 * License      : WTFPL
 * Maintainer   : meg@caca.paris
 * Stability    : burn with it or don't try.
 * Portability  : POSIX
 *
 * This module is a Haskell's parsec clone.
 * Parsec is a library intended to write LL(1) parsers using a procedural
 * fashion and combinators.
 * This is, a parsec follows a LL(1) grammar where disjonctions are modelized
 * using combinators and rules (or leaves) using parsing function.
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

/* Parser flags. */
typedef enum {
    /* This flag is a special case for parsers. When a parser has this flag,
     * if a parser failed, the input will not be consumed, and PARSER_FAILURE
     * wil be returned. */
    PARSER_FLAG_TRY     = 0x1,
} parser_flag_t;

/* Data structure using to describe a parser function. */
typedef struct parser {
    parser_flag_t flags;      /* Parser flags */
    parser_func_t parse_func; /* Parser function */
} parser_t;

/* This function executes a parser described above.
 * `input` is the file that will be read.
 * `args` are special args that we give to the parser to personalize its
 * behavior.
 * `output` is where we store the parsing result.
 * `read` is the number of characters that have been read.
 * This functions returns the status of the parser : a FAILURE means the input
 * haven't been read. If the flag "TRY" had been set, then the input is not
 * consumed.
 * If the return is SUCCESS then the input has been read and output is set.
 */
parser_status_t parser_parse(parser_t* parser,
                             FILE* input,
                             const void* args,
                             void* output,
                             int* read);

/* Combinator types. */
typedef enum {
    /* This type means that the combinator try to parse the input with one of
     * the given input. All parsers (but not the last) must have the TRY flag.
     */
    COMBNINATOR_TYPE_OR,
} combinator_type_t;

/* (limitation) Maximum number of parsers a combinator can contain. */
#define COMBINATOR_MAX_PARSERS  32

/* Combinator data structure. */
typedef struct combinator {
    combinator_type_t type;
    unsigned int      nparsers;
    parser_t          parsers[COMBINATOR_MAX_PARSERS];
} combinator_t;

#endif

