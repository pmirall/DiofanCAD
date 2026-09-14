# Master Integration Plan

## Pipeline

Sketch
→ Part
→ Assembly
→ Drawing
→ BOM
→ CAM
→ Revision
→ Export
→ Reload

## Gate

Module-level PASS no implica product-level PASS.

## Verificaciones

- semantic identity survives downstream use
- references remain valid or produce explicit repair diagnostics
- save/reload preserves required state
- BOM and drawing agree with released revision
- manufacturing outputs identify their input revision
- exported artifact reloads or round-trips according to compatibility contract
