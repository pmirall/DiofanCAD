# DiofanCAD — Baseline Architecture

## Status

- Document class: Reality Map / Baseline
- Pass: 1 (component map located at fork baseline; behaviour still unmeasured)
- Evidence status: LOCATED / NOT YET MEASURED
- Source charter: `docs/spec/diofancad-fusion-killer-spec-v7.md`
- Baseline commit: see `docs/upstream-sync.md`

## Purpose

Establish the current architectural reality before substantial implementation. This document is not a target architecture and must not be used to justify changes that have not been measured.

## Architectural north star from V7

DiofanCAD targets the following conceptual flow:

```text
USER INTENT
    ↓
CONTEXT ENGINE
    ↓
SEMANTIC MODEL
    ↓
CAD / Assembly / Drawing / CAM / FEM / ECAD
    ↓
ENGINEERING DATA
    ↓
AGENT / API LAYER
```

The intended property is: internally modular, externally integrated.

## Current architecture map

The `Location` column is the result of repository inspection at the fork baseline commit and may be treated as fact. Every other column is an evidence slot: `Behaviour status` stays `UNKNOWN` until the subsystem has been read, instrumented or benchmarked, and `Divergence risk` stays `TBD` until a DiofanCAD change is actually proposed against it. Locating code is not the same as understanding it.

| Layer / subsystem | Location (measured) | Behaviour status | Evidence needed | Divergence risk |
|---|---|---|---|---|
| Geometry kernel | OCCT, consumed via `src/Mod/Part` | UNKNOWN | version/capability audit | TBD |
| Document / model object layer | `src/App` (`Document`, `DocumentObject`, `Property*`) | UNKNOWN | read + instrument | TBD |
| Transactions / undo | `src/App/Transactions.*`, `AutoTransaction.*`, `TransactionalObject.*` | UNKNOWN | read + P0-B/P0-D | TBD |
| Topological naming / element identity | `src/App/ElementMap.*`, `MappedName.*`, `MappedElement.*`, `ElementNamingUtils.*` | UNKNOWN | read + P0-A | TBD |
| Dependency / recompute | `src/App/Document.cpp` (boost adjacency_list + topological sort) | UNKNOWN | read + P0-C instrumentation | TBD |
| Persistence | `src/Base/Persistence.*`, `Reader.*`, `Writer.*`, `XMLParser.*`; zipios++ container | UNKNOWN | read + P0-D round-trip | TBD |
| Sketcher / constraint solving | `src/Mod/Sketcher` (planegcs solver) | UNKNOWN | read + solver benchmark | TBD |
| Part Design | `src/Mod/PartDesign` | UNKNOWN | read + change challenge | TBD |
| Assembly | `src/Mod/Assembly` + `src/3rdParty/OndselSolver` | UNKNOWN | read + large-assembly fixture | TBD |
| TechDraw | `src/Mod/TechDraw` | UNKNOWN | read + associativity test | TBD |
| CAM | `src/Mod/CAM` | UNKNOWN | read + post/verification audit | TBD |
| FEM / CAE | `src/Mod/Fem` | UNKNOWN | read + provenance audit | TBD |
| Surface / freeform | `src/Mod/Surface`, `src/Mod/ReverseEngineering` | UNKNOWN | read | TBD |
| ECAD integration | no first-party module at baseline; via addons / `src/Mod/Import` | UNKNOWN | ecosystem survey | TBD |
| Import / export | `src/Mod/Import` (STEP/IGES), `src/Mod/Mesh` (STL), `src/Mod/Draft` (DXF), `src/Mod/BIM` (IFC) | UNKNOWN | round-trip matrix | TBD |
| Python / plugin API | `*.pyi` + `*PyImp.cpp` across `src/App`, `src/Base`, `src/Gui`, `src/Mod`; `src/Tools/typing/stubgen` | UNKNOWN | API surface inventory | TBD |
| GUI / command routing | `src/Gui` (`Command*`, workbench registration) | UNKNOWN | read + Context Engine study | TBD |
| Testing / CI | `tests/` (C++ gtest), `src/Mod/Test` (Python), `.github/workflows/CI_primary.yml` | UNKNOWN | `docs/baseline-test-health.md` | TBD |
| Packaging / release | `CMakeLists.txt`, `CMakePresets.json`, `pixi.toml`, `package/`, `.github/workflows/build_release.yml` | UNKNOWN | build reproduction | TBD |

### How to use this table

An `UNKNOWN` in `Behaviour status` is a research task, not a claim of absence. A located subsystem may already implement a capability the program is tempted to rebuild; V7 §76 requires an explicit REUSE / BUILD / INTEGRATE / DEFER decision against these locations before any implementation starts.

## Semantic architecture hypothesis

V7 identifies engineering semantics as the primary architectural bet. A semantic object may expose:

```text
identity
meaning
geometry
parameters
relationships
references
metadata
domain-specific properties
```

This is a hypothesis, not permission to create a parallel source of truth.

### Architecture invariant

A new semantic layer must explicitly state:

1. what it owns;
2. what it references;
3. what it derives;
4. what it must never duplicate.

### Single-source-of-truth rule

Do not create a second authoritative geometry or parameter model unless an architecture proof demonstrates that the existing model cannot satisfy the required behavior.

The baseline already ships an element-identity mechanism (`src/App/ElementMap.*`) and a dependency graph (`src/App/Document.cpp`). Any proposal for DiofanCAD persistent identity or dependency tracking must first state why these cannot be extended, per `docs/architecture-boundary.md`.

## Required architecture proofs

Before scaling the semantic architecture, produce:

- P0-A persistent-reference prototype
- P0-B semantic-object prototype
- P0-C recompute instrumentation prototype
- P0-D save/reload prototype
- P0-E realistic change-challenge prototype

Each proof must contain hypothesis, minimum implementation, expected result, falsification condition, measured result and decision. See `gauntlet/protocols/architecture-proofs.md`.

## Architecture boundary questions to answer in implementation reality

- Which structures are authoritative for geometry?
- Which structures are authoritative for parameters?
- Where is persistent identity stored?
- How is semantic identity serialized?
- Where are references resolved?
- What layer owns repair and confidence scoring?
- What layer owns dependency invalidation?
- What can be added without changing existing file semantics?
- Which changes are fork-only and which are intended for upstream extraction?

## Exit criteria for this baseline

PASS only when the repository evidence supports a concrete component map **and** each critical semantic assumption has an owner and a persistence boundary. Locating the files satisfies neither condition on its own; this document remains open until the `Behaviour status` column is populated from reading and measurement.
