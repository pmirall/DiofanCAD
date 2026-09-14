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
