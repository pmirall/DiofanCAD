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

### D-010 — Do not build a new persistent-reference mechanism; reuse ElementMap

**Decision:** `App::ElementMap` / `MappedName` remain the identity substrate. DiofanCAD does not build a competing persistent-reference scheme.

**Status:** ACTIVE

**Evidence:** Round 002 (P0-A). 60 references across 6 change scenarios: 0 resolved to the wrong face, 86.7% resolved correctly, and all 8 failures were visible rather than silent. A replacement identity scheme has essentially no headroom: it cannot beat 0% corruption, and it would have to re-earn the 86.7%.

**Consequences:** Removes the largest speculative workstream from the roadmap. Work moves up the stack, to what the baseline genuinely lacks.

**Revisit trigger:** ambiguity or silent corruption found on harder geometry (fillets, revolutions, coplanar faces), or on edges and sketch external geometry, none of which round 002 measured.

### D-011 — The wedge is reference REPAIR, not reference survival

**Decision:** The first competitive wedge, set in D-002 as "change resilience", is sharpened to **repair of references after structural change**. D-002 stands; this narrows it.

**Status:** ACTIVE

**Evidence:** Survival is already good (86.7%) and already safe (0% silent corruption). Repair is absent: the baseline marks a broken reference `?` and stops, and almost nothing consumes even that. Every one of the 8 broken references had exactly one matching candidate — the information needed to propose a repair exists and is thrown away.

**Consequences:** `docs/first-wedge-benchmark.md` metrics shift from "surviving references / total" toward repair rate, repair correctness, and — the dangerous one — confidently wrong repairs. A repair layer that proposes a confident wrong answer is worse than today's honest breakage, so the primary metric of the next round is the mis-repair rate, not the repair rate.

**Revisit trigger:** if ambiguity on hard geometry makes confident repair unsafe, the wedge moves again rather than being forced.

### D-012 — Repair is never automatic; it proposes and the engineer confirms

**Decision:** A reference repair layer may diagnose, rank candidates and state its ambiguity. It may not apply a fix on its own.

**Status:** ACTIVE. Constrains D-011.

**Evidence:** Round 003, H4. With a hole deleted and re-created and the block lengthened, a scenario-blind geometric matcher proposed the **wrong hole with a perfect score of 1.0**; the intended one scored 0.958. A mis-repair was prevented only by a margin threshold of 0.05 against a measured margin of 0.042 — a number chosen with no principled basis.

**Consequences:**
- **Score is not confidence; the margin is.** Any confidence a repair reports must be the distance to the runner-up, never the match quality. A match quality of 1.0 was wrong.
- The 0.042 margin is an information problem, not a tuning problem: position, area and radius all agree with the wrong answer, so no reweighting fixes it.
- Repair is needed only when a generator changes, which is exactly when a geometric descriptor is least able to disambiguate. The cases that need repair are the cases where repair is least reliable.

**Revisit trigger:** if provenance (the naming grammar's tags and `ElementMap::getElementHistory()`) is shown to disambiguate what geometry cannot, automatic repair could be reconsidered for the cases provenance resolves — and only those.

### D-013 — The differentiation is diagnosis, not survival

**Decision:** The product claim moves from "preserves engineering intent through change better than the benchmark" toward "explains what broke and why, and offers candidates you confirm."

**Status:** PROVISIONAL — recorded because the evidence points here, not because the product direction has been decided.

**Evidence:** Across rounds 002 and 003, 68 tracked references: 60 resolved correctly, 8 broke visibly, **0 ever resolved to the wrong face**. On the hard geometry of round 003 the figure was 8/8 correct. The baseline already preserves intent well; there is little headroom in surviving more, and none in safety.

What the baseline does not do is explain. A broken reference is marked `?` and almost nothing consumes even that.

**Consequences:** This points at Model Doctor (V7 §19) as the wedge's product surface rather than a repair engine. `docs/first-wedge-benchmark.md` and the competitive framing need rework before any claim is made.

**Revisit trigger:** measurement on edges, sketch external geometry, or document-level recompute finding materially worse survival than faces showed.

## Pending decisions

- Exact semantic identity serialization model.
- Exact ownership boundary between existing document model and new semantic metadata.
- Exact divergence budgets (`docs/upstream-sync.md` budget table is deliberately unset until real divergence data exists).
- Exact milestone numeric performance bars.
- Exact benchmark participant protocol.
