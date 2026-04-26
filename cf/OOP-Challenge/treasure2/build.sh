#!/bin/bash
# Build the local treasure2 judge.
#
# Links:
#   runner.cpp              -- test driver (defines main)
#   treasure2lib_local.cpp  -- simulator (judge core)
#   treasure2.cpp           -- the submission file (per F.md "File bài làm phải tên là treasure2.cpp")
#
# DO NOT pass -DLOCAL or any define that solution.cpp branches on. The
# binary that gets submitted must be byte-identical to the local build,
# minus the simulator + runner.

set -euo pipefail

cd "$(dirname "$0")"

g++ -std=c++14 -Wall -Wextra -Wpedantic -O2 -g \
    runner.cpp \
    treasure2lib_local.cpp \
    treasure2.cpp \
    -o judge

echo "OK: built $(pwd)/judge"
