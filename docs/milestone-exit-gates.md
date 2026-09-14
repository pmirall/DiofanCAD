# Milestone Exit Gates

Un milestone sólo sale cuando cumple simultáneamente tres barras.

## Product Bar

- workflow meets defined functional target
- benchmark is reproducible
- no unresolved critical failure

## Architecture Bar

- architecture proof passed
- no undocumented source-of-truth duplication
- migration and rollback understood
- performance impact measured

## Divergence Bar

- fork delta recorded
- changed subsystems known
- upstream candidate status recorded
- maintenance cost accepted explicitly

## Estados

- FAIL
- CONDITIONAL
- PASS

Un PASS puede estar acompañado de follow-up work, pero no de una violación no declarada de una barra crítica.
