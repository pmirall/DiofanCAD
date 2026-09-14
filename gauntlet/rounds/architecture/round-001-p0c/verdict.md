# Round 001 — P0-C — Verdict

## PASS (with named gaps)

Every bar item was demonstrated by executable evidence. Two limitations are real and are recorded rather than smoothed over.

| # | Requirement | Result | Evidence |
|---|---|---|---|
| B1 | Per-object time attributed | PASS | `RecomputeTraceTest.AttributesTimePerObject`; `evidence/example-trace.json.txt` |
| B2 | Material share of wall-clock attributed | PASS, **84.5%** | `benchmark.json` → `attribution` |
| B3 | Causality attributed per object | PASS, 0 `unknown` | `AttributesCausalityToPropagation` |
| B4 | Propagation edges with property | PASS, 1999 edges, property-level | `benchmark.json` → `causality` |
| B5 | Semantics unchanged | PASS | `DoesNotChangeRecomputeSemantics` |
| B6 | No test regression | PASS | 3 pre-existing failures before and after; no new failures |
| B7 | Disabled cost not measurable | PASS | below the container's noise floor |
| B8 | Enabled cost quantified | PASS, **~1.09 µs/object** | median of 5 within-process samples |
| B9 | Machine-readable | PASS | `WritesJsonToOutputPath` |

No critical failure occurred.

## What the trace produces

```json
{"name": "tracetest#A", "order": 0, "recomputed": true, "seconds": 0.000009372, "cause": "pre-touched"},
{"name": "tracetest#B", "order": 1, "recomputed": true, "seconds": 0.000008085, "cause": "propagated",
 "cause_source": "tracetest#A", "cause_property": "Link"}
```

That is the P0-C hypothesis in concrete form: *what ran, how long it took, and who caused it.*

## Honest reading of the cost

The enabled overhead is **1.04–1.37 µs per object**, median 1.09 µs. On this fixture that is 8–12%, because `App::FeatureTest` executes in roughly 7–11 µs — close to the cheapest feature that can exist. A PartDesign feature taking 1 ms would pay about 0.1%.

V7 §21's ">10% fails" threshold is not applied to this number, and the reason is stated rather than assumed: the policy governs what users experience, and the trace is opt-in and off by default. The number that would govern users is the **disabled** cost, which is below what this machine can measure. If the trace ever becomes on-by-default, §21 applies in full and this measurement must be redone on a quiet machine.

## Falsification conditions, revisited

| Condition | Outcome |
|---|---|
| Cannot attribute time | Falsified — per-object, 84.5% of wall-clock |
| Cannot attribute causality | Falsified — 0 unknown causes, property-level edges |
| Perturbs what it measures | Falsified when disabled; when enabled the perturbation is measured and bounded, not hidden |

## Decision

**KEEP.** P0-C passes. The instrumentation stays as `U2` (experimental) in `docs/upstream-backlog.md`.

This unblocks P0-E (change resilience), which needs exactly this: an executable record of what survived a change and why it recomputed. It does not license broader architecture work — P0-A, P0-B and P0-D remain unproven.

## Caveat on the environment

Measured on a shared cloud container with ~18% cross-run variance. Within-process interleaved comparison is the only figure treated as signal here. Before any external claim, this must be rerun on a controlled machine per `docs/benchmark-protocol.md`.
