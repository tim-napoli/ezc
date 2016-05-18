#ifndef _ez_parser_h_
#define _ez_parser_h_

#include "parser.h"
#include "ez-lang.h"

parser_status_t comment_parser(FILE* input, const void* unused_args,
                               void* unused_output);

parser_status_t empty_parser(FILE* input, const void* unused_args,
                             void* unused_output);

parser_status_t space_parser(FILE* input, const void* unused_args,
                             void* unused_output);

parser_status_t comment_or_empty_parser(FILE* input,
                                        const void* unused_args,
                                        void* unused_output);

parser_status_t end_of_line_parser(FILE* input, const void* args,
                                   void* unused_output);

parser_status_t identifier_parser(FILE* input, const void* args,
                                  identifier_t* output);

parser_status_t range_parser(FILE* input, const void* args,
                             void* output);

parser_status_t type_parser(FILE* input, const void* args,
                            void* output);

parser_status_t string_parser(FILE* input, const void* args,
                              char** output);

parser_status_t natural_parser(FILE* input, const void* args,
                               unsigned int* output);

parser_status_t integer_parser(FILE* input, const void* args,
                               int* output);

parser_status_t real_parser(FILE* input, const void* args,
                            double* output);

parser_status_t bool_parser(FILE* input, const void* args,
                            bool* output);

parser_status_t valref_parser(FILE* input, const void* args,
                              valref_t** output);

parser_status_t parameters_parser(FILE* input, const void* args,
                                  parameters_t* output);

parser_status_t value_parser(FILE* input, const void* args,
                             value_t* output);

parser_status_t cmp_op_parser(FILE* input, const void* args,
                              void* output);

parser_status_t bool_op_parser(FILE* input, const void* args,
                               void* output);

parser_status_t arithmetic_op_parser(FILE* input, const void* args,
                                     void* output);

parser_status_t expression_next_parser(FILE* input, const void* args,
                                       void* output);

parser_status_t expression_parser(FILE* input, const void* args,
                                  void* output);

parser_status_t print_parser(FILE* input, const void* args,
                             void* output);

parser_status_t return_parser(FILE* input, const void* args,
                              void* output);

parser_status_t if_parser(FILE* input, const void* args,
                          void* output);

parser_status_t on_parser(FILE* input, const void* args,
                          void* output);

parser_status_t while_parser(FILE* input, const void* args,
                             void* output);

parser_status_t for_parser(FILE* input, const void* args,
                           void* output);

parser_status_t loop_parser(FILE* input, const void* args,
                            void* output);

parser_status_t flowcontrol_parser(FILE* input, const void* args,
                                   void* output);

parser_status_t affectation_parser(FILE* input, const void* args,
                                   void* output);

parser_status_t instruction_parser(FILE* input, const void* args,
                                   void* output);

parser_status_t instructions_parser(FILE* input, const void* args,
                                    void* output);

parser_status_t structure_parser(FILE* input,
                                 const void* unused_args,
                                 void* unused_output);

parser_status_t variable_tail_parser(FILE* input, const void* args,
                                     void* output);

parser_status_t program_parser(FILE* input,
                               const void* unused_args,
                               void* unused_output);

#endif
