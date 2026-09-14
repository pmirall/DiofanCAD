# DiofanCAD — Baseline Test Health

## Purpose

Measure the starting quality of the upstream/project test estate before new architectural work is introduced.

## Status

`UNKNOWN — repository test inventory not yet executed`

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
