# DiofanCAD — Baseline Performance

## Purpose

Establish the measured performance baseline before architecture or product optimization.

V7 requires independent tracking of startup, open/save, selection, viewport, Sketcher solve, recompute, assembly solve, drawing update, CAM generation, FEM preparation, and memory.

## Status

`UNKNOWN — repository benchmark not yet executed`

## Measurement rules

Record:

```text
run_id
commit
build configuration
OS
CPU
GPU
RAM
model identifier
model size
warm/cold state
operation
start timestamp
end timestamp
duration
peak memory
result status
artifact path
```

For interactive operations, prefer percentile latency where multiple repetitions are possible. For long-running jobs, report wall-clock time and peak resource usage.

## Initial benchmark table

| Operation | Benchmark | Baseline | Target | Status |
|---|---|---:|---:|---|
| Startup | representative launch | UNKNOWN | establish baseline | TO VALIDATE |
| Open | golden mechanical model | UNKNOWN | establish baseline | TO VALIDATE |
| Save | same model | UNKNOWN | establish baseline | TO VALIDATE |
| Selection | representative selection task | UNKNOWN | establish baseline | TO VALIDATE |
| Viewport | first usable display | UNKNOWN | establish baseline | TO VALIDATE |
| Sketcher solve | defined sketch | UNKNOWN | establish baseline | TO VALIDATE |
| Recompute | defined dependency graph | UNKNOWN | establish baseline | TO VALIDATE |
| Assembly solve | large-assembly fixture | UNKNOWN | establish baseline | TO VALIDATE |
| Drawing update | released drawing fixture | UNKNOWN | establish baseline | TO VALIDATE |
| CAM generation | defined CAM fixture | UNKNOWN | establish baseline | TO VALIDATE |
| FEM preparation | defined study fixture | UNKNOWN | establish baseline | TO VALIDATE |
| Memory | representative large model | UNKNOWN | establish baseline | TO VALIDATE |

## Regression policy

V6/V7 default policy:

```text
<5% slower      acceptable
5–10%           warning
>10%            fail
```

Exceptions require evidence and an explicit decision record.

## Required next actions

1. Build a repeatable benchmark harness.
2. Freeze hardware/software configuration for primary comparisons.
3. Establish cold and warm runs where relevant.
4. Store raw measurements with the evidence ledger.
5. Replace every `UNKNOWN` above with measured data.
