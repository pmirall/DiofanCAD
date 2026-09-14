# Round 005 — Largest Gap

**Largest Gap:** the programme cannot test document-level persistence at all, because a build-tree-only setup cannot construct a `Part::Box` — and that single blocker now gates the string-table question, P0-E, and 125 Part tests.

## Why this one

Round 002 recorded the blocker as an aside: 125 of 312 Part tests fail with
`Material not found` because the material library's resources only exist after
an install. Three rounds later it has become the thing standing between the
programme and every remaining question:

- the string-table dependency (round 005's `UNCLASSIFIED`) needs a real
  document save;
- P0-E's change challenge wants a document recompute, not a TopoShape chain;
- 125 tests are silently untestable, so the Part suite's health is unknown;
- V7 §52.5's round-trip matrix needs `.FCStd` files, which need documents.

It is also the cheapest gap on the list: it is an install step, not a design
problem.

## The next action

Produce an installed build (`cmake --install` into a prefix) and re-run the
Part suite from it. That single step either unblocks all of the above or
reveals that the 125 failures are more than a resources problem — and either
answer is worth more than another round at TopoShape level.

## Gaps deliberately not chosen

| Gap | Why it waits |
|---|---|
| Principled abstention thresholds | Round 004's gap, still open; it limits repair *rates*, and repair is a modest feature |
| Edges and vertices | Open since round 002 |
| Sketch external geometry | Needs Sketcher built |
| P0-B semantic object prototype | D-010 removed most of its purpose; it needs restating before it is worth running |
