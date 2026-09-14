# DiofanCAD — Upstream Backlog

## Purpose

Maintain an explicit queue of changes that may be extracted, proposed, or deliberately kept experimental.

## Record format

| ID | Change | Class | Problem | Evidence | Dependencies | Compatibility | Performance | Upstream boundary | Status |
|---|---|---|---|---|---|---|---|---|---|
| U-001 | TO VALIDATE | U2 | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | UNKNOWN |

## Promotion rules

A candidate moves only with evidence:

```text
U3 experimental
  ↓ evidence
U2 experimental
  ↓ stabilization
U1 upstream candidate
  ↓ reviewable extraction
U0 upstream-ready
  ↓ upstream decision
UPSTREAM-ACCEPTED
  ↓ sustained maintenance
UPSTREAM-MAINTAINED
```

## Drop / defer rules

A candidate should be dropped or deferred when its measured value no longer justifies:

- implementation cost;
- maintenance cost;
- divergence cost;
- compatibility complexity;
- upstream extraction cost.
