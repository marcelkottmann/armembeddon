#!/bin/bash

set -e

# check if .profile file exists and source it
[ -f $HOME/.profile ] && . $HOME/.profile

### SETTINGS #############################

# Uncomment the following line to create a LLVM IR build from codon and then use 
# llc to create the object file.
PROJECT_DIR=$1
FILE_BASE_NAME=$2
LLVM_BUILD_OBJ="build/${FILE_BASE_NAME}.ll"
BUILD_OBJ="build/${FILE_BASE_NAME}.o"

# check if "armembeddon-release" directory exists in project directory
if [ -d "$PROJECT_DIR/bin" ]; then
    ARMEMBEDDON_CODON_FORK="$PROJECT_DIR/bin"
    ARMEMBEDDON_CODON_LLVM_FORK="$PROJECT_DIR/bin"
else
    if [ -z ${ARMEMBEDDON_CODON_FORK+x} ]; then
        echo "Error: ARMEMBEDDON_CODON_FORK is not set and armembeddon-release directory not found." >&2
        exit 1
    fi
    if [ -z ${ARMEMBEDDON_CODON_LLVM_FORK+x} ]; then
        echo "Error: ARMEMBEDDON_CODON_LLVM_FORK is not set and armembeddon-release directory not found." >&2
        exit 1  
    fi
fi

##########################################

mkdir -p build
rm -f $BUILD_OBJ $LLVM_BUILD_OBJ

echo "-- Cordon build..."
# Build the LLVM IR from the Python source code
if [ -z ${LLVM_BUILD_OBJ+x} ]; then
    OUT_FILE=$BUILD_OBJ
else
    OUT_FILE=$LLVM_BUILD_OBJ
fi

${ARMEMBEDDON_CODON_FORK}/codon build -release --disable-exceptions src/${FILE_BASE_NAME}.py -o $OUT_FILE -float-abi=soft -mcpu=cortex-m4 -mattr=+vfpv4-sp-d16

if [ ! -z ${LLVM_BUILD_OBJ+x} ]; then
    echo "-- LLVM build..."
    ${ARMEMBEDDON_CODON_LLVM_FORK}/opt -strip-debug -S -O3 -disable-simplify-libcalls \
        -mtriple=thumbv7em-none-eabi \
        -mcpu=cortex-m4 -mattr=+vfpv4-sp-d16 -float-abi=soft \
        $LLVM_BUILD_OBJ -o $LLVM_BUILD_OBJ
        
    # build the object file for ARM Cortex-M4
    ${ARMEMBEDDON_CODON_LLVM_FORK}/llc -disable-simplify-libcalls -filetype=obj \
        -mtriple=thumbv7em-none-eabi \
        -mcpu=cortex-m4 -mattr=+vfpv4-sp-d16 -float-abi=soft \
        $LLVM_BUILD_OBJ -o $BUILD_OBJ

fi

echo "-- LLVM objcopy redefine..."
${ARMEMBEDDON_CODON_LLVM_FORK}/llvm-objcopy --redefine-sym main=codon_main $BUILD_OBJ
