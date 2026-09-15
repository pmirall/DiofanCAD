#!/bin/sh
# DiofanCAD — one-command setup on a fresh Ubuntu 24.04 machine.
#
#   tools/diofancad/bootstrap.sh [build-dir] [--with-sketcher]
#
# Installs dependencies, configures a console-only build (no GUI), builds the
# test targets, creates the resource layout the tests need, and verifies the
# result. Everything this repository learned about building FreeCAD in rounds
# 001-010 is in this one file.
#
# Expect 45-90 minutes on 4 cores the first time. Re-runs are incremental.

set -eu

BUILD_DIR=${1:-build}
WITH_SKETCHER=no
for arg in "$@"; do
    [ "$arg" = "--with-sketcher" ] && WITH_SKETCHER=yes
done
SRC_DIR=$(cd "$(dirname "$0")/../.." && pwd)

say() { printf '\n== %s ==\n' "$1"; }

# --- dependencies --------------------------------------------------------
# This exact list built rounds 001-010. If cmake later names something else,
# add it here rather than installing it by hand, so the next machine works.
DEPS="cmake ninja-build g++ \
      qt6-base-dev qt6-tools-dev qt6-l10n-tools qt6-base-dev-tools \
      libboost-dev libboost-filesystem-dev libboost-program-options-dev \
      libboost-regex-dev libboost-thread-dev libboost-graph-dev libboost-date-time-dev \
      libxerces-c-dev zlib1g-dev python3-dev libfmt-dev libyaml-cpp-dev \
      libgtest-dev libgmock-dev libeigen3-dev pybind11-dev \
      libharfbuzz-dev libfreetype-dev libtbb-dev \
      libocct-foundation-dev libocct-modeling-algorithms-dev libocct-modeling-data-dev \
      libocct-ocaf-dev libocct-visualization-dev libocct-data-exchange-dev"
[ "$WITH_SKETCHER" = yes ] && DEPS="$DEPS swig"

say "dependencies"
if [ "$(id -u)" = "0" ]; then
    # shellcheck disable=SC2086
    apt-get install -y --no-install-recommends $DEPS
else
    echo "Run this once, then re-run the script:"
    echo
    # shellcheck disable=SC2086
    echo "  sudo apt-get install -y --no-install-recommends" $DEPS
    echo
    command -v cmake >/dev/null || exit 1
    echo "cmake found, assuming dependencies are installed; continuing."
fi

# --- configure -----------------------------------------------------------
# Traps this encodes, each paid for once:
#   BUILD_GUI=OFF needs the SetupQt.cmake fix (U-001), already in this repo.
#   BUILD_PART=ON requires BUILD_MATERIAL=ON - cmake says so explicitly.
#   OCCT needs libtbb-dev at LINK time, long after configure succeeds.
#   BUILD_SKETCHER=ON needs swig.
MODULES_OFF=""
for m in FEM TEMPLATE ADDONMGR BIM DRAFT HELP INSPECTION JTREADER MESH MESH_PART \
         FLAT_MESH OPENSCAD CAM ASSEMBLY PLOT POINTS REVERSEENGINEERING ROBOT SHOW \
         SPREADSHEET START TEST MEASURE TECHDRAW TUX WEB SURFACE IMPORT; do
    MODULES_OFF="$MODULES_OFF -DBUILD_${m}=OFF"
done
if [ "$WITH_SKETCHER" = yes ]; then
    EXTRA="-DBUILD_SKETCHER=ON -DBUILD_PART_DESIGN=ON"
    TARGET=PartDesign_tests_run
else
    EXTRA="-DBUILD_SKETCHER=OFF -DBUILD_PART_DESIGN=OFF"
    TARGET=Part_tests_run
fi

say "configure ($TARGET)"
# shellcheck disable=SC2086
cmake -S "$SRC_DIR" -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_GUI=OFF \
    -DENABLE_DEVELOPER_TESTS=ON \
    -DBUILD_PART=ON \
    -DBUILD_MATERIAL=ON \
    $EXTRA $MODULES_OFF

say "build (45-90 min on 4 cores from cold)"
cmake --build "$BUILD_DIR" --target "$TARGET" -j"$(nproc)"

# --- resources -----------------------------------------------------------
say "test resources"
"$SRC_DIR/tools/diofancad/setup-test-resources.sh" "$BUILD_DIR"

# --- verify --------------------------------------------------------------
say "verify"
echo "Part suite (expect 331/331 passing):"
"$BUILD_DIR/tests/Part_tests_run" 2>/dev/null | tail -3 || true
echo
echo "App suite (expect 3 pre-existing failures, unexplained since round 001):"
"$BUILD_DIR/tests/App_tests_run" 2>/dev/null | tail -3 || true

cat <<EOF

Done. Next:
  $BUILD_DIR/tests/Part_tests_run --gtest_filter='ReferencePopulationTest.*'
  cat HANDOFF.md
EOF
