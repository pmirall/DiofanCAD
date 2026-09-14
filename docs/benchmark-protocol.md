# Benchmark Protocol

## Preparación

1. Congelar versiones.
2. Definir hardware y entorno.
3. Preparar condiciones iniciales idénticas o la equivalencia documentada.
4. Fijar target state y constraints.
5. Registrar versión, build, configuración y dataset.

## Ejecución

1. Ejecutar workflow.
2. Registrar tiempo.
3. Registrar interacciones cuando sea medible.
4. Registrar errores y recuperación.
5. Verificar estado final.
6. Guardar y recargar.
7. Aplicar change challenge.
8. Ejecutar Model Doctor o equivalente cuando exista.
9. Ejecutar performance checks.
10. Generar evidence bundle.

## Rendimiento

Separar:

- p50/p95 para operaciones interactivas;
- wall-clock para operaciones batch;
- peak memory;
- time-to-first-visible en ensamblajes grandes.

Las regresiones deben compararse contra un baseline versionado, no contra impresiones subjetivas.

## Reproducibilidad

Cada run registra al menos:

- run_id
- commit/build
- hardware
- software versions
- dataset ID
- configuration
- seed cuando aplique
- timestamp
