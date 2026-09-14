# Round 001 — Largest Gap

**Largest Gap:** 15.5% of recompute wall-clock is not attributed to any named object, so a slow recompute can still end in "the framework did it" rather than a specific cause.

## Why this one

The round passed, so this gap sets the next iteration rather than blocking. Of the known gaps it is the only one that limits the *diagnostic* claim: a user asking "why is my recompute slow?" can today be answered for 84.5% of the time and told "overhead" for the rest. The unattributed slice is topological sort, propagation bookkeeping, signal emission and teardown — each individually cheap, none individually measured.

## The other known gaps, deliberately not chosen

| Gap | Why it waits |
|---|---|
| `Document::recomputeFeature()` (single-object path) is not instrumented | A smaller version of the same hook; mechanical once the design is settled |
| Async recompute is not covered | The trace is mutex-guarded but keeps one global run; concurrent runs would interleave. Needs a design decision, not a fix |
| No Python binding | Deliberate DEFER; JSON covers the benchmark harness |
| Only the last run is retained | Bounded memory was the intent; a ring buffer is trivial when a consumer needs it |
| Cause `pre-touched` does not say *who* touched it | Requires instrumenting property changes outside recompute — a larger scope, and arguably P0-A's territory |
