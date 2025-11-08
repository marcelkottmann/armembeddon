FROM ubuntu:24.04

# #RUN git clone https://github.com/marcelkottmann/armembeddon-codon-fork.git codon
# COPY codon.tar.gz codon.tar.gz
# RUN tar -xvf codon.tar.gz

# #RUN git clone https://github.com/marcelkottmann/armembeddon-codon-llvm-fork.git llvm-project
# COPY llvm-project.tar.gz llvm-project.tar.gz
# RUN tar -xvf llvm-project.tar.gz

RUN apt update
RUN apt install build-essential python3 python3-dev python3-venv python3-pip cmake -y

# RUN cmake -S llvm-project/llvm -B llvm-project/build \
#     -DCMAKE_BUILD_TYPE=Release \
#     -DLLVM_INCLUDE_TESTS=OFF \
#     -DLLVM_ENABLE_RTTI=ON \
#     -DLLVM_ENABLE_ZLIB=OFF \
#     -DLLVM_ENABLE_ZSTD=OFF \
#     -DLLVM_ENABLE_PROJECTS="clang;openmp" \
#     -DLLVM_TARGETS_TO_BUILD=all

# RUN cmake --build llvm-project/build
