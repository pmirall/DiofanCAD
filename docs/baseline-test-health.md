# DiofanCAD — Baseline Test Health

## Purpose

Measure the starting quality of the upstream/project test estate before new architectural work is introduced.

## Status

`PARTIAL — App-level C++ suite measured; everything else still UNKNOWN`

Measured at the fork baseline commit on 2026-09-14, in a console-only build
(`BUILD_GUI=OFF`, all workbenches OFF, `ENABLE_DEVELOPER_TESTS=ON`). See
`gauntlet/rounds/architecture/round-001-p0c/evidence/baseline-tests.txt`.

### Measured: `App_tests_run`

| Metric | Value |
|---|---:|
| Tests run | 553 |
| Passed | 548 |
| **Failed** | **3** |
| Skipped | 2 |
| Disabled | 7 |
| Wall time | ~0.3 s |

Three tests fail on a clean checkout, before any DiofanCAD change:

| Test | Status |
|---|---|
| `DocumentObjectTest.getSubObjectList` | failing at baseline, cause not investigated |
| `MoveProperty.staticProperty` | failing at baseline, cause not investigated |
| `PropertyExpressionEngineTest.executeCrossPropertyReference` | failing at baseline, cause not investigated |

This matters beyond bookkeeping: a project that does not know its baseline
failures will eventually blame them on its own changes. These three are now
named, so any future failure list can be diffed against them.

Not yet investigated, and deliberately not fixed in the same round that
measured them. Candidates for the upstream backlog once root-caused.

### Measured: `Part_tests_run` (round 002)

| Metric | Value |
|---|---:|
| Tests run | 312 |
| Passed | 182 |
| **Failed** | **130** |

The 130 failures are an **environment limitation, not defects**: constructing
a `Part::Box` throws `Material not found` because the material library's
resources are only present after an install step, and a build-tree-only setup
has none. Everything that avoids document objects passes — all 90
`TopoShapeExpansionTest` tests, for instance.

This is worth knowing before anyone reads a red Part suite as a code problem.
It also means a large fraction of the Part suite is silently untestable
without installing, and nothing tells you so. Confirming that an installed
build passes those 130 has not been done.

## Inventory

Populate from the actual repository:

| Area | Test inventory | Pass | Fail | Skipped | Flaky | Coverage signal | Status |
|---|---:|---:|---:|---:|---:|---|---|
| Core | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TO VALIDATE |
| Document model | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TO VALIDATE |
| Sketcher | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TO VALIDATE |
| Part Design | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TO VALIDATE |
| Assembly | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TO VALIDATE |
| TechDraw | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TO VALIDATE |
| CAM | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TO VALIDATE |
| FEM | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TO VALIDATE |
| Import/export | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TO VALIDATE |
| Python/API | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TO VALIDATE |
| Addon/workbench compatibility | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TO VALIDATE |

## Health dimensions

Track separately:

- deterministic failures;
- flaky failures;
- missing coverage on critical workflows;
- tests that cannot run in CI;
- slow tests that block iteration;
- tests that lack meaningful assertions;
- compatibility/regression tests.

## Baseline rule

Do not use a green CI result as proof of good health without inspecting scope and test coverage.

## Exit criteria

Baseline is complete when:

```text
[ ] test inventory is reproducible
[ ] critical workflow coverage is identified
[ ] flaky tests are classified
[ ] baseline failures are recorded
[ ] CI command is documented
[ ] artifact locations are recorded
```
