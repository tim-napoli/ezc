cmake_minimum_required(VERSION 2.6)
project(ezc)

set(CMAKE_C_FLAGS "--std=gnu99 -Wall ${CMAKE_C_FLAGS}")

include_directories("${PROJECT_SOURCE_DIR}/include")

#add_executable(
#
#)
#target_link_libraries(
#)

add_library(rand-grey STATIC
            src/cparser/parser.c)
