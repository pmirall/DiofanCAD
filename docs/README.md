# DiofanCAD — Engineering Document Set

This directory is the program's control system, not its documentation folder. V7 treats the repository, benchmarks, Gauntlet state, evidence ledger and architecture records as one system: if a decision, measurement or claim is not here, it does not exist.

Charter: [`spec/diofancad-fusion-killer-spec-v7.md`](spec/diofancad-fusion-killer-spec-v7.md)

## Reality map — what is actually true today

| Document | Answers | State |
|---|---|---|
| [baseline-architecture.md](baseline-architecture.md) | Where does each subsystem live, and what do we actually know about it? | LOCATED / NOT MEASURED |
| [baseline-feature-matrix.md](baseline-feature-matrix.md) | What does the platform already do? | UNKNOWN |
| [baseline-performance.md](baseline-performance.md) | How fast is it, per latency class? | UNKNOWN |
| [baseline-test-health.md](baseline-test-health.md) | What does the test estate really cover? | UNKNOWN |
| [baseline-compatibility.md](baseline-compatibility.md) | What round-trips, and what silently loses data? | UNKNOWN |
| [dependencies-license-matrix.md](dependencies-license-matrix.md) | What are we allowed to ship, and what can we maintain? | UNKNOWN |

## Architecture — what we are allowed to build

| Document | Answers |
|---|---|
| [architecture-boundary.md](architecture-boundary.md) | Who owns what; what must never be duplicated |
| [architecture-risks.md](architecture-risks.md) | The register, AR-001 … AR-010, and what would falsify each |
| [architecture-proof-experiments.md](architecture-proof-experiments.md) | The P0-A … P0-E proof set |
| [decisions.md](decisions.md) | The decision log, D-001 … |
| [kill-criteria.md](kill-criteria.md) | When to stop, pivot or drop |

## Evaluation — how a claim earns the right to exist

| Document | Answers |
|---|---|
| [evidence-ledger.md](evidence-ledger.md) | Every claim, its baseline, method and status |
| [benchmark-protocol.md](benchmark-protocol.md) | How a measurement is taken so it can be repeated |
| [golden-workflow-suite.md](golden-workflow-suite.md) | The 13 representative workflows |
| [first-wedge-benchmark.md](first-wedge-benchmark.md) | The first falsifiable product claim: change resilience |
| [fusion-benchmark-plan.md](fusion-benchmark-plan.md) | How the Fusion 360 comparison is run fairly |
| [fusion-scorecard.md](fusion-scorecard.md) | The ten scoring dimensions |
| [red-team-plan.md](red-team-plan.md) | The mutations a "robust" workflow must survive |
| [human-blind-protocol.md](human-blind-protocol.md) | Real engineers, identity hidden until after judgment |
| [master-integration-plan.md](master-integration-plan.md) | Sketch → … → Reload; module PASS is not product PASS |
| [milestone-exit-gates.md](milestone-exit-gates.md) | What a phase must clear to be called done |

## Upstream — the constitutional obligation

| Document | Answers |
|---|---|
| [upstream-strategy.md](upstream-strategy.md) | U0 … U3 classes and the extraction pipeline |
| [upstream-backlog.md](upstream-backlog.md) | The queue of extractable changes |
| [upstream-sync.md](upstream-sync.md) | Where the fork was cut and what divergence costs |
| [community-gap-map.md](community-gap-map.md) | Signal → requirement, without popularity becoming specification |
| [competitor-landscape.md](competitor-landscape.md) | What to learn from whom, and what to ignore |

## Reading these documents honestly

Almost every cell reads `UNKNOWN` or `TO VALIDATE`. That is the intended state of a program that has cut its fork and measured nothing yet — the alternative would be invented facts, which V7 forbids outright. The documents are shaped so that filling a cell requires producing an artifact.

Three rules carry most of the weight:

1. A blank or `UNKNOWN` cell is never evidence of absence. It is a research task.
2. Nothing moves from `UNKNOWN` to `SUPERIOR` in one step.
3. A failed result stays in the ledger after the implementation changes.
