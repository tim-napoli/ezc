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

parser_status_t end_of_file_parser(FILE* input, const void* args,
                                   void* output);

parser_status_t identifier_parser(FILE* input, const context_t* ctx,
                                  identifier_t* identifier);

parser_status_t range_parser(FILE* input, const context_t* ctx,
                             range_t* range);

parser_status_t type_parser(FILE* input, const context_t* ctx,
                            type_t** type);

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

parser_status_t valref_parser(FILE* input, const context_t* ctx,
                              valref_t** valref);

parser_status_t parameters_parser(FILE* input, const context_t* ctx,
                                  parameters_t** parameters);

parser_status_t value_parser(FILE* input, const context_t* ctx,
                             value_t* value);

parser_status_t expression_parser(FILE* input, const context_t* ctx,
                                  expression_t** expression);

parser_status_t print_parser(FILE* input, const context_t* ctx,
                             parameters_t* parameters);

parser_status_t return_parser(FILE* input, const context_t* ctx,
                              expression_t** expression);

parser_status_t if_parser(FILE* input, const context_t* ctx,
                          if_instr_t** if_instr);

parser_status_t on_parser(FILE* input, const context_t* ctx,
                          on_instr_t** on_instr);

parser_status_t while_parser(FILE* input, const context_t* ctx,
                             while_instr_t** while_instr);

parser_status_t for_parser(FILE* input, const context_t* ctx,
                           for_instr_t** for_instr);

parser_status_t loop_parser(FILE* input, const context_t* ctx,
                            loop_instr_t** loop_instr);

parser_status_t flowcontrol_parser(FILE* input, const context_t* ctx,
                                   flowcontrol_t* flowcontrol);

parser_status_t affectation_parser(FILE* input, const context_t* ctx,
                                   affectation_instr_t* affectation_instr);

parser_status_t instruction_parser(FILE* input, const context_t* ctx,
                                   instruction_t** instruction);

parser_status_t instructions_parser(FILE* input, const context_t* ctx,
                                    vector_t* vector);

parser_status_t structure_parser(FILE* input,
                                 const context_t* ctx,
                                 structure_t** structure);

parser_status_t structure_member_parser(FILE* input,
                                        const void* unused_args,
                                        symbol_t** symbol);

parser_status_t variable_tail_parser(FILE* input, const context_t* ctx,
                                     symbol_t** symbol);

parser_status_t global_parser(FILE* input,
                              const context_t* ctx,
                              symbol_t** symbol);


parser_status_t constant_parser(FILE* input,
                                const context_t* ctx,
                                constant_t** constant);

parser_status_t local_parser(FILE* input,
                       const context_t* ctx,
                       symbol_t** symbol);

parser_status_t entity_parser(FILE* input,
                               context_t* ctx,
                               program_t* prg);

parser_status_t function_args_parser(FILE* input, const context_t* ctx,
                                     vector_t* vector);

parser_status_t function_parser(FILE* input, context_t* ctx,
                                function_t** function);

parser_status_t procedure_parser(FILE* input, context_t* ctx,
                                 function_t** function);

parser_status_t program_parser(FILE* input,
                               context_t* ctx,
                               program_t** program);

#endif
