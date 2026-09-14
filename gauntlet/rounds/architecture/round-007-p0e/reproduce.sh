#!/bin/sh
# Reproduce round 007 from a clean checkout on Ubuntu 24.04.
#
#   gauntlet/rounds/architecture/round-007-p0e/reproduce.sh [build-dir]
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
echo "== document-level change challenge =="
echo "PREREQUISITE: tools/diofancad/setup-test-resources.sh \$BUILD_DIR: if the negative control does not report resolved_WRONG=1,"
echo "every zero in this round is meaningless."
"$BUILD_DIR/tests/Part_tests_run" --gtest_filter='DocumentChangeChallengeTest.*'

echo
echo "The decisive lines are P1 (does any feature carry an element map) and"
echo "M1 NOT EXERCISED. See verdict.md."
