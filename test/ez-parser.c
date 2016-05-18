#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "ez-parser.h"
#include "ez-lang.h"

#define TEST_ON(_name) \
    f = fmemopen(_name, sizeof(_name), "r")

#define END_TEST    fclose(f)

void comment_test() {
    FILE* f;

    char inline_comment[] = "// zuqgyufzgffzeufhzepfhfoeh\n";
    char multiline_comment[] = "/* qdozhod/*\nqzfq\nqzffq\n*/";

    TEST_ON(inline_comment);
    assert(comment_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(multiline_comment);
    assert(comment_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;
}

void identifier_test() {
    FILE* f;

    char valid_id[] = "blectre01zzfafe";
    char invalid_id1[] = "[]";
    char invalid_id2[] = "0fefze";

    identifier_t id;

    TEST_ON(valid_id);
    assert(identifier_parser(f, NULL, &id) == PARSER_SUCCESS);
    assert(strcmp(id.value, "blectre01zzfafe") == 0);
    END_TEST;

    TEST_ON(invalid_id1);
    assert(identifier_parser(f, NULL, NULL) == PARSER_FAILURE);
    END_TEST;

    TEST_ON(invalid_id2);
    assert(identifier_parser(f, NULL, NULL) == PARSER_FAILURE);
    END_TEST;
}

void type_test() {
    FILE* f;

    char invalid_type[] = "blectre01zzfafe";
    char type_string[] = "string";
    char type_integer[] = "integer";
    char type_real[] = "real";
    char type_vector_simple[] = "vector of string";
    char type_vector_recursive[] = "vector of vector of string";

    type_t *type;

    TEST_ON(invalid_type);
    assert(type_parser(f, NULL, NULL) == PARSER_FAILURE);
    END_TEST;

    TEST_ON(type_string);
    assert(type_parser(f, NULL, &type) == PARSER_SUCCESS);
    assert(type->type == TYPE_TYPE_STRING);
    type_delete(type);
    END_TEST;

    TEST_ON(type_integer);
    assert(type_parser(f, NULL, &type) == PARSER_SUCCESS);
    assert(type->type == TYPE_TYPE_INTEGER);
    type_delete(type);
    END_TEST;

    TEST_ON(type_real);
    assert(type_parser(f, NULL, &type) == PARSER_SUCCESS);
    assert(type->type == TYPE_TYPE_REAL);
    type_delete(type);
    END_TEST;

    TEST_ON(type_vector_simple);
    assert(type_parser(f, NULL, &type) == PARSER_SUCCESS);
    assert(type->type == TYPE_TYPE_VECTOR);
    assert(type->vector_type->type == TYPE_TYPE_STRING);
    type_delete(type);
    END_TEST;

    TEST_ON(type_vector_recursive);
    assert(type_parser(f, NULL, &type) == PARSER_SUCCESS);
    assert(type->type == TYPE_TYPE_VECTOR);
    assert(type->vector_type->type == TYPE_TYPE_VECTOR);
    assert(type->vector_type->vector_type->type == TYPE_TYPE_STRING);
    type_delete(type);
    END_TEST;
}

void declaration_test() {
    FILE *f;

    char integer[] = "n is integer";
    char vector_of_real[] = "toto is vector of real";
    char vector_of_vector_of_boolean[] = "_ze23 is vector of vector of boolean";

    // TODO : local or global ...

    symbol_t *s;

    TEST_ON(integer);
    assert(variable_tail_parser(f, NULL, &s) == PARSER_SUCCESS);
    assert(strcmp(s->identifier.value, "n") == 0);
    assert(s->is->type == TYPE_TYPE_INTEGER);
    symbol_delete(s);
    END_TEST;

    TEST_ON(vector_of_real);
    assert(variable_tail_parser(f, NULL, &s) == PARSER_SUCCESS);
    assert(strcmp(s->identifier.value, "toto") == 0);
    assert(s->is->type == TYPE_TYPE_VECTOR);
    assert(s->is->vector_type->type == TYPE_TYPE_REAL);
    symbol_delete(s);
    END_TEST;

    TEST_ON(vector_of_vector_of_boolean);
    assert(variable_tail_parser(f, NULL, &s) == PARSER_SUCCESS);
    assert(strcmp(s->identifier.value, "_ze23") == 0);
    assert(s->is->type == TYPE_TYPE_VECTOR);
    assert(s->is->vector_type->type == TYPE_TYPE_VECTOR);
    assert(s->is->vector_type->vector_type->type == TYPE_TYPE_BOOLEAN);
    symbol_delete(s);
    END_TEST;
}

void value_test() {
    FILE* f;

    char string_simple[] = "\"xyz lnoehfeh\"";
    char string_quoted[] = "\"cdzeuiz \\\" dedsfefz\"";
    char integer[] = "1245863";
    char neg_integer[] = "-123456";
    char real[] = "-12456.32000";
    char bool_true[] = "true";
    char bool_false[] = "false";
    char varref_simple[] = "xyz";
    char varref_rec[] = "a.b.c";
    char funccall_simple[] = "x()";
    char funccall_rec[] = "v.at(3).at(5).z";
    char array_rec[] = "x[0][1 + 2 * 3][5].z";

    value_t v;

    TEST_ON(string_simple);
    assert(value_parser(f, NULL, &v) == PARSER_SUCCESS);
    assert(strcmp(v.string, "xyz lnoehfeh") == 0);
    value_wipe(&v);
    END_TEST;

    TEST_ON(string_quoted);
    assert(value_parser(f, NULL, &v) == PARSER_SUCCESS);
    assert(strcmp(v.string, "cdzeuiz \\\" dedsfefz") == 0);
    value_wipe(&v);
    END_TEST;

    TEST_ON(integer);
    assert(value_parser(f, NULL, &v) == PARSER_SUCCESS);
    assert(v.integer == 1245863);
    END_TEST;

    TEST_ON(neg_integer);
    assert(value_parser(f, NULL, &v) == PARSER_SUCCESS);
    assert(v.integer == -123456);
    END_TEST;

#if 0
    TEST_ON(real);
    assert(value_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;
#endif

    TEST_ON(bool_true);
    assert(value_parser(f, NULL, &v) == PARSER_SUCCESS);
    assert(v.boolean == true);
    END_TEST;

    TEST_ON(bool_false);
    assert(value_parser(f, NULL, &v) == PARSER_SUCCESS);
    assert(v.boolean == false);
    END_TEST;

    TEST_ON(varref_simple);
    assert(value_parser(f, NULL, &v) == PARSER_SUCCESS);
    assert(strcmp(v.valref->identifier.value, "xyz") == 0);
    assert(v.valref->next == NULL);
    value_wipe(&v);
    END_TEST;

    TEST_ON(varref_rec);
    assert(value_parser(f, NULL, &v) == PARSER_SUCCESS);
    assert(strcmp(v.valref->identifier.value, "a") == 0);
    assert(strcmp(v.valref->next->identifier.value, "b") == 0);
    assert(strcmp(v.valref->next->next->identifier.value, "c") == 0);
    value_wipe(&v);
    END_TEST;

    TEST_ON(funccall_simple);
    assert(value_parser(f, NULL, &v) == PARSER_SUCCESS);
    assert(strcmp(v.valref->identifier.value, "x") == 0);
    assert(v.valref->is_funccall);
    value_wipe(&v);
    END_TEST;

    TEST_ON(funccall_rec);
    assert(value_parser(f, NULL, &v) == PARSER_SUCCESS);
    assert(strcmp(v.valref->identifier.value, "v") == 0);
    assert(strcmp(v.valref->next->identifier.value, "at") == 0);
    assert(v.valref->next->is_funccall);
    assert(strcmp(v.valref->next->next->identifier.value, "at") == 0);
    assert(v.valref->next->next->is_funccall);
    assert(strcmp(v.valref->next->next->next->identifier.value, "z") == 0);
    value_wipe(&v);
    END_TEST;

#if 0
    TEST_ON(array_rec);
    assert(value_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;
#endif
}

void expr_test() {
    FILE* f;

    char simple_expr[] = "5";
    char enclosed_expr[] = "(5)";
    char operation_expr[] = "5 + 2";
    char complexe_expr[] = "1 / 2 + (f() * g()) + 3 / (1 + x.y.z)";

    TEST_ON(simple_expr);
    assert(expression_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(enclosed_expr);
    assert(expression_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(operation_expr);
    assert(expression_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(complexe_expr);
    assert(expression_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    char unclosed_expr[] = "1 + 2 * (3 + 4";
    char unfinished_expr[] = "1 +";

    TEST_ON(unclosed_expr);
    assert(expression_parser(f, NULL, NULL) == PARSER_FAILURE);
    END_TEST;

    TEST_ON(unfinished_expr);
    assert(expression_parser(f, NULL, NULL) == PARSER_FAILURE);
    END_TEST;
}

void print_test() {
    FILE* f;
    char valid_print[] = "print \"Hello mister \", person.name\n";
    char invalid_print[] = "print \"Hello mister \" person.name\n";

    TEST_ON(valid_print);
    assert(print_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(invalid_print);
    assert(print_parser(f, NULL, NULL) == PARSER_FAILURE);
    END_TEST;
}

void return_test() {
    FILE* f;
    char valid_return[] = "return 32 * (2 / 5 + x.y.f(5 * 8))\n";
    char invalid_return[] = "return \n";

    TEST_ON(valid_return);
    assert(return_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(invalid_return);
    assert(return_parser(f, NULL, NULL) == PARSER_FATAL);
    END_TEST;
}

void on_test() {
    FILE* f;
    char valid_on[] = "on x + 1 == 2 do print \"x = \", x\n";
    char invalid_on_1[] = "on do print \"hello\"\n";
    char invalid_on_2[] = "on true print \"hello\"\n";

    TEST_ON(valid_on);
    assert(on_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(invalid_on_1);
    assert(on_parser(f, NULL, NULL) == PARSER_FATAL);
    END_TEST;

    TEST_ON(invalid_on_2);
    assert(on_parser(f, NULL, NULL) == PARSER_FATAL);
    END_TEST;
}

void if_test() {
    FILE* f;

    char simple_if[] = "if x + 1 == 2 then\n"
                       "    print \"Hello\"\n"
                       "endif\n";
    char complexe_if[] = "if x + 1 == 2 then\n"
                         "    print \"Hello\"\n"
                         "    if true then\n"
                         "        print \"ok\"\n"
                         "    endif\n"
                         "elsif true then\n"
                         "    print \"elsif\"\n"
                         "    if true then\n"
                         "        print \"ok\"\n"
                         "    endif\n"
                         "else\n"
                         "    print \"else\"\n"
                         "    if true then\n"
                         "        print \"ok\"\n"
                         "    endif\n"
                         "endif\n";
    TEST_ON(simple_if);
    assert(if_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;
    TEST_ON(complexe_if);
    assert(if_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;
}

void while_test() {
    FILE* f;

    char simple_while[] = "while true do\n"
                          "    print \"true\"\n"
                          "endwhile\n";

    TEST_ON(simple_while);
    assert(while_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;
}

void for_test() {
    FILE* f;

    char simple_for[] = "for x in 1..32 do\n"
                        "    print \"true\"\n"
                        "endfor\n";

    TEST_ON(simple_for);
    assert(for_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;
}

void loop_test() {
    FILE* f;

    char simple_loop[] = "loop\n"
                         "    print \"true\"\n"
                         "until true\n";

    TEST_ON(simple_loop);
    assert(loop_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;
}

void affectation_test() {
    FILE* f;
    char valid_1[] = "x = (32 + 6) / 2\n";
    char valid_2[] = "x.y.z = (32 + 6) / 2\n";
    char invalid[] = "x.y.z() = (32 + 6) / 2\n";

    TEST_ON(valid_1);
    assert(affectation_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(valid_2);
    assert(affectation_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

#if 0
    TEST_ON(invalid);
    assert(affectation_parser(f, NULL, NULL) == PARSER_FAILURE);
    END_TEST;
#endif
}

void structure_test() {
    FILE* f;

    char valid_1[] = "structure Vector2d is\n"
                     "    x is real\n"
                     "    \n"
                     "    y is real\n"
                     "   \n"
                     "end\n";
    TEST_ON(valid_1);
    assert(structure_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;
}

int main(int argc, char** argv) {

    //comment_test();
    identifier_test();
    type_test();
    declaration_test();
    value_test();
    //expr_test();
    //print_test();
    //return_test();
    //on_test();
    //if_test();
    //affectation_test();
    //while_test();
    //for_test();
    //loop_test();
    //structure_test();

    return 0;
}
