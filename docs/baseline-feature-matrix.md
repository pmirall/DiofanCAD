# DiofanCAD — Baseline Feature Matrix

## Status

- Document class: Product / capability reality map
- Pass: 1
- Evidence status: INITIAL / TO VALIDATE

## Purpose

Map what the baseline platform actually provides before prioritizing implementation. A blank or UNKNOWN cell is not evidence of absence.

## Core matrix

| Capability | Existing FreeCAD capability | DiofanCAD delta hypothesis | Fusion benchmark needed | Status |
|---|---|---|---|---|
| Sketch creation | UNKNOWN | snapping / discoverability / semantic references | YES | UNKNOWN |
| Constraint editing | UNKNOWN | diagnostics / intent preservation | YES | UNKNOWN |
| Part Design | UNKNOWN | change resilience | YES | UNKNOWN |
| Holes / patterns | UNKNOWN | semantic hole identity | YES | UNKNOWN |
| Topology-changing edits | UNKNOWN | persistent semantic references / repair | YES | UNKNOWN |
| Recompute | UNKNOWN | observability / deterministic semantics | YES | UNKNOWN |
| Model health | UNKNOWN | Model Doctor | YES | UNKNOWN |
| Assembly | UNKNOWN | source/occurrence / joints / configurations | YES | UNKNOWN |
| Drawing | UNKNOWN | associativity / revision / BOM | YES | UNKNOWN |
| CAM | UNKNOWN | manufacturing semantics / verification | YES | UNKNOWN |
| FEM | UNKNOWN | provenance | YES | UNKNOWN |
| Surfaces | UNKNOWN | robustness / healing / continuity | YES | UNKNOWN |
| ECAD ↔ MCAD | UNKNOWN | explicit synchronization | YES | UNKNOWN |
| Open data / PDM | UNKNOWN | project/revision/dependency model | YES | UNKNOWN |
| Python / automation | UNKNOWN | semantic API | YES | UNKNOWN |
| Agent automation | UNKNOWN | transactions / permissions | YES | UNKNOWN |
| Offline operation | UNKNOWN | zero-server capability | YES | YES |
| Interoperability | UNKNOWN | open formats / explicit contracts | YES | YES |

## Priority rules

A capability becomes committed work only when:

1. the user problem is demonstrated;
2. the baseline is measured;
3. the proposed delta is defined;
4. the benchmark bar is defined;
5. capacity exists or another commitment is displaced.

## First wedge

The first proof-of-value is change resilience through engineering semantics:

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

## Anti-pattern

Do not interpret this matrix as a feature backlog. It is a reality map. Feature work follows evidence and the wedge strategy.
