# Round 001 — Changes

## Divergence introduced

This round makes the first deliberate changes to upstream code. Recorded here because `docs/upstream-sync.md` budgets divergence rather than discovering it later.

| File | Lines | Class | What |
|---|---|---|---|
| `cMake/FreeCAD_Helpers/SetupQt.cmake` | +5 / −2 | **U0** | Fix: `LinguistTools` was only requested under `BUILD_GUI`, but `src/App` calls `qt_add_translation()` unconditionally, so `BUILD_GUI=OFF` could not configure at all |
| `src/App/RecomputeTrace.h` | +125 new | U2 | Trace API |
| `src/App/RecomputeTrace.cpp` | +326 new | U2 | Trace implementation and JSON writer |
| `src/App/Document.cpp` | +36 / −1 | U2 | Six hooks in `Document::recompute`, each guarded by an inline enabled check |
| `src/App/CMakeLists.txt` | +2 | U2 | Register the new sources |
| `tests/src/App/RecomputeTrace.cpp` | +235 new | U2 | Five tests plus a disabled overhead benchmark |
| `tests/src/App/CMakeLists.txt` | +1 | U2 | Register the test |

Total: 4 upstream files modified, 3 new files.

## The U0 by-product

The `BUILD_GUI=OFF` breakage is a genuine upstream bug, not a local workaround, and it is independent of everything else in this round:

- `src/App/CMakeLists.txt:370` calls `qt_find_and_add_translation()` outside any `if(BUILD_GUI)` block;
- `SetupQt.cmake` appended `LinguistTools` to the Qt component list only inside `if(BUILD_GUI)`;
- no CI workflow builds with `BUILD_GUI=OFF`, which is why it survived.

Reproduction: configure with `-DBUILD_GUI=OFF` at the baseline commit and the run fails with `Unknown CMake command "qt_add_translation"`.

It is queued as `U-001` in `docs/upstream-backlog.md` and is extractable as a standalone PR that has nothing to do with DiofanCAD.

## Hook design

Every hook follows the same shape so that a disabled trace costs one predictable branch and nothing else:

```cpp
const bool traced = RecomputeTrace::isEnabled();   // once per run
...
if (traced) { RecomputeTrace::recordPropagation(*obj, *inObjIt, nullptr); }
```

`isEnabled()` is inline in the header, so a disabled build path does not even make a call. Timestamps are only taken when tracing, so the clock cost is not paid by default either.

No existing statement was removed or reordered. The single deleted line is `int res = _recomputeFeature(obj);`, split into a traced and an untraced branch that call it identically.

## What was deliberately not done

- `Document::recomputeFeature()` (single-object path) — out of scope for this round's bar.
- Python bindings — DEFER, per `input.md`.
- Any change to propagation, ordering, or error handling. The proof observes; it does not steer.
