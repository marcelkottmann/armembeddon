#!/bin/bash

set -e

mkdir -p release/armembeddon/bin
mkdir -p release/armembeddon/lib

# copy llvm binaries
cp llvm-project/install/bin/llc release/armembeddon/bin/llc
cp llvm-project/install/bin/llvm-objcopy release/armembeddon/bin/llvm-objcopy

# copy codon libs and binaries
cp codon/install/bin/codon release/armembeddon/bin/codon
cp -r codon/install/lib/codon/ release/armembeddon/lib/

# copy armembeddon files
cp -r armembeddon/* release/armembeddon/

tar cvzf armembeddon.tar.gz -C release armembeddon

gh release create "$RELEASE_VERSION" --repo marcelkottmann/armembeddon ./armembeddon.tar.gz
