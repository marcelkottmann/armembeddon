#!/bin/bash

set -e

mkdir -p release/armembeddon/bin
mkdir -p release/armembeddon/lib

cp llvm-project/install/bin/llc release/armembeddon/bin/llc
cp llvm-project/install/bin/llvm-objcopy release/armembeddon/bin/llvm-objcopy

cp codon/install/bin/codon release/armembeddon/bin/codon
cp -r codon/install/lib/codon/ release/armembeddon/lib/

tar cvzf armembeddon.tar.gz -C release armembeddon

gh --version
gh api user