# Round 007 — Changes

## Divergence introduced

**None in upstream code.**

| File | Lines | Class | What |
|---|---|---|---|
| `tests/src/Mod/Part/App/DocumentChangeChallenge.cpp` | +390 new | U3 | Document-level change challenge, save/reload, and the element-map probe |
| `tests/src/Mod/Part/App/CMakeLists.txt` | +1 | U3 | Register it |

## Depends on round 006

This round could not have run a week of rounds ago: every test here constructs
a `Part::Box`, which threw `Material not found` until round 006 supplied the
two symlinks. `tools/diofancad/setup-test-resources.sh <build-dir>` is a
prerequisite.

## The probe worth keeping

`P1_WhichFeaturesCarryElementMaps` prints `hasElementMap()` and
`getElementMapSize()` for each feature type. It is three lines of output and it
reframed four rounds of work. Any future measurement of the identity layer
should print it first, so that the measurement's relevance is established
before its results are interpreted.
