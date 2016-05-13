#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "ez-parser.h"

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

    TEST_ON(valid_id);
    assert(identifier_parser(f, NULL, NULL) == PARSER_SUCCESS);
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

    TEST_ON(invalid_type);
    assert(type_parser(f, NULL, NULL) == PARSER_FAILURE);
    END_TEST;

    TEST_ON(type_string);
    assert(type_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(type_integer);
    assert(type_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(type_real);
    assert(type_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(type_vector_simple);
    assert(type_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(type_vector_recursive);
    assert(type_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;
}

void value_test() {
    FILE* f;

    char string_simple[] = "\"xyz lnoehfeh\"";
    char string_quoted[] = "\"cdzeuiz \\\" dedsfefz\"";
    char integer[] = "1245863";
    char bool_true[] = "true";
    char bool_false[] = "false";
    char varref_simple[] = "xyz";
    char varref_rec[] = "a.b.c";
    char funccall_simple[] = "x()";
    char funccall_rec[] = "x.y.z()";

    TEST_ON(string_simple);
    assert(value_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(string_quoted);
    assert(value_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(integer);
    assert(value_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(bool_true);
    assert(value_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(bool_false);
    assert(value_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(varref_simple);
    assert(value_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(varref_rec);
    assert(value_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(funccall_simple);
    assert(value_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;

    TEST_ON(funccall_rec);
    assert(value_parser(f, NULL, NULL) == PARSER_SUCCESS);
    END_TEST;
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
    assert(return_parser(f, NULL, NULL) == PARSER_FAILURE);
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
    assert(on_parser(f, NULL, NULL) == PARSER_FAILURE);
    END_TEST;

    TEST_ON(invalid_on_2);
    assert(on_parser(f, NULL, NULL) == PARSER_FAILURE);
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

int main(int argc, char** argv) {

    comment_test();
    identifier_test();
    type_test();
    value_test();
    expr_test();
    print_test();
    return_test();
    on_test();
    if_test();

    return 0;
}

