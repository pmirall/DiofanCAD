# First Wedge Benchmark — Change Resilience

> **Reframed on 2026-09-14 by round 002 (P0-A).** Reference *survival* measured
> at 86.7% with **0% silent corruption** across 60 references and 6 scenarios,
> so there is little headroom in surviving change and none in safety. The wedge
> is now **repair**: proposing the right face when a reference does break. The
> metrics below are updated accordingly; see `docs/decisions.md` D-011.
>
> The metric that decides the wedge is not the repair rate. It is the
> **mis-repair rate** — a confident wrong repair is worse than the honest
> breakage the baseline gives today.
>
> **Constrained again on 2026-09-14 by round 003 (D-012).** A geometric matcher
> scored 1.0 on the *wrong* feature after a resize, saved from mis-repairing
> only by an arbitrary margin threshold. Repair may therefore never be
> automatic: it diagnoses, ranks candidates, states its ambiguity, and the
> engineer confirms. Any confidence it reports must be the **margin** to the
> runner-up, never the match score.
>
> **Round 004 chose the disambiguator (D-014):** geometry plus exclusion of
> faces already claimed by a surviving reference. Provenance filtering was
> measured and rejected. Across rounds 002-004 the honest shape is: references
> rarely break; when they do, the best strategy proposes a correct candidate
> about half the time and a wrong one never, and abstains the rest of the time.
> Every one of those numbers is conditional on abstention thresholds that were
> guessed, which is the programme's current largest gap.

## Hipótesis

DiofanCAD puede ofrecer una ventaja material en ingeniería paramétrica cuando un diseño cambia de forma o topología.

## Workflow

Create part
→ add semantic references
→ create downstream dependency
→ change parameter
→ force topology change
→ recompute
→ inspect downstream state
→ repair if necessary
→ save/reload

## Métricas

- **mis-repair rate: repairs proposed confidently that point at the wrong face (primary; must be ~0)**
- **margin between the top candidate and the runner-up (this, not the score, is the confidence)**
- abstention rate: how often ambiguity is correctly declared instead of guessed
- acceptance rate: how often an engineer accepts a proposed candidate (unmeasured, needs humans)
- **reference count of the model under test** — round 004 showed repair quality depends on how richly referenced the model is, so a repair number without this is meaningless
- repair rate: broken references for which a single correct candidate is proposed
- candidate ambiguity: how many faces match the stored descriptor
- surviving references / total references
- preserved engineering intent
- successful recompute
- repair count
- repair time
- manual intervention count
- final correctness
- downstream artifact survival

## Falsación

La hipótesis se considera no demostrada si no se obtiene una mejora reproducible y material frente al baseline definido.
