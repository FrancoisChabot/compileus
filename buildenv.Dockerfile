FROM ubuntu:19.10

RUN apt-get update \
  && apt-get install -y \
      cmake \
      g++ \
      clang-format \
      clang-tidy \
      llvm-dev \
      gdb \
      git \
      libsqlite3-dev \
  && rm -rf /var/lib/apt/lists/*
 
COPY scripts/build_dependencies.sh tmp/
RUN tmp/build_dependencies.sh