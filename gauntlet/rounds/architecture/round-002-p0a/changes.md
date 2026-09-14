# Round 002 — Changes

## Divergence introduced

**None in upstream code.**

| File | Lines | Class | What |
|---|---|---|---|
| `tests/src/Mod/Part/App/ReferenceResilience.cpp` | +400 new | U3 | The change-challenge harness |
| `tests/src/Mod/Part/App/CMakeLists.txt` | +1 | U3 | Register it |

The whole round is a measurement, so it belongs in a test. No `src/` file was
touched, which is the correct shape for a proof that might conclude "build
nothing" — as this one did.

## Environment findings, recorded so they are not rediscovered

Three things cost time and are now in `reproduce.sh`:

1. `BUILD_PART=ON` requires `BUILD_MATERIAL=ON`. CMake says so explicitly.
2. OCCT needs `libtbb-dev` at link time, or ninja stops with
   `libtbb.so, needed by Mod/Part/Part.so, missing`.
3. **130 of 312 Part tests fail in a minimal build** because `Part::Box`
   construction throws `Material not found` — the material library's resources
   are only present after an install. This is an environment limitation, not a
   defect, but it means the document-object path is not testable in a
   build-tree-only setup. All 90 `TopoShapeExpansionTest` tests pass, which is
   why the harness works at TopoShape level.

Point 3 is worth upstream's attention eventually: a large fraction of the Part
suite is silently untestable without an install step, and nothing says so.
Not queued as an upstream item yet — it needs confirmation that an installed
build really does pass those 130, which this round did not do.

## What was deliberately not built

No persistent-reference mechanism, no semantic reference type, no repair layer.
The bar said Phase 2 only if Phase 1 found real failures. It found 8, all
recoverable in principle — but the verdict is that a repair layer, not a new
identity, is what those 8 justify, and that decision belongs to a round with
harder geometry.
