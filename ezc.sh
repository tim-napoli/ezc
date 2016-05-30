#!/bin/bash

help() {
    echo "ezc.sh [ez source file] [cpp source file] [executable]"
    exit 1
}

check_ez_source_exist() {
    if [ ! -f $1 ]; then
        echo "EZ source file $1 doesn't exist"
        exit 1
    fi
}

check_have_ezc() {
    if [ ! -f "ezc" ]; then
        echo "Couldn't found ez compiler"
        exit 1
    fi
}

if [ $# -lt 2 ]; then
    help
fi

ez_source=$1
cpp_source=$2
exe=$3

check_ez_source_exist $ez_source
check_have_ezc

./ezc $ez_source > $cpp_source
if [ ! $? -eq 0 ]; then
    echo "Compilation failure"
    exit 1
fi

g++ -Wall -std=c++11 $cpp_source -o $exe

