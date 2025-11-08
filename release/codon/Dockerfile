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

# install gh cli
RUN (type -p wget >/dev/null || (apt update && apt install wget -y)) \
	&& mkdir -p -m 755 /etc/apt/keyrings \
	&& out=$(mktemp) && wget -nv -O$out https://cli.github.com/packages/githubcli-archive-keyring.gpg \
	&& cat $out | tee /etc/apt/keyrings/githubcli-archive-keyring.gpg > /dev/null \
	&& chmod go+r /etc/apt/keyrings/githubcli-archive-keyring.gpg \
	&& mkdir -p -m 755 /etc/apt/sources.list.d \
	&& echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" | tee /etc/apt/sources.list.d/github-cli.list > /dev/null \
	&& apt update \
	&& apt install gh -y

COPY release/codon/pack.sh pack.sh
COPY scripts armembeddon/scripts
COPY include armembeddon/include
COPY src armembeddon/src

RUN --mount=type=secret,id=github_token chmod a+x ./pack.sh && GH_TOKEN=$(cat /run/secrets/github_token) VERSION=${VERSION} ./pack.sh
