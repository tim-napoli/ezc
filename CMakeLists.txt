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

add_library(cparser STATIC
            src/parser.c)

add_executable(ezc src/ezc.c src/parser.c src/ez-parser.c)

# Tests
add_executable(test-parser test/parser.c)
target_link_libraries(test-parser cparser)
