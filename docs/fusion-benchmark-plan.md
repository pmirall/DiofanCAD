# DiofanCAD — Fusion Benchmark Plan

## Objective

Create a falsifiable comparison between DiofanCAD and Fusion 360 on real engineering workflows while preserving fair starting conditions and explicit uncertainty.

## Status

`PLANNING — benchmark execution pending`

## Golden workflow candidates

Use the V7 suite as the starting set:

1. Bracket
2. Parametric enclosure
3. Gearbox
4. Mounting assembly
5. Motion assembly
6. Complex housing
7. Surface-heavy consumer object
8. 3-axis milling part
9. Multiaxis candidate
10. Structural simulation
11. PCB enclosure with connectors and keepouts
12. Released drawing + BOM + revision
13. End-to-end product release

## Protocol

For each workflow:

```text
1. identical starting conditions
2. target state
3. explicit assumptions
4. execute
5. record timing
6. record interaction count
7. record failures
8. verify final state
9. save/reload
10. apply change challenge
11. run Model Doctor where available
12. run performance check
13. produce evidence bundle
```

## Scoring dimensions

- Correctness
- Robustness
- Efficiency
- Discoverability
- Interaction quality
- Recovery
- Performance
- Automation
- Data ownership
- Integration

Critical failures override arithmetic.

## Blind comparison

When comparing user-facing workflows:

```text
Candidate A
Candidate B
```

Hide product identity, randomize order, and record run metadata before reveal.

## Result states

```text
SUPERIOR
TIED
INFERIOR
UNKNOWN
```

Never convert `UNKNOWN` directly to `SUPERIOR`.

## Human evaluation

Agents are not users. Major claims must eventually include real-engineer evaluation with identity hidden until after judgment.
