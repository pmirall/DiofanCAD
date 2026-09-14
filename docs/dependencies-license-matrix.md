# DiofanCAD — Dependency / License Matrix

## Purpose

Track dependency, distribution, modification, security, and maintenance implications before introducing new architectural components.

## Status

`UNKNOWN — repository/dependency manifests require inspection`

## Matrix

| Dependency | Version | License | Linking / distribution | Modification rights | Security | Maintenance | Replacement path | Risk | Evidence |
|---|---|---|---|---|---|---|---|---|---|
| FreeCAD | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | UNKNOWN | repository/license review |
| Geometry kernel | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | UNKNOWN | dependency audit |
| Sketch solver | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | UNKNOWN | dependency audit |
| Assembly dependencies | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | UNKNOWN | dependency audit |
| CAM dependencies | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | UNKNOWN | dependency audit |
| FEM dependencies | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | UNKNOWN | dependency audit |
| ECAD interoperability | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | TO VALIDATE | UNKNOWN | dependency audit |

## Rules

1. Verify actual license text and distribution implications; do not infer from package names.
2. Record transitive dependencies when they materially affect distribution.
3. Re-run the audit when versions change.
4. Security and replacement risk are first-class fields, not afterthoughts.
5. A license-compatible dependency is not automatically a maintenance-compatible dependency.

## Exit criteria

No new dependency becomes committed architecture while its license/distribution status is `UNKNOWN`.
