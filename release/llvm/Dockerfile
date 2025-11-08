FROM ubuntu:24.04

RUN apt update
RUN apt install git build-essential clang cmake -y

RUN git clone https://github.com/marcelkottmann/armembeddon-codon-llvm-fork.git llvm-project

RUN cmake -S llvm-project/llvm -B llvm-project/build \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_INCLUDE_TESTS=OFF \
    -DLLVM_ENABLE_RTTI=ON \
    -DLLVM_ENABLE_ZLIB=OFF \
    -DLLVM_ENABLE_ZSTD=OFF \
    -DLLVM_ENABLE_PROJECTS="clang;openmp" \
    -DLLVM_TARGETS_TO_BUILD=all

RUN cmake --build llvm-project/build
RUN cmake --install llvm-project/build --prefix=llvm-project/install
