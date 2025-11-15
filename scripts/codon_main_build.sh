#!/bin/bash

### SETTINGS #############################

# Uncomment the following line to create a LLVM IR build from codon and then use 
# llc to create the object file.
FILE_BASE_NAME=$1
LLVM_BUILD_OBJ="build/${FILE_BASE_NAME}.ll"
BUILD_OBJ="build/${FILE_BASE_NAME}.o"
ARMEMBEDDON_CODON_FORK="$HOME/work/projects/codon"
##########################################

set -e

mkdir -p build
rm -f $BUILD_OBJ $LLVM_BUILD_OBJ

PATH=$ARMEMBEDDON_CODON_FORK/codon/install/bin:$PATH
PATH=$ARMEMBEDDON_CODON_FORK/llvm-project/install/bin:$PATH

echo "-- Cordon build..."
# Build the LLVM IR from the Python source code
if [ -z ${LLVM_BUILD_OBJ+x} ]; then
    OUT_FILE=$BUILD_OBJ
else
    OUT_FILE=$LLVM_BUILD_OBJ
fi

codon build -release --disable-exceptions src/${FILE_BASE_NAME}.py -o $OUT_FILE -float-abi=soft -mcpu=cortex-m4 -mattr=+vfpv4-sp-d16

if [ ! -z ${LLVM_BUILD_OBJ+x} ]; then
    echo "-- LLVM build..."
    opt -strip-debug -S -O3 -disable-simplify-libcalls \
        -mtriple=thumbv7em-none-eabi \
        -mcpu=cortex-m4 -mattr=+vfpv4-sp-d16 -float-abi=soft \
        $LLVM_BUILD_OBJ -o $LLVM_BUILD_OBJ
        
    # build the object file for ARM Cortex-M4
    llc -disable-simplify-libcalls -filetype=obj \
        -mtriple=thumbv7em-none-eabi \
        -mcpu=cortex-m4 -mattr=+vfpv4-sp-d16 -float-abi=soft \
        $LLVM_BUILD_OBJ -o $BUILD_OBJ

fi

echo "-- LLVM objcopy redefine..."
llvm-objcopy --redefine-sym main=codon_main $BUILD_OBJ
