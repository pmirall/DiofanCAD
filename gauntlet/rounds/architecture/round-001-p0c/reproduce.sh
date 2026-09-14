#!/bin/sh
# Reproduce round 001 (P0-C) from a clean checkout on Ubuntu 24.04.
#
#   gauntlet/rounds/architecture/round-001-p0c/reproduce.sh [build-dir]
#
# Builds a console-only FreeCAD (no GUI, no workbenches) with the developer
# tests, then runs the P0-C tests and the overhead benchmark.
#
# Note: BUILD_GUI=OFF requires the SetupQt.cmake fix from this round (U-001).
# At the unpatched baseline commit, configure fails.

set -eu

BUILD_DIR=${1:-build-p0c}
SRC_DIR=$(cd "$(dirname "$0")/../../.." && pwd)

echo "== dependencies (Ubuntu 24.04) =="
echo "sudo apt-get install -y --no-install-recommends \\"
echo "  cmake ninja-build g++ qt6-base-dev qt6-tools-dev qt6-l10n-tools \\"
echo "  libboost-dev libboost-filesystem-dev libboost-program-options-dev \\"
echo "  libboost-regex-dev libboost-thread-dev libboost-graph-dev libboost-date-time-dev \\"
echo "  libxerces-c-dev zlib1g-dev python3-dev libfmt-dev libyaml-cpp-dev \\"
echo "  libgtest-dev libgmock-dev libeigen3-dev pybind11-dev libharfbuzz-dev libfreetype-dev \\"
echo "  libocct-foundation-dev libocct-modeling-algorithms-dev libocct-modeling-data-dev \\"
echo "  libocct-ocaf-dev libocct-visualization-dev libocct-data-exchange-dev"
echo

MODULES_OFF=""
for module in FEM TEMPLATE ADDONMGR BIM DRAFT HELP IMPORT INSPECTION JTREADER MATERIAL \
              MESH MESH_PART FLAT_MESH OPENSCAD PART PART_DESIGN CAM ASSEMBLY PLOT POINTS \
              REVERSEENGINEERING ROBOT SHOW SKETCHER SPREADSHEET START TEST MEASURE \
              TECHDRAW TUX WEB SURFACE; do
    MODULES_OFF="$MODULES_OFF -DBUILD_${module}=OFF"
done

echo "== configure =="
# shellcheck disable=SC2086
cmake -S "$SRC_DIR" -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_GUI=OFF \
    -DENABLE_DEVELOPER_TESTS=ON \
    $MODULES_OFF

echo "== build =="
cmake --build "$BUILD_DIR" --target App_tests_run -j"$(nproc)"

echo
echo "== full suite (compare against evidence/baseline-tests.txt) =="
"$BUILD_DIR/tests/App_tests_run" || true

echo
echo "== P0-C tests =="
"$BUILD_DIR/tests/App_tests_run" --gtest_filter='RecomputeTraceTest.*'

echo
echo "== overhead benchmark =="
echo "Absolute times are machine-specific. Only the within-process off/on"
echo "comparison and ns-per-object figure are portable."
"$BUILD_DIR/tests/App_tests_run" \
    --gtest_also_run_disabled_tests \
    --gtest_filter='RecomputeTraceTest.DISABLED_OverheadBenchmark'

echo
echo "To measure the DISABLED overhead you must also build with the"
echo "Document.cpp hooks reverted and compare: see benchmark.json."
