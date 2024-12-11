#!/bin/bash

for i in $(dirname $(find . -type f -iname "Makefile"))
do
    passed=0
    total=0

    make -s -C $i > /dev/null 2>&1
    while read -r line; do
        if [[ $line == Test* ]]; then
            ((total++))
        fi

        if [[ $line == *0 ]]; then
            ((passed++))
        fi
    done < <(make test -s -C $i 2> /dev/null)

    if [ $passed -eq $total ]; then
        echo -e "$(realpath -s --relative-to=$(pwd) $i): \033[0;32mPASS\033[0m [$passed/$total]"
    else
        echo -e "$(realpath -s --relative-to=$(pwd) $i): \033[1;31mFAIL\033[0m [$passed/$total]"
    fi
done