#!/bin/bash
for f in ./exemples/*.c
do
    filename=$(basename -- "$f")
    filename="${filename%.*}"
    gcc -Wall -Werror -I./includes/ -o ./build/$filename ./src/*.c $f
done