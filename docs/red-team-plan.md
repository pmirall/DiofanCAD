# Red Team Plan

## Ataques mínimos

- dimension changes
- topology changes
- feature reorder
- suppression
- dependency deletion
- component replacement
- configuration changes
- unit changes
- save/reload
- external dependency failures
- large models
- pathological constraints
- assembly modifications

## Para cada ataque

Registrar:

```text
input state
mutation
expected behavior
observed behavior
failure category
data loss?
recoverable?
recovery cost
evidence bundle
```

## Principio

Un workflow que pasa el caso nominal pero falla bajo una mutación relevante no debe marcarse como robusto.
