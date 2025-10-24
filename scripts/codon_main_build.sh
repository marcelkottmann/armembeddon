#!/bin/bash

### SETTINGS #############################

# Uncomment the following line to create a LLVM IR build from codon and then use 
# llc to create the object file.
#LLVM_BUILD_OBJ="build/codon_main.ll"
BUILD_OBJ="build/codon_main.o"
ARMEMBEDDON_CODON_FORK="$HOME/work/projects/codon"
##########################################

set -e

mkdir -p build
rm -f $BUILD_OBJ $LLVM_BUILD_OBJ


PATH=$ARMEMBEDDON_CODON_FORK/codon/install/bin:$ARMEMBEDDON_CODON_FORK/llvm-project/build/bin:$PATH

echo "-- Cordon build..."
# Build the LLVM IR from the Python source code
if [ -z ${LLVM_BUILD_OBJ+x} ]; then
    OUT_FILE=$BUILD_OBJ
else
    OUT_FILE=$LLVM_BUILD_OBJ
fi

codon build -release --disable-exceptions src/codon_main.py -o $OUT_FILE -float-abi=soft -mcpu=cortex-m4 -mattr=+vfpv4-sp-d16

if [ ! -z ${LLVM_BUILD_OBJ+x} ]; then

    #grep -v -E 'attributes #1|attributes #2' $BUILD_OBJ > build/main_patched.ll
    #cp build/main_patched.ll build/codon_main.ll

    echo "-- LLVM build..."
    # build the object file for ARM Cortex-M4
    llc -filetype=obj \
        -mtriple=thumbv7em-none-eabihf \
        -mcpu=cortex-m4 -mattr=+vfpv4-sp-d16 -float-abi=soft \
        $LLVM_BUILD_OBJ -o $BUILD_OBJ

fi

echo "-- LLVM objcopy redefine..."
llvm-objcopy --redefine-sym main=codon_main $BUILD_OBJ
