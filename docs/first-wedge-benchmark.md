# First Wedge Benchmark — Change Resilience

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
