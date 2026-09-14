# Round 001 — P0-C Recompute Instrumentation — Bar

## Proof

P0-C, from `gauntlet/protocols/architecture-proofs.md`.

**Hypothesis:** dependency propagation and recompute behaviour can be observed well enough to support deterministic, reproducible diagnosis.

**Minimum implementation:** instrument the existing dependency graph in `src/App/Document.cpp` — invalidation, propagation, recompute cost — without changing its semantics.

**Falsification:** the instrumentation cannot attribute time or causality, or it perturbs the behaviour it measures.

## Bar

The proof PASSES only if all of the following are demonstrated by executable evidence:

| # | Requirement | Measured how |
|---|---|---|
| B1 | Per-object execution time is attributed, not just a run total | trace reports seconds per object; sum ≤ run total |
| B2 | A material share of run wall-clock is attributed to named objects | attributed fraction reported, and stated honestly if < 100% |
| B3 | Causality is attributed: why each object recomputed | every recomputed object carries `pre-touched` or `propagated` + source; `unknown` is a failure |
| B4 | Propagation edges are observed, with the property when available | edge list non-empty for a dependency chain |
| B5 | Recompute semantics are unchanged | same recomputed count, touched and error state, trace on vs off |
| B6 | No test regression | full `App_tests_run` compared against the pre-change baseline |
| B7 | Disabled cost is not measurable | instrumented binary with trace off vs uninstrumented binary |
| B8 | Enabled cost is quantified in absolute terms | ns per object, not only a percentage |
| B9 | The trace is machine-readable | JSON consumable without Python bindings |

## Critical failures

Any of these fails the round regardless of the other results:

- recompute semantics differ with the trace enabled (B5);
- a new test failure attributable to the change (B6);
- an object recomputes with cause `unknown` in a simple chain (B3).

## Out of scope for this round

Single-object `Document::recomputeFeature()`, async recompute, Python bindings, and attribution of the framework's own time. Named in `largest-gap.md` rather than silently dropped.
