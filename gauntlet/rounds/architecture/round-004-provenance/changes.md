# Round 004 — Changes

## Divergence introduced

**None in upstream code.**

| File | Lines | Class | What |
|---|---|---|---|
| `tests/src/Mod/Part/App/ReferenceProvenance.cpp` | +560 new | U3 | Three-strategy comparison, five break scenarios, negative control |
| `tests/src/Mod/Part/App/CMakeLists.txt` | +1 | U3 | Register it |

Still a test-file prototype. The round's job was to choose a disambiguator, not
to ship one.

## On the fairness of the comparison

Two things were checked so the result is not an artefact of the harness:

1. **Exclusion uses only what a document has.** `claimedFaces()` resolves the
   other tracked references against the changed shape. No privileged knowledge,
   no access to ground truth. R5 exists to show the mechanism has no hidden
   power: with one reference it degrades exactly to geometry alone.
2. **The negative control is repeated in this file** rather than inherited, so
   this round's zeros stand on their own.

## Known asymmetry

Provenance was implemented as a **hard filter** and exclusion as a **candidate
removal**. Those are not the same shape of mechanism, and the comparison
flatters exclusion in that respect. The verdict states the reasoning for why
provenance-as-bonus would be inert on these scenarios rather than claiming it
was tested.
