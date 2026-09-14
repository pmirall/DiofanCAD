# DiofanCAD — Architecture Decisions

## Status

- Document class: Decision log
- Pass: 1
- Rule: decisions are provisional until evidence changes them.

## Decision template

```text
ID:
Date:
Decision:
Context:
Alternatives:
Evidence:
Consequences:
Revisit trigger:
Status:
```

## Initial decisions

### D-001 — Semantic layer is not allowed to become a duplicate CAD model

**Decision:** The semantic layer may own engineering meaning, identity and relationships, but must not duplicate authoritative geometry or parameters without a successful architecture proof demonstrating necessity.

**Status:** PROVISIONAL

**Rationale:** V7 makes engineering semantics the primary architectural bet while requiring the program not to scale on assumptions.

### D-002 — First competitive wedge is change resilience

**Decision:** The first proof-of-value is measured preservation of engineering intent under realistic parameter, topology and feature-order changes.

**Status:** ACTIVE

### D-003 — Architecture must be proven before scaled implementation

**Decision:** P0-A through P0-E are required before scaling the semantic architecture.

**Status:** ACTIVE

### D-004 — UNKNOWN is a valid engineering state

**Decision:** No baseline capability or competitive claim is promoted directly from UNKNOWN to SUPERIOR.

**Status:** ACTIVE

### D-005 — Permanent divergence is an exception

**Decision:** Permanent fork divergence requires explicit benefit, absence of acceptable upstream-compatible design, measured value, bounded burden, compatibility strategy, ownership and architecture approval.

**Status:** ACTIVE

### D-006 — DiofanCAD content is confined to native paths

**Decision:** DiofanCAD-native content lives only in `docs/`, `gauntlet/`, `tools/diofancad/` and `DIOFANCAD.md`. Upstream files are left untouched at the fork baseline, including the root `README.md`, which keeps FreeCAD's content.

**Status:** ACTIVE

**Rationale:** It makes divergence measurable. `tools/diofancad/divergence-report.sh` can then separate program overhead from real divergence in upstream code, and the first line of upstream code changed becomes a visible, deliberate event rather than a gradual drift. Adding a DiofanCAD pointer to the root `README.md` would cost a permanent conflict surface against every upstream README change; the discoverability gain did not justify it at baseline.

**Revisit trigger:** DiofanCAD becomes separately distributable, or the fork stops presenting itself as FreeCAD to users.

### D-007 — `main` is a clean upstream mirror

**Decision:** `main` tracks upstream and is fast-forward only. All DiofanCAD work happens on lab branches and is merged deliberately, never as unreviewed commits on the mirror.

**Status:** ACTIVE

**Rationale:** A mirror that cannot drift makes `upstream age` and `merge cost` cheap to compute and keeps the extraction pipeline viable. If `git merge --ff-only upstream/main` ever fails, that is a defect to explain, not to merge over.

### D-008 — First deliberate divergence into upstream code, taken for P0-C

**Decision:** P0-C modifies four upstream files (`Document.cpp`, two `CMakeLists.txt`, `SetupQt.cmake`) and adds three new ones. D-006's "zero upstream lines" baseline ends here, deliberately and on the record.

**Status:** ACTIVE

**Context:** Per-object recompute attribution cannot be done from outside. The observer-only alternative was investigated and rejected on a checkable fact, not a preference: there is no pre-object signal, so an observer cannot bound any single object's execution (`round-001-p0c/input.md`).

**Consequences:** 36 added lines in `Document.cpp`, all behind an inline enabled check, no statement removed or reordered. `SetupQt.cmake` is a separable U0 fix. The rest is U2 and extractable as a unit.

**Revisit trigger:** if `Document.cpp` hooks exceed roughly 100 lines, or if any hook needs to influence recompute rather than observe it, the design has drifted and needs architecture review.

### D-009 — The console-only build is the program's fast feedback loop

**Decision:** Core architecture proofs run against a console build (`BUILD_GUI=OFF`, all workbenches OFF) with `ENABLE_DEVELOPER_TESTS=ON`.

**Status:** ACTIVE

**Rationale:** `FreeCADApp` links only Base, Boost, fmt and Qt Core/Xml — no OCCT, no Coin3D, no PySide. The full `App_tests_run` suite builds and runs in minutes and executes in under a second, which makes measurement cheap enough to do before every claim. The proofs that matter for the wedge (references, semantics, recompute, save/reload) all live in `src/App` and `src/Base`.

**Consequences:** This configuration was broken at the baseline (U-001) and is untested by upstream CI, so the program depends on a config nobody else exercises. Reproduction is scripted in `gauntlet/rounds/architecture/round-001-p0c/reproduce.sh`.

**Revisit trigger:** a proof that genuinely needs geometry (P0-A on real topology, P0-E on a real part) will need `BUILD_PART=ON` and a heavier toolchain.

## Pending decisions

- Exact semantic identity serialization model.
- Exact ownership boundary between existing document model and new semantic metadata.
- Exact divergence budgets (`docs/upstream-sync.md` budget table is deliberately unset until real divergence data exists).
- Exact milestone numeric performance bars.
- Exact benchmark participant protocol.
