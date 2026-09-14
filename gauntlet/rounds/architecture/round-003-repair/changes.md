# Round 003 — Changes

## Divergence introduced

**None in upstream code.**

| File | Lines | Class | What |
|---|---|---|---|
| `tests/src/Mod/Part/App/ReferenceRepair.cpp` | +520 new | U3 | Matcher prototype, scenario-blind, plus six scenarios and a negative control |
| `tests/src/Mod/Part/App/CMakeLists.txt` | +1 | U3 | Register it |

The repair matcher is deliberately a **test-file prototype**, not a `src/`
component. The round's job was to decide whether it should exist at all, and
the answer turned out to be "only in a constrained form". Building it into the
product first would have prejudged that.

## Test suite state

| | Before round 003 | After |
|---|---:|---:|
| Part tests run | 313 | 319 |
| Passed | 188 | 194 |
| Failed | 125 | 125 |

The 125 failures are the pre-existing `Material not found` environment
limitation recorded in round 002. Unchanged, and no new failures.

## What was deliberately not built

No repair layer in `src/`. No change to `ElementMap`. No new reference type.
The round measured a prototype and recorded what the measurement means for the
design; that is where it stops.
