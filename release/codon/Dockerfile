FROM ghcr.io/marcelkottmann/armembeddon/tools-base-codon:0.0.1

RUN apt update && apt upgrade -y
RUN apt install git build-essential clang gfortran python3 python3-dev python3-venv python3-pip cmake -y

# RUN git clone https://github.com/marcelkottmann/armembeddon-codon-fork.git codon

ENV CODON_SYSTEM_LIBRARIES=/usr/lib/x86_64-linux-gnu

ARG VERSION

RUN cd codon && git pull --tags && git checkout ${VERSION}

RUN cd codon && mkdir -p build
RUN cd codon && cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_DIR=../llvm-project/install/lib/cmake/llvm \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++

RUN cd codon && cmake --build build --config Release
RUN cd codon && cmake --install build --prefix=install

COPY codon/pack.sh pack.sh
RUN chmod +x pack.sh && ./pack.sh
