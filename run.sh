#!/bin/bash

for i in $(dirname $(find . -type f -iname "Makefile"))
do
    make -C $i
    make test -C $i
done