#!/usr/bin/env sh

set -e
set -u

mkdir -p $(dirname $0)/../build
cd $(dirname $0)/../build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cppcheck \
 --std=c++20 --language=c++ \
 --error-exitcode=1 \
 --verbose \
 --project=compile_commands.json \
 --enable=all \
 --inline-suppr \
 --suppress=unusedFunction --suppress=missingIncludeSystem --suppress=unmatchedSuppression
