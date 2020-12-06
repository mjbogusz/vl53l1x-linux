#!/bin/sh
mkdir -p $(dirname $0)/../build
cd $(dirname $0)/../build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cppcheck \
 --std=c++14 --language=c++ \
 --error-exitcode=1 --verbose \
 --project=compile_commands.json \
 --enable=all --suppress=unusedFunction --suppress=missingIncludeSystem --suppress=unmatchedSuppression
