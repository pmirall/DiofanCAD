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
| E-0013 | element references survive hard geometry: similar pockets, identical hole patterns, fillets | 8 refs, 6 scenarios | n/a — first measurement | absolute-coordinate ground truth, independent of the matcher | `gauntlet/rounds/architecture/round-003-repair/` | NOT YET | PASS | 2026-09-14 |
| E-0014 | the harness can detect a wrong resolution | negative control N1 | n/a | ground truth deliberately mislabelled; harness must report it | same round | reproducible by anyone | PASS | 2026-09-14 |
| E-0015 | a geometric matcher scores 1.0 on the WRONG feature after a resize | H4, repeated holes | the intended face scored 0.958 | scenario-blind matcher vs analytic ground truth | same round, `benchmark.json` → `h4_decisive_result` | NOT YET | PASS (the finding is the risk) | 2026-09-14 |
| E-0016 | exclusion by surviving references disambiguates where geometry cannot | 7 breaks, 5 scenarios | geometry-only matcher on the same breaks | three strategies, identical inputs, independent ground truth | `gauntlet/rounds/architecture/round-004-provenance/` | NOT YET | PASS | 2026-09-14 |
| E-0017 | provenance as a hard filter is harmful, not merely unhelpful | same | geometry-only on the same breaks | same | same round | NOT YET | PASS (negative result) | 2026-09-14 |
| E-0018 | exclusion's benefit depends on the document holding other references | R5 vs R1 | R1 with four references | same change, one reference | same round | NOT YET | PASS | 2026-09-14 |
| E-0019 | element identity survives the save/restore boundary unchanged | 10 faces, box with pocket | n/a — first measurement | real `ElementMap::save`/`restore` path | `gauntlet/rounds/architecture/round-005-p0d/` | NOT YET | PASS | 2026-09-14 |
| E-0020 | downstream naming is deterministic across a reload | same change applied to reloaded vs never-saved shape | never-saved shape | byte comparison of 14 mapped names | same round | NOT YET | PASS | 2026-09-14 |
| E-0021 | behaviour when the string table is lost | partial restore | — | attempted twice; the construction path creates no StringIDs | same round, M4/M5 | — | **UNKNOWN — not exercised** | 2026-09-14 |
| E-0022 | the Part test suite passes completely; the 125 failures were a resource layout | `Part_tests_run`, 331 tests | the same suite before the fix, 206/331 | two symlinks under the build root; each removed individually to confirm | `gauntlet/rounds/architecture/round-006-unblock/` | reproducible by anyone from a clean checkout | PASS | 2026-09-14 |
| E-0023 | document references survive a parameter change and a real .FCStd round-trip | `Part::Fillet` + `PropertyLinkSub`, box lengthened 20→30 | n/a — first document-level measurement | physical ground truth: which corner, which face | `gauntlet/rounds/architecture/round-007-p0e/` | NOT YET | PASS | 2026-09-14 |
| E-0024 | **no document-level Part feature carried an element map** | `Part::Box`, `Part::Cut`, `Part::Fillet` | n/a | `hasElementMap()` / `getElementMapSize()` after recompute | same round, probe P1 | NOT YET | PASS (scoped: this configuration, Part workbench, chain from primitives) | 2026-09-14 |
| E-0025 | both document references survived by stable indexing, not by the element map | same | — | `PropertyLinkSub` shadow mapped name was empty; `Part::Fillet` stores a raw index by design | same round | NOT YET | PASS | 2026-09-14 |

## Verification status

Every row above is marked `NOT YET` for independent verification, and that is not a formality. All of it was measured by one operator, in one session, on one shared cloud container with roughly 18% cross-run timing variance. Rule 3 below is satisfied only when someone who did not write `RecomputeTrace` reruns `reproduce.sh` on a quiet machine.

The claims that are robust to the environment are the structural ones (E-0005, E-0006, E-0009, E-0010, E-0011): they do not depend on timing.

E-0016 to E-0018 are conditional on abstention thresholds (score 0.50, margin 0.05) that were chosen, not derived. That is round 004's largest gap, and it means these entries should be read as *directional* rather than as rates.

E-0013 and E-0015 come from round 003, which exists because E-0012's caveat was real: its candidate counter shared a predicate with its ground truth. Round 003 separated them and added a negative control (E-0014), without which none of its zeros would be interpretable.

E-0012 carries a caveat that matters more than its status: the candidate counter shares a predicate with the ground truth, so "exactly one candidate" is partly true by construction on axis-aligned boxes. It is a necessary condition for unambiguous repair, never a sufficient one, and it must be re-measured on fillets and coplanar faces before anything is built on it. The timing claims (E-0004, E-0007, E-0008) should be treated as order-of-magnitude until reproduced.

## A warning about E-0024

E-0024 is the most consequential entry in this ledger and the most easily
over-read. It says that in **one configuration** (`BUILD_GUI=OFF`, default
parameters, Part workbench, a chain starting from primitives) no feature
carried an element map. It does **not** say element maps are absent in FreeCAD.
PartDesign chains starting from a Sketch — where the toponaming work is aimed —
were not tested, because Sketcher and PartDesign are not built here.

Until that is measured, E-0011 through E-0021 should be read as describing
`ElementMap`'s behaviour rather than a user's experience.

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
