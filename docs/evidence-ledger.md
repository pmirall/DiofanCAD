# DiofanCAD — Evidence Ledger

## Purpose

Make every important claim traceable to reproducible evidence.

## Status vocabulary

```text
UNKNOWN
IN_PROGRESS
FAIL
PASS
SUPERIOR
REGRESSED
```

## Record format

| ID | Claim | Scope | Baseline | Method | Evidence | Independent verification | Status | Date |
|---|---|---|---|---|---|---|---|---|
| E-0001 | V7 architecture hypothesis is viable | semantic/reference proof | current FreeCAD baseline | architecture proof gate | TO CREATE | TO CREATE | UNKNOWN |  |
| E-0002 | DiofanCAD preserves intent better under a defined change challenge | Slice A | benchmark baseline | blind/controlled workflow | TO CREATE | TO CREATE | UNKNOWN |  |
| E-0003 | performance regression remains within policy | affected subsystem | recorded baseline | repeatable benchmark | TO CREATE | TO CREATE | UNKNOWN |  |
| E-0004 | recompute time can be attributed per object | `Document::recompute`, 2000-object chain | `TimeTracker` gives 3 run-level checkpoints and nothing per object | `App::RecomputeTrace` + gtest; median of 7, interleaved | `gauntlet/rounds/architecture/round-001-p0c/` | NOT YET — single operator, single machine | PASS | 2026-09-14 |
| E-0005 | recompute causality can be attributed per object | same | baseline records errors only (`_RecomputeLog`) | propagation edges observed in-run; 0 unknown causes in a 3- and 2000-object chain | same round, `evidence/example-trace.json.txt` | NOT YET | PASS | 2026-09-14 |
| E-0006 | the instrumentation does not perturb recompute | same | uninstrumented build of the same commit | semantics test + cross-build timing | same round, `benchmark.json` | NOT YET | PASS | 2026-09-14 |
| E-0007 | 84.5% of recompute wall-clock is attributable; 15.5% is not | same | n/a — first measurement | sum of per-object times vs run total | same round | NOT YET | PASS | 2026-09-14 |
| E-0008 | enabled trace costs ~1.09 µs per object | same | trace-off in the same process | within-process interleaved comparison, 5 samples | same round | NOT YET | PASS | 2026-09-14 |
| E-0009 | `BUILD_GUI=OFF` cannot configure at the fork baseline | `cMake/FreeCAD_Helpers/SetupQt.cmake` | n/a — defect report | configure at baseline commit and observe the failure | round 001 `changes.md` | reproducible by anyone from a clean checkout | PASS (defect confirmed, fixed) | 2026-09-14 |
| E-0010 | element references never silently resolve to the wrong face under change | 60 refs, 6 scenarios, Part booleans | n/a — first measurement | geometric ground truth: normal + outer/pocket | `gauntlet/rounds/architecture/round-002-p0a/` | NOT YET | PASS | 2026-09-14 |
| E-0011 | 86.7% of references survive a topology-changing edit correctly | same | same | same | same round, `benchmark.json` | NOT YET | PASS | 2026-09-14 |
| E-0012 | every broken reference had exactly one matching descriptor candidate | 8 broken refs | n/a | candidate count per broken reference | same round | NOT YET | PASS (necessary, not sufficient — see caveat) | 2026-09-14 |

## Verification status

Every row above is marked `NOT YET` for independent verification, and that is not a formality. All of it was measured by one operator, in one session, on one shared cloud container with roughly 18% cross-run timing variance. Rule 3 below is satisfied only when someone who did not write `RecomputeTrace` reruns `reproduce.sh` on a quiet machine.

The claims that are robust to the environment are the structural ones (E-0005, E-0006, E-0009, E-0010, E-0011): they do not depend on timing.

E-0012 carries a caveat that matters more than its status: the candidate counter shares a predicate with the ground truth, so "exactly one candidate" is partly true by construction on axis-aligned boxes. It is a necessary condition for unambiguous repair, never a sufficient one, and it must be re-measured on fillets and coplanar faces before anything is built on it. The timing claims (E-0004, E-0007, E-0008) should be treated as order-of-magnitude until reproduced.

## Rules

1. Every SUPERIOR claim must point to a baseline and method.
2. Evidence must identify the exact build/commit used.
3. Independent verification means an evaluator who did not author the implementation or primary claim where practical.
4. A failed result remains in the ledger; it is not deleted because the implementation later changes.
5. Repeated measurements must remain linked to the same claim with distinct run IDs.

## Evidence bundle

A meaningful benchmark claim should carry:

```text
input
configuration
commands / procedure
raw measurements
verification artifact
human-readable result
verdict
```
