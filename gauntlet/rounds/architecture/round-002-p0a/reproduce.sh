#!/bin/sh
# Reproduce round 002 (P0-A) from a clean checkout on Ubuntu 24.04.
#
#   gauntlet/rounds/architecture/round-002-p0a/reproduce.sh [build-dir]
#
# Builds Part (and Material, which Part requires) without the GUI, then runs
# the reference-resilience change challenge.
#
# Extra findings from round 002, both of which cost time to rediscover:
#   * BUILD_PART=ON requires BUILD_MATERIAL=ON; cmake fails with an explicit
#     message if you forget.
#   * OCCT needs libtbb-dev at link time. Without it ninja stops with
#     "libtbb.so, needed by Mod/Part/Part.so, missing".
#   * OCCT 7.6.3 from the Ubuntu archive is sufficient; FindOCC.cmake has a
#     pre-7.8 branch that selects the older toolkit names.

set -eu

BUILD_DIR=${1:-build-p0a}
SRC_DIR=$(cd "$(dirname "$0")/../../.." && pwd)

echo "== dependencies, in addition to those listed in round-001-p0c/reproduce.sh =="
echo "sudo apt-get install -y --no-install-recommends libtbb-dev"
echo

MODULES_OFF=""
for module in FEM TEMPLATE ADDONMGR BIM DRAFT HELP INSPECTION JTREADER MESH MESH_PART \
              FLAT_MESH OPENSCAD PART_DESIGN CAM ASSEMBLY PLOT POINTS REVERSEENGINEERING \
              ROBOT SHOW SKETCHER SPREADSHEET START TEST MEASURE TECHDRAW TUX WEB SURFACE; do
    MODULES_OFF="$MODULES_OFF -DBUILD_${module}=OFF"
done

echo "== configure =="
# shellcheck disable=SC2086
cmake -S "$SRC_DIR" -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_GUI=OFF \
    -DENABLE_DEVELOPER_TESTS=ON \
    -DBUILD_PART=ON \
    -DBUILD_MATERIAL=ON \
    -DBUILD_IMPORT=OFF \
    $MODULES_OFF

echo "== build (expect 30-60 min on 4 cores from cold) =="
cmake --build "$BUILD_DIR" --target Part_tests_run -j"$(nproc)"

echo
echo "== change challenge =="
"$BUILD_DIR/tests/Part_tests_run" --gtest_filter='ReferenceResilienceTest.*'
