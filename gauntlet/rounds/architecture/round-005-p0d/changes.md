# Round 005 — Changes

## Divergence introduced

**None in upstream code.**

| File | Lines | Class | What |
|---|---|---|---|
| `tests/src/Mod/Part/App/ReferencePersistence.cpp` | +330 new | U3 | Five measurements and a negative control |
| `tests/src/Mod/Part/App/CMakeLists.txt` | +1 | U3 | Register it |

## One API note

`ComplexGeoData::elementMap()` is protected. The public route to a shape's
element map is `resetElementMap()`, which detaches the map and returns it — so
the harness does that on a copy, leaving the caller's shape untouched. Worth
recording because the obvious accessor is not the available one.

## The diagnostic worth keeping

`roundTrip()` reports `hasher_entries_at_save`. Without it, M4 would have been
recorded as a pass. With it, M4 is recorded as not exercised.

Two rounds in a row have now produced a clean result that turned out to be a
measurement that never fired (round 003's ground truth, round 005's hasher).
Both were caught by asking whether the mechanism under test had actually been
invoked. That question should be routine in every future harness.
