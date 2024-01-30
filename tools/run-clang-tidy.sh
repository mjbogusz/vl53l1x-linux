#!/usr/bin/env sh

set -e
set -u

mkdir -p $(dirname $0)/../build
cd $(dirname $0)/../build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
run-clang-tidy -p=.
