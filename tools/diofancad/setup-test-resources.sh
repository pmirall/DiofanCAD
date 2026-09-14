#!/bin/sh
# Make FreeCAD's developer tests runnable from a build tree.
#
#   tools/diofancad/setup-test-resources.sh <build-dir>
#
# The test binaries resolve data through App::Application::getHomePath(), which
# for a build-tree binary is the BUILD ROOT. Two directories are expected there
# and a plain `cmake --build` creates neither:
#
#   <build>/share/Mod/Material/Resources   the material library, or every test
#                                          that constructs a Part::Box throws
#                                          "Material not found"
#   <build>/tests/brepfiles                .brep fixtures for FuzzyBooleanTest
#   <build>/tests/TestModels               .FCStd fixtures for PartDesign's
#                                          BackwardCompatibilityTest, which
#                                          otherwise throws and takes the whole
#                                          PartDesign suite down with it
#
# Without them the Part suite reports 125 of 331 failures that look like code
# defects and are not. With them it is 331 of 331.
#
# Measured in round 006; see gauntlet/rounds/architecture/round-006-unblock/.

set -eu

BUILD_DIR=${1:-}
if [ -z "$BUILD_DIR" ] || [ ! -d "$BUILD_DIR" ]; then
    echo "usage: $0 <build-dir>" >&2
    exit 1
fi

SRC_DIR=$(cd "$(dirname "$0")/../.." && pwd)
BUILD_DIR=$(cd "$BUILD_DIR" && pwd)

mkdir -p "$BUILD_DIR/share/Mod/Material" "$BUILD_DIR/tests"
ln -sfn "$SRC_DIR/src/Mod/Material/Resources" "$BUILD_DIR/share/Mod/Material/Resources"
ln -sfn "$SRC_DIR/tests/src/Mod/Part/App/brepfiles" "$BUILD_DIR/tests/brepfiles"
ln -sfn "$SRC_DIR/tests/src/Mod/PartDesign/App/TestModels" "$BUILD_DIR/tests/TestModels"

echo "linked:"
echo "  $BUILD_DIR/share/Mod/Material/Resources"
echo "  $BUILD_DIR/tests/brepfiles"
echo "  $BUILD_DIR/tests/TestModels"
echo
echo "Now run e.g. $BUILD_DIR/tests/Part_tests_run"
