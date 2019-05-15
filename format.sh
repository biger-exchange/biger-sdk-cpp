#!/bin/sh
find ./src -name "*.hpp" -o -name "*.cpp" -o -name "*.h"|xargs clang-format -i

