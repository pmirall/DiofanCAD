# Architecture Proof Experiments

> Resumen. La definición operativa de cada proof — hipótesis, implementación mínima,
> condición de falsación y evidencia requerida — vive en
> [`gauntlet/protocols/architecture-proofs.md`](../gauntlet/protocols/architecture-proofs.md).
> Ese archivo manda si los dos divergen.

## P0-A Persistent Reference Prototype

Probar identidad persistente + mapping topológico + confidence + diagnóstico en un modelo pequeño con cambios de topología.

## P0-B Semantic Object Prototype

Demostrar un objeto semántico que referencia la fuente geométrica/paramétrica existente sin crear una segunda fuente de verdad.

## P0-C Recompute Instrumentation

Instrumentar dependency propagation, invalidation y recompute para medir coste y observabilidad.

## P0-D Save/Reload

Verificar persistencia de identidad y relaciones después de save/reload.

## P0-E Change Resilience

Ejecutar Sketch → Part → Modify → Change challenge y medir qué intención y referencias sobreviven.

## Resultado requerido

Cada experimento debe terminar en PASS / FAIL / UNKNOWN con evidencia. No implementar una plataforma completa a partir de un único prototipo.
