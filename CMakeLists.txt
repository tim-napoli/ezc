cmake_minimum_required(VERSION 2.6)
project(ezc)

set(CMAKE_C_FLAGS "--std=gnu99 -Wall ${CMAKE_C_FLAGS}")

include_directories("${PROJECT_SOURCE_DIR}/include")

#add_executable(
#
#)
#target_link_libraries(
#)

file(COPY samples DESTINATION .)

add_library(parser STATIC
            src/parser.c)

add_library(ez-parser STATIC
            src/parser.c
            src/ez-parser.c
            src/ez-parser-base.c
            src/ez-parser-value.c
            src/ez-parser-expr.c
            src/ez-parser-instr.c)

add_library(ez-lang STATIC
            src/ez-lang-value.c
            src/ez-lang-expr.c
            src/ez-lang-symbol.c)

add_library(vector STATIC
            src/vector.c)

add_executable(ezc src/ezc.c)
target_link_libraries(ezc ez-parser ez-lang vector)

# Tests
add_executable(test-parser test/parser.c)
target_link_libraries(test-parser parser)

add_executable(test-ez-parser test/ez-parser.c)
target_link_libraries(test-ez-parser ez-parser ez-lang vector)

add_executable(test-ez-expr test/ez-expr.c)
target_link_libraries(test-ez-expr ez-parser ez-lang vector)

add_executable(test-vector test/vector.c)
target_link_libraries(test-vector vector)
