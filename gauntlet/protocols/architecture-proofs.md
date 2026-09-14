# Architecture Proof Set — P0-A … P0-E

Before scaling a major architecture, prove the smallest version that could falsify the idea (V7 §75).

Each proof must record, in its round directory:

```text
hypothesis
minimum implementation
expected result
falsification condition
measured result
decision
artifact
```

## P0-A — Persistent-reference prototype

**Hypothesis:** a persistent/semantic reference can survive a representative topology-changing edit more reliably than the baseline.

**Minimum implementation:** the smallest reference mechanism capable of running one realistic change challenge. Start from the baseline's existing element identity (`src/App/ElementMap.*`, `MappedName.*`) and state explicitly what it cannot do before adding anything.

**Falsification:** no measurable improvement over baseline, unacceptable ambiguity in candidate matching, or complexity disproportionate to the gain.

## P0-B — Semantic-object prototype

**Hypothesis:** engineering semantics can be added without creating a second authoritative geometry/parameter model.

**Minimum implementation:** one semantic object, one explicit ownership boundary, persistence, reload, and one downstream query.

**Falsification:** duplicated authority, synchronization instability, or disproportionate architecture complexity.

## P0-C — Recompute instrumentation prototype

**Hypothesis:** dependency propagation and recompute behaviour can be observed well enough to support deterministic, reproducible diagnosis.

**Minimum implementation:** instrument the existing dependency graph in `src/App/Document.cpp` — invalidation, propagation, recompute cost — without changing its semantics.

**Falsification:** the instrumentation cannot attribute time or causality, or it perturbs the behaviour it measures.

## P0-D — Save/reload prototype

**Hypothesis:** new identity/semantic metadata remains coherent across persistence boundaries.

**Minimum implementation:** write semantic metadata through the existing persistence path, reload it, and verify identity survives. Include the reverse case: what upstream FreeCAD does when it opens the same file.

**Falsification:** identity is not deterministic across save/reload, or the file becomes unreadable or lossy for upstream FreeCAD in a way that cannot be classified as `SAFE LOSS`.

## P0-E — Realistic change-challenge prototype

**Workflow:**

```text
Create part
 ↓
Create downstream artifact
 ↓
Change parameter / topology / feature order
 ↓
Measure surviving references
 ↓
Measure downstream validity
 ↓
Recover failures
 ↓
Save / reload
```

**Falsification:** no reproducible, material improvement over the defined baseline. See `docs/first-wedge-benchmark.md` for the metric set.

## Failure rule

A failed proof produces an architecture review. It does not automatically justify adding more code or broadening scope, and it is never quietly rerun until it passes. Record the failure, then decide KEEP / PIVOT / DROP per `docs/kill-criteria.md`.
