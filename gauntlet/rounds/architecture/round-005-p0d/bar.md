# Round 005 — P0-D Save/Reload — Bar

Written before the measurement was run.

## Why this and not the threshold gap

Round 004's largest gap is that the abstention thresholds were guessed. That
gap limits how far the *repair rates* can be trusted — and repair is now known
to be a modest feature (D-014).

P0-D sits underneath all of it. Rounds 002–004 measured reference survival on
shapes that were **never persisted**. If identity does not survive the
persistence boundary, none of those numbers describe a real model, which is
always saved and reopened. This is a validity check on three rounds of work,
and it is a required member of the Phase 0 proof set.

## Proof

**Hypothesis:** identity metadata remains coherent across persistence boundaries.

**Falsification:** identity is not deterministic across save/reload, or the
round-trip loses information in a way that cannot be classified as `SAFE LOSS`.

## Reframed by D-010

P0-D was written assuming DiofanCAD would add its own persistent semantic
metadata. D-010 decided it will not — `ElementMap` and `MappedName` are reused.
So P0-D becomes: **does the baseline's identity survive its own persistence
boundary, and do rounds 002–004's findings still hold after a save/reload
cycle?**

## What gets measured

| # | Measurement | Why it matters |
|---|---|---|
| M1 | Every mapped name resolves to the same face after `save` → `restore` | the minimum; if this fails nothing else matters |
| M2 | A stored reference still resolves correctly after the shape is reloaded | this is what a reopened document does |
| M3 | **Downstream determinism**: the same change applied to a reloaded shape produces the same mapped names as applied to a never-saved shape | V7 §81. If a reload silently changes downstream naming, every reference captured before a save is at risk after one |
| M4 | What happens when the map is restored with a **fresh** hasher instead of the saved one | the `StringHasher` is separate persistent state; §52.5 requires each incompatibility to be classified |

## Prediction, recorded before measuring

> M1, M2 and M3 will hold — the element map is designed for exactly this and is
> covered by upstream tests. M4 will lose information, because mapped names
> reference string IDs held by the hasher, and a fresh hasher cannot resolve
> them. The open question is whether that loss is **visible** (`SAFE LOSS`:
> names fail to resolve and say so) or **silent** (`UNSAFE CHANGE`: names
> resolve to something else).

If M4 turns out silent, that is the round's headline and a genuine defect
class, not a curiosity.

## Bar

| # | Requirement | Measured how |
|---|---|---|
| B1 | Round-trip through the real `save`/`restore` path, not a copy | `ElementMap::save(ostream)` / `restore(hasher, istream)` |
| B2 | Negative control fires | a deliberately mislabelled expectation must be reported |
| B3 | M3 compares reloaded against never-saved on identical input | same operands, same tags |
| B4 | M4's loss is classified per §52.5 | `SAFE LOSS` / `VISIBLE LOSS` / `UNSAFE CHANGE` / `BLOCKED` |
| B5 | No upstream divergence | test file only |

## Critical failures

- **M3 fails** → references captured before a save are unreliable after one, and rounds 002–004's numbers do not transfer to real models.
- **M4 is silent** → a reference can resolve to the wrong element after a partial restore, which is the silent corruption every previous round failed to find.

## Out of scope

Document-level `.FCStd` round-trip (blocked by the material-library limitation
from round 002), what upstream FreeCAD does with a DiofanCAD-written file
(nothing is written differently — D-010), edges and vertices.
