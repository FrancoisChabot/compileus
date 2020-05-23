#!/bin/bash

BASEDIR=$(pwd)

if [ $# -eq 0 ]
then
  INSTALL_DIR=$1
else
  INSTALL_DIR=/usr/local
fi

SPDLOG_VERSION=v1.6.0

mkdir ${BASEDIR}/_extern_src

echo "--- Fetching spdlog ---"
git clone -b ${SPDLOG_VERSION} --single-branch  https://github.com/gabime/spdlog.git ${BASEDIR}/_extern_src/spdlog_src

cd ${BASEDIR}/_extern_src/spdlog_src
git submodule update --init --recursive

echo "--- Building spdlog ---"
mkdir ${BASEDIR}/_extern_src/spdlog_src/_bld
cd ${BASEDIR}/_extern_src/spdlog_src/_bld
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
make -j$(nproc)
make test

echo "--- Installing spdlog ---"
make install

echo "--- Done with spdlog ---"

rm -rf mkdir ${BASEDIR}/_extern_src