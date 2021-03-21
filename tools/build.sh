#!/bin/sh

set -e
set -u

cd $(dirname $0)/..
mkdir -p build
cd build
cmake ..
cmake --build . --parallel
