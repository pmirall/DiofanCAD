# Round 005 — P0-D Save/Reload — Verdict

## PASS on what could be measured. One measurement could not be run, and is recorded as such.

| # | Requirement | Result |
|---|---|---|
| B1 | Real `save`/`restore` path | PASS — `ElementMap::save(ostream)` / `restore(hasher, istream)` |
| B2 | Negative control fires | PASS — N3 reports 4 differences |
| B3 | M3 compares reloaded against never-saved | PASS |
| B4 | M4's loss classified per §52.5 | **NOT POSSIBLE** — the case could not be exercised; classified `UNCLASSIFIED` rather than assumed safe |
| B5 | No upstream divergence | PASS — one new test file |

## The result that matters

**M3: downstream naming is deterministic across a reload.**

The same change applied to a reloaded shape produced **byte-identical mapped
names** to the same change applied to a never-saved one — 14 faces, 14
identical, 0 differing.

That is the validity check rounds 002–004 needed. Their numbers were measured
on shapes that had never been persisted; a real model is always saved and
reopened. If a reload had shifted downstream naming, every reference captured
before a save would have been at risk after one, and three rounds of survival
measurements would not have described real use. They do.

M1 (10 of 10 names identical through save → restore) and M2 (a stored reference
still resolving to the same face) are the floor beneath that, and both hold.

## The measurement that did not happen

M4 asked what happens when the element map is restored with a string table that
never saw the save. It came back perfectly clean — and means nothing.

```
M4 hasher_entries_at_save=0
M4 NOT EXERCISED: the save stored no string IDs, so restoring with a fresh
   hasher is indistinguishable from restoring with the real one
```

I tried again in M5 with a shared `StringHasher` across all operands and a
five-deep boolean chain producing 26 faces. Still zero. The reason is in
`ElementMap::beforeSave`: it **marks** StringIDs already attached to mapped
names, it does not create them. The names produced by this construction path
carry none, so the string table stays empty no matter how deep the chain goes.

So the string-table dependency is real in the code and unreachable from here.
It is recorded as `UNCLASSIFIED`, not as safe.

## On the prediction

The bar predicted M4 would lose information. That prediction is neither
confirmed nor refuted — it was never tested. Scoring it as a pass because the
numbers looked clean is exactly the error this project keeps catching in
itself, and it is the second time in three rounds that a clean result turned
out to be a measurement that never fired.

The guard that caught it was one line of diagnostic asking whether the
mechanism under test had been invoked at all. That guard is worth making
routine: **a measurement should report whether it was exercised, not only what
it found.**

## Decision

P0-D **PASS**, with the string-table case explicitly open.

Phase 0's proof set now stands at: P0-A PASS, P0-C PASS, P0-D PASS,
**P0-B and P0-E unproven**. Phase 0 cannot exit until those are done and the
milestone's other bars are met (§78).

## Caveats

TopoShape-level persistence, not a document `.FCStd` round-trip. One shape
family, faces only. The unmeasured string-table case needs document-level
persistence, which is still blocked by the material-library limitation from
round 002.
