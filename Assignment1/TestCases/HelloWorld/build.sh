#!/bin/sh


# Before using this script, please set the LLVM_BUILD_DIR environment variable.
# example: export LLVM_BUILD_DIR=/home/cpen/llvm-project/build

rm *.ll
$LLVM_BUILD_DIR/bin/clang++ -O0 -g -S -emit-llvm -o HelloWorld.ll -c HelloWorld.cpp
