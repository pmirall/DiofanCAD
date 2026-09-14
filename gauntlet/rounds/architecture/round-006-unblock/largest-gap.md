# Round 006 — Largest Gap

**Largest Gap:** P0-E has never been run, and it is now the only thing standing between Phase 0 and its exit.

## Why this one

Phase 0's proof set is P0-A through P0-E. Four are settled: P0-A, P0-C and
P0-D pass, and P0-B lost most of its purpose to D-010 and needs restating
before it is worth running. P0-E — the realistic change challenge, end to end,
at document level — was blocked by round 006's two symlinks and is now not.

It is also the proof closest to the product claim. Everything measured so far
has been at `TopoShape` level: shapes built directly, operations re-run by
hand. P0-E is the first measurement of a *document* being changed and
recomputed the way a user changes it.

## Gaps deliberately not chosen

| Gap | Why it waits |
|---|---|
| The string-table case from round 005 | Now testable, but it is a compatibility question; P0-E gates the phase |
| The three unexplained App failures | Recorded since round 001, still unexplained, still not blocking |
| Principled abstention thresholds | Round 004's gap; limits repair rates, and repair is modest |
| Edges, vertices, sketch external geometry | Open since round 002 |
