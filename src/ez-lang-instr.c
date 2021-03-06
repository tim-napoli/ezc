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
    vector_wipe(&elsif->instructions, (delete_func_t)&instruction_delete);
    free(elsif);
}

void elsif_instr_print(FILE* output, const context_t* ctx,
                       const elsif_instr_t* elsif)
{
    fprintf(output, "else if (");
    expression_print(output, ctx, elsif->coundition);
    fprintf(output, ") {\n");

    instructions_print(output, ctx, &elsif->instructions);

    fprintf(output, "}\n");
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

    vector_wipe(&if_instr->instructions, (delete_func_t)&instruction_delete);
    vector_wipe(&if_instr->elsifs, (delete_func_t)&elsif_instr_delete);
    vector_wipe(&if_instr->else_instrs, (delete_func_t)&instruction_delete);
    free(if_instr);
}

void if_instr_print(FILE* output, const context_t* ctx,
                    const if_instr_t* if_instr)
{
    fprintf(output, "if (");
    expression_print(output, ctx, if_instr->coundition);
    fprintf(output, ") {\n");

    instructions_print(output, ctx, &if_instr->instructions);
    fprintf(output, "}\n");

    for (int i = 0; i < if_instr->elsifs.size; i++) {
        elsif_instr_print(output, ctx, if_instr->elsifs.elements[i]);
    }

    if (if_instr->else_instrs.size) {
        fprintf(output, "else {\n");
        instructions_print(output, ctx, &if_instr->else_instrs);
        fprintf(output, "}\n");
    }
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
    vector_wipe(&loop->instructions, (delete_func_t)&instruction_delete);
    free(loop);
}

void loop_instr_print(FILE* output, const context_t* ctx,
                      const loop_instr_t* loop_instr)
{
    fprintf(output, "do {\n");
    instructions_print(output, ctx, &loop_instr->instructions);
    fprintf(output, "} while (!(");
    expression_print(output, ctx, loop_instr->coundition);
    fprintf(output, "));\n");
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
    vector_wipe(&while_instr->instructions, (delete_func_t)&instruction_delete);
    free(while_instr);
}

void while_instr_print(FILE* output, const context_t* ctx,
                       const while_instr_t* while_instr)
{
    fprintf(output, "while (");
    expression_print(output, ctx, while_instr->coundition);
    fprintf(output, ") {\n");
    instructions_print(output, ctx, &while_instr->instructions);
    fprintf(output, "}\n");
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
    instruction_delete(on_instr->instruction);
    free(on_instr);
}

void on_instr_print(FILE* output, const context_t* ctx,
                    const on_instr_t* on_instr)
{
    fprintf(output, "if (");
    expression_print(output, ctx, on_instr->coundition);
    fprintf(output, ") {\n");
    instruction_print(output, ctx, on_instr->instruction);
    fprintf(output, "}\n");
}

for_instr_t* for_instr_new(const identifier_t* subject) {
    for_instr_t* instr = malloc(sizeof(for_instr_t));

    memcpy(&instr->subject, subject, sizeof(identifier_t));
    instr->range.from = NULL;
    instr->range.to   = NULL;
    vector_init(&instr->instructions, 0);

    return instr;
}

void range_set_from(range_t* range, expression_t* from) {
    range->from = from;
}

void range_set_to(range_t* range, expression_t* to) {
    range->to = to;
}

void for_instr_delete(for_instr_t* for_instr) {
    expression_delete(for_instr->range.from);
    expression_delete(for_instr->range.to);

    vector_wipe(&for_instr->instructions, (delete_func_t)&instruction_delete);
    free(for_instr);
}

void for_instr_print(FILE* output, const context_t* ctx,
                     const for_instr_t* for_instr)
{
    fprintf(output, "for (%s = ", for_instr->subject.value);
    expression_print(output, ctx, for_instr->range.from);
    fprintf(output, "; %s < ", for_instr->subject.value);
    expression_print(output, ctx, for_instr->range.to);
    fprintf(output, "; %s++) {\n", for_instr->subject.value);
    instructions_print(output, ctx, &for_instr->instructions);
    fprintf(output, "}\n");
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

void flowcontrol_print(FILE* output, const context_t* ctx,
                       const flowcontrol_t* fc_instr)
{
    switch (fc_instr->type) {
      case FLOWCONTROL_TYPE_IF:
        if_instr_print(output, ctx, fc_instr->if_instr);
        break;

      case FLOWCONTROL_TYPE_WHILE:
        while_instr_print(output, ctx, fc_instr->while_instr);
        break;

      case FLOWCONTROL_TYPE_LOOP:
        loop_instr_print(output, ctx, fc_instr->loop_instr);
        break;

      case FLOWCONTROL_TYPE_ON:
        on_instr_print(output, ctx, fc_instr->on_instr);
        break;

      case FLOWCONTROL_TYPE_FOR:
        for_instr_print(output, ctx, fc_instr->for_instr);
        break;

    }
}

void affectation_instr_wipe(affectation_instr_t* affectation) {
    valref_delete(affectation->lvalue);
    expression_delete(affectation->expression);
}

void affectation_instr_print(FILE* output, const context_t* ctx,
                             const affectation_instr_t* aff)
{
    valref_print(output, ctx, aff->lvalue);
    fprintf(output, " = ");
    expression_print(output, ctx, aff->expression);
    fprintf(output, ";\n");
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

void instruction_print(FILE* output, const context_t* ctx,
                       const instruction_t* instr)
{
    switch (instr->type) {
      case INSTRUCTION_TYPE_PRINT:
        fprintf(output, "std::cout << ");
        for (int i = 0; i < instr->parameters.parameters.size; i++) {
            expression_print(output, ctx,
                             instr->parameters.parameters.elements[i]);
            if (i + 1 < instr->parameters.parameters.size) {
                fprintf(output, " << ");
            }
        }
        fprintf(output, ";\n");
        break;

      case INSTRUCTION_TYPE_READ:
        fprintf(output, "std::cin >> ");
        valref_print(output, ctx,
                     instr->valref);
        fprintf(output, ";\n");
        break;

      case INSTRUCTION_TYPE_RETURN:
        fprintf(output, "return ");
        expression_print(output, ctx,
                         instr->expression);
        fprintf(output, ";\n");

      case INSTRUCTION_TYPE_FLOWCONTROL:
        flowcontrol_print(output, ctx, &instr->flowcontrol);
        break;

      case INSTRUCTION_TYPE_EXPRESSION:
        expression_print(output, ctx, instr->expression);
        fprintf(output, ";\n");
        break;

      case INSTRUCTION_TYPE_AFFECTATION:
        affectation_instr_print(output, ctx, &instr->affectation);
        break;
    }
}

void instructions_print(FILE* output, const context_t* ctx,
                        const vector_t* instrs)
{
    for (int i = 0; i < instrs->size; i++) {
        instruction_print(output, ctx, instrs->elements[i]);
    }
}
