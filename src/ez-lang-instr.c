#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    free(if_instr);
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
    vector_wipe(&loop->instructions, NULL);
    free(loop);
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
    vector_wipe(&while_instr->instructions, NULL);
    free(while_instr);
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
    free(on_instr);
}


for_instr_t* for_instr_new(const identifier_t* subject) {
    for_instr_t* instr = malloc(sizeof(for_instr_t));

    memcpy(&instr->subject, subject, sizeof(identifier_t));
    instr->range.from = NULL;
    instr->range.to   = NULL;
    vector_init(&instr->instructions, 0);

    return instr;
}

void for_instr_delete(for_instr_t* for_instr) {
    expression_delete(for_instr->range.from);
    expression_delete(for_instr->range.to);

    /* TODO instruction_delete */
    vector_wipe(&for_instr->instructions, NULL);
    free(for_instr);
}

void flowcontrol_wipe(flowcontrol_t* fc) {
    switch (fc->type) {
      case FLOWCONTROL_TYPE_IF:
        if_instr_delete(fc->if_instr);
        break;

      case FLOWCONTROL_TYPE_WHILE:
        while_instr_delete(fc->while_instr);
        break;

      case FLOWCONTROL_TYPE_LOOP:
        loop_instr_delete(fc->loop_instr);
        break;

      case FLOWCONTROL_TYPE_ON:
        on_instr_delete(fc->on_instr);
        break;

      case FLOWCONTROL_TYPE_FOR:
        for_instr_delete(fc->for_instr);
        break;

    }
}

void affectation_instr_wipe(affectation_instr_t* affectation) {
    valref_delete(affectation->lvalue);
    expression_delete(affectation->expression);
}

instruction_t* instruction_new(instruction_type_t type) {
    instruction_t* instr = malloc(sizeof(instruction_t));

    memset(instr, 0, sizeof(instruction_t));
    instr->type = type;

    return instr;
}

void instruction_delete(instruction_t* instr) {
    switch (instr->type) {
      case INSTRUCTION_TYPE_PRINT:
        parameters_wipe(&instr->parameters);
        break;

      case INSTRUCTION_TYPE_READ:
        valref_delete(instr->valref);
        break;

      case INSTRUCTION_TYPE_RETURN:
      case INSTRUCTION_TYPE_EXPRESSION:
        expression_delete(instr->expression);
        break;

      case INSTRUCTION_TYPE_FLOWCONTROL:
        flowcontrol_wipe(&instr->flowcontrol);
        break;

      case INSTRUCTION_TYPE_AFFECTATION:
        affectation_instr_wipe(&instr->affectation);
        break;
    }

    free(instr);
}

