#!/bin/sh
# Reproduce round 003 from a clean checkout on Ubuntu 24.04.
#
#   gauntlet/rounds/architecture/round-003-repair/reproduce.sh [build-dir]
#
# Uses the same build as round 002; see that round's reproduce.sh for the
# dependency list and the two configuration traps (BUILD_PART requires
# BUILD_MATERIAL; OCCT needs libtbb-dev).

set -eu

BUILD_DIR=${1:-build-p0a}
SRC_DIR=$(cd "$(dirname "$0")/../../.." && pwd)

if [ ! -d "$BUILD_DIR" ]; then
    echo "No build at $BUILD_DIR. Run round-002-p0a/reproduce.sh first, or pass its build dir."
    exit 1
fi

cmake --build "$BUILD_DIR" --target Part_tests_run -j"$(nproc)"

echo
echo "== repair on hard geometry =="
echo "Read N1 first: if the negative control does not report resolved_WRONG=1,"
echo "every zero in this round is meaningless."
"$BUILD_DIR/tests/Part_tests_run" --gtest_filter='ReferenceRepairTest.*'

echo
echo "The decisive line is H4's: the matcher's top candidate, its score, and"
echo "its margin against the intended face. See verdict.md."
