# Round 010 — Changes

## Divergence introduced

**None in upstream code.**

| File | Lines | Class | What |
|---|---|---|---|
| `tests/src/Mod/Part/App/ReferencePopulation.cpp` | +250 new | U3 | Three observations, plus positive and negative controls |
| `tests/src/Mod/Part/App/CMakeLists.txt` | +1 | U3 | Register it |

## The question was corrected before it was asked

Round 009 handed this round a gap phrased as a *rate*: "how often do the 48
capable headers actually hold a mapped name?". Reading
`_updateElementReference` first showed there is no rate to measure — the
fraction in any model is a function of what that model references.

So the bar committed, before measuring, to reporting the **rule** and to
treating any bare population rate as a critical failure. That reframing is the
most useful thing this round did, and it came from reading the source rather
than running anything.

## Controls, and what they cost

Both controls live in the same run as the measurement, which matters here:
M1 reports an **empty** result for the primitive, and an empty result is only
meaningful if the probe has been shown to see a non-empty one. It does, in the
same test, before the assertion.
