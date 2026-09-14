# DiofanCAD — Open Engineering CAD

DiofanCAD is an experimental product-evolution layer derived from [FreeCAD](https://github.com/FreeCAD/FreeCAD).

Its purpose is to explore how far an open-source engineering CAD platform can go on important real-world workflows, while continuously producing useful, reviewable improvements that can return upstream.

```text
            FREECAD UPSTREAM
                   ▲
                   │  useful contributions
                   │
            DIOFANCAD LAB
                   │  experiments / product
                   │  benchmarks / Gauntlet
                   │  agents
```

**DiofanCAD is not a hostile replacement for FreeCAD.** Upstream remains sovereign. The lab may experiment aggressively; the contributions it sends back must be reviewable, narrow and tested.

## Where things are

| Path | What it is |
|---|---|
| [`docs/`](docs/README.md) | The engineering document set: baselines, architecture records, evidence ledger, upstream strategy |
| [`docs/spec/`](docs/spec/diofancad-fusion-killer-spec-v7.md) | The V7 charter that governs the program |
| [`gauntlet/`](gauntlet/README.md) | The reproducible evaluation system: protocols, state, rounds, evidence |
| [`tools/diofancad/`](tools/diofancad/) | Program tooling, including the fork divergence report |
| everything else | FreeCAD upstream, unmodified |

That last row is the point. At the fork baseline, DiofanCAD changes **zero** lines of upstream code. Every line it eventually changes is a cost to be justified, measured and, where possible, extracted back upstream.

## Current status

**Phase 0 — Reality Map.** Terrain prepared; nothing measured.

No baseline has been measured, no architecture proof has been run, and no claim of superiority over anything exists. The honest status of essentially every question this project asks is `UNKNOWN`, and that is recorded rather than hidden.

## How work happens here

```text
PROVE → MEASURE → DECIDE → SCALE
```

and never the inverse:

```text
DESIGN → ASSUME → IMPLEMENT → DEFEND
```

Five rules govern execution (V7 §0.5):

1. Prove architecture before scaling scope.
2. Prefer one source of truth over parallel models.
3. Win one workflow decisively before expanding the platform.
4. Every major idea needs a kill criterion.
5. Fork divergence is budgeted like any other engineering resource.

## The first wedge

One claim comes before any others, and it is falsifiable:

> DiofanCAD preserves engineering intent through realistic change better than the benchmark baseline.

Measured as: create a part, create a downstream artifact, change a parameter / the topology / the feature order, then count what survives, what can be repaired, and what is still true after save and reload. See [`docs/first-wedge-benchmark.md`](docs/first-wedge-benchmark.md).

No expansion into CAM, FEM, ECAD or AI is justified by architecture alone. The wedge produces measurable value first.

## Before you contribute

- Read [`docs/architecture-boundary.md`](docs/architecture-boundary.md). The semantic layer is not permitted to become a second CAD model.
- Read [`docs/upstream-sync.md`](docs/upstream-sync.md). Changes to upstream code are divergence, and divergence is budgeted.
- Classify the change: `U0` upstream-ready, `U1` upstream candidate, `U2` experimental, `U3` DiofanCAD-only. See [`docs/upstream-strategy.md`](docs/upstream-strategy.md).
- Claims go in [`docs/evidence-ledger.md`](docs/evidence-ledger.md) with a baseline and a method, or they do not go anywhere.

Upstream FreeCAD's own contribution guidance still applies to everything under `src/`: see [`CONTRIBUTING.md`](CONTRIBUTING.md).

## Naming

- **DiofanCAD** — this product, branch and lab.
- **FreeCAD** — the upstream project.
- **Fusion 360** — the primary commercial benchmark.
- *"Fusion Killer"* — an internal, falsifiable mission label used in the charter. It is never the public product identity, and it is unearned until the benchmarks say otherwise.

## Licence

DiofanCAD inherits FreeCAD's LGPL-2.1 licence. See [`LICENSE`](LICENSE).
