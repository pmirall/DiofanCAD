# Kill Criteria

## Objetivo

Evitar que el proyecto sostenga indefinidamente ideas cuyo coste supera su valor demostrado.

## Señales de kill / pivot

- arquitectura no puede pasar architecture proof gate
- mejora competitiva no aparece después de iteraciones definidas
- coste de divergencia supera el umbral aprobado
- mantenimiento crece desproporcionadamente respecto al uso
- compatibilidad crítica se rompe sin justificación suficiente
- evidencia no puede reproducirse
- existe una alternativa claramente más simple con resultado equivalente

## Procedimiento

1. Registrar fallo dominante.
2. Separar causa de síntoma.
3. Comparar alternativas.
4. Decidir KEEP / PIVOT / DROP.
5. Si DROP, documentar qué se conserva y qué se elimina.

La finalidad del kill criterion es ahorrar desarrollo, no castigar experimentos fallidos.
