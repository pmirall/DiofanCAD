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
