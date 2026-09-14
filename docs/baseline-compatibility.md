# DiofanCAD — Baseline Compatibility

## Status

- Document class: Compatibility baseline
- Pass: 1
- Evidence status: INITIAL / TO VALIDATE

## Compatibility contract

Where practical, protect:

- existing FreeCAD files
- STEP
- IGES
- STL
- DXF
- IFC
- Python API
- macros
- addons
- workbench ecosystem

Intentional breaking changes require justification, migration, documentation, tests and explicit status.

## Compatibility dimensions

Compatibility must be evaluated separately for:

```text
OPEN
READ
EDIT
RECOMPUTE
SAVE
ROUND-TRIP
CROSS-VERSION
CROSS-BRANCH
DOWNGRADE / RETURN TO FREECAD
```

## Initial matrix

| Surface | Open | Read | Edit | Recompute | Save | Round-trip | Evidence |
|---|---|---|---|---|---|---|---|
| Native FreeCAD files | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TBD |
| STEP | UNKNOWN | UNKNOWN | N/A | N/A | N/A | UNKNOWN | TBD |
| IGES | UNKNOWN | UNKNOWN | N/A | N/A | N/A | UNKNOWN | TBD |
| STL | UNKNOWN | UNKNOWN | N/A | N/A | N/A | UNKNOWN | TBD |
| DXF | UNKNOWN | UNKNOWN | N/A | N/A | N/A | UNKNOWN | TBD |
| IFC | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | TBD |
| Python API | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | N/A | UNKNOWN | TBD |
| Macros | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | N/A | UNKNOWN | TBD |
| Addons | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | N/A | UNKNOWN | TBD |
| Workbenches | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | N/A | UNKNOWN | TBD |

## Semantic migration requirement

For any new persistent semantic identity, define:

- serialization format;
- behavior when opened by older FreeCAD;
- behavior when reopened by DiofanCAD;
- behavior after topology changes;
- migration strategy;
- corruption / partial-read behavior;
- downgrade expectations.

## Compatibility gate

No architecture claim of “compatible” is valid until the corresponding dimension has an executable test and evidence artifact.
