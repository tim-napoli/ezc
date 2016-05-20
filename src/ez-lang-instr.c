#include <stdio.h>
#include <stdlib.h>
#include "ez-lang.h"

elsif_instr_t* elsif_instr_new(expression_t* coundition) {
    elsif_instr_t* elsif = malloc(sizeof(elsif_instr_t));
    if (!elsif) {
        fprintf(stderr, "couldn't allocate elsif");
        return NULL;
    }

    elsif->coundition = coundition;
    vector_init(&elsif->instructions, 0);

    return elsif;
}

void elsif_instr_delete(elsif_instr_t* elsif) {
    expression_delete(elsif->coundition);
    /* TODO instruction_delete */
    vector_wipe(&elsif->instructions, NULL);
    free(elsif);
}

if_instr_t* if_instr_new(expression_t* coundition) {
    if_instr_t* if_instr = malloc(sizeof(if_instr_t));
    if (!if_instr) {
        fprintf(stderr, "couldn't allocate if instruction\n");
        return NULL;
    }

    if_instr->coundition = coundition;
    vector_init(&if_instr->instructions, 0);
    vector_init(&if_instr->elsifs, 0);
    vector_init(&if_instr->else_instrs, 0);

    return if_instr;
}

void if_instr_delete(if_instr_t* if_instr) {
    expression_delete(if_instr->coundition);

    /* TODO instruction_delete */
    vector_wipe(&if_instr->instructions, NULL);
    vector_wipe(&if_instr->elsifs, (delete_func_t)&elsif_instr_delete);
    /* TODO instruction_delete */
    vector_wipe(&if_instr->else_instrs, NULL);
}

loop_instr_t* loop_instr_new(expression_t* coundition) {
    loop_instr_t* loop = malloc(sizeof(loop_instr_t));
    if (!loop) {
        fprintf(stderr, "couldn't allocate loop instruction\n");
        return NULL;
    }

    loop->coundition = coundition;
    vector_init(&loop->instructions, 0);

    return loop;
}

void loop_instr_delete(loop_instr_t* loop) {
    expression_delete(loop->coundition);
    /* TODO instruction_delete */
    vector_delete(&loop->instructions, NULL);
}

while_instr_t* while_instr_new(expression_t* coundition) {
    while_instr_t* while_instr = malloc(sizeof(while_instr_t));
    if (!while_instr) {
        fprintf(stderr, "couldn't allocate while instruction\n");
        return NULL;
    }

    while_instr->coundition = coundition;
    vector_init(&while_instr->instructions, 0);

    return while_instr;
}

void while_instr_delete(while_instr_t* while_instr) {
    expression_delete(while_instr->coundition);
    /* TODO instruction_delete */
    vector_delete(&while_instr->instructions, NULL);
}

on_instr_t* on_instr_new(expression_t* coundition) {
    on_instr_t* on_instr = malloc(sizeof(on_instr_t));
    if (!on_instr) {
        fprintf(stderr, "couldn't allocate on instruction\n");
        return NULL;
    }

    on_instr->coundition = coundition;
    on_instr->instruction = NULL;

    return on_instr;
}

void on_instr_delete(on_instr_t* on_instr) {
    expression_delete(on_instr->coundition);
    /* TODO instruction_delete */
}

