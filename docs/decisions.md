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

**Decision:** DiofanCAD-native content lives only in `docs/`, `gauntlet/`, `tools/diofancad/`, `DIOFANCAD.md` and `CLAUDE.md`. Upstream files are left untouched at the fork baseline, including the root `README.md`, which keeps FreeCAD's content.

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

### D-014 — Exclusion by surviving references is the disambiguator; provenance is not

**Decision:** If a repair feature is built, it ranks candidates geometrically and then removes any face already claimed by a reference that still resolves. It does **not** filter candidates by originating tag.

**Status:** ACTIVE

**Evidence:** Round 004, 7 breaks across 5 scenarios, three strategies on identical inputs. Geometry alone repaired 3 correctly; geometry plus exclusion, 4; geometry plus a provenance filter, 1. No strategy ever mis-repaired.

Provenance lost both cases that geometry alone got right, because a replaced tool's tag no longer exists and the filter rejects every candidate. The generalisable reason: **provenance is not independent information about the broken case** — when the tag survives, the reference usually survives too; when the reference breaks because its generator changed, the tag has broken with it.

**Consequences:** Exclusion is free — no name parsing, no new persistent data, no change to `ElementMap`. But its power is borrowed from the document: R5 showed that with a single reference it degrades exactly to geometry alone. Any benchmark of a repair feature must therefore state how many references the model carried, or the number means nothing.

**Revisit trigger:** a case where an originating tag survives alongside genuine geometric ambiguity, which none of these scenarios produced. Provenance as a score bonus — rather than a filter — is untested and would be inert here.

### D-015 — Every measurement must report whether it was exercised

**Decision:** A harness reports not only what it found but whether the mechanism under test was actually invoked. A clean result from a measurement that never fired is recorded as `NOT EXERCISED`, never as a pass.

**Status:** ACTIVE

**Evidence:** Two rounds in three produced a clean result that meant nothing. Round 003's first ground truth scored legitimately moved faces as silent corruption and would have reported a serious FreeCAD defect. Round 005's M4 returned a perfect score on a string-table loss that never happened, because the save stored no string IDs — caught only by printing `hasher_entries_at_save`.

**Consequences:** Negative controls (round 003's N1 onward) prove the harness *can* fire. This adds the complement: proof that it *did*. Both are cheap and both have already changed a conclusion.

**Revisit trigger:** none expected; this is a floor, not a trade-off.

### D-016 — Establish where a mechanism is active before measuring how well it works

**Decision:** Before any further measurement of the element map, establish which document features and workbenches actually populate one. No round may characterise a mechanism's quality until its coverage is known.

**Status:** ACTIVE

**Evidence:** Round 007 set out to check coverage and got the answer wrong, which makes the case for the decision rather than against it. Its probe reported no feature carrying an element map; corrected, `Part::Cut` carries 50 entries and `Part::Fillet` 32, while primitives carry none. Coverage varies by feature, it was never established, and four rounds of detailed measurement ran without anyone knowing it.

**Consequences:** A coverage probe belongs at the start of any round that measures a mechanism's quality — and per D-018 it must be able to prove it can see the mechanism when present.

**Revisit trigger:** none; this is a floor. The cost of asking "is this thing on?" is three lines, and the cost of not asking was four rounds of ambiguous relevance.

### D-017 — A measurement error is more likely to manufacture a finding than to hide one

**Decision:** Treat any measurement that produces a dramatic result as suspect until the instrument has been checked, and check it before writing the result up.

**Status:** ACTIVE

**Evidence:** Three instances in seven rounds, all biased the same way. Round 003's ground truth scored legitimately moved faces as silent corruption — a serious FreeCAD defect that did not exist. Round 005's M4 reported a clean string-table loss that had never been exercised. Round 007's M2 reported a fillet jumping to a different corner when it had followed correctly, because the expectation compared a cylindrical face centre against corner coordinates.

Each error pointed towards a *more* dramatic conclusion than the truth. None pointed the other way.

**Consequences:** The negative control (does the harness fire?) and the exercised check (did the mechanism run?) are both mandatory, and neither is sufficient — all three errors above passed one or the other. The remaining defence is suspicion proportional to how good the result would be for the programme.

### D-018 — A probe reporting absence must first prove it can detect presence

**Decision:** When a measurement concludes that a mechanism is absent, missing, empty or zero, it must include a **positive control**: the same probe applied to a case where the mechanism is known to be present, showing a non-zero result.

**Status:** ACTIVE

**Evidence:** Round 007's coverage probe reported `hasElementMap()` false for every feature and was believed, committed, pushed and reported. It was measuring a temporary `TopoShape` constructed from `Shape.getValue()`, which drops the element map. A positive control — any shape known to carry a map — would have shown zero as well and exposed the probe immediately.

**Why the existing rules did not catch it:** D-015 requires a measurement to report whether it was *exercised*, and this one genuinely ran. Negative controls (round 003 onward) prove a harness can report a *difference*; they say nothing about whether it can see a *mechanism*. This is the missing third control.

**Consequences:** Three controls now, and they answer different questions — can the harness report a difference (negative control), did the mechanism run (exercised check), and can the probe see the mechanism at all (positive control).

### D-019 — Two independent routes before any headline claim

**Decision:** No headline claim is written down until it has been established twice, by routes that could not fail the same way. Route 1 is the harness; route 2 must be structurally different — reading the source that implements it, computing the expected value by hand, measuring an independent quantity that must move with the claim, or inverting the test so the opposite result is what gets asserted.

**Status:** ACTIVE. This is the methodology change made after round 007's retraction.

**Evidence:** Eight rounds, four wrong results, all defects in this project's instrument and all biased towards a more dramatic conclusion. The controls added after each one were reactive: the negative control (after 003) proves a harness can report a difference, the exercised check (after 005) proves the mechanism ran, the positive control (after 007) proves the probe can see the mechanism. Each closed the previous error's class and the next error opened a new one.

What actually caught all four was the same thing every time, and it was never a control: physical reasoning (003), an independent quantity (005), hand arithmetic (007 M2), source reading (007 P1). Each was an independent route, and each was taken *after* the write-up rather than before.

**Consequences:** Rounds cost more. `gauntlet/protocols/round-template.md` now carries a pre-flight checklist covering all three controls plus the second route, and a round that cannot name its second route is not ready to run. Scrutiny is explicitly asymmetric (D-017): a result that favours the programme needs the second route and a written "what would make this wrong?"; a result that says the baseline is fine needs less, because that is the direction the errors do not go.

**Revisit trigger:** a fifth measurement error of the same shape would mean this rule is also insufficient, and the next step would be an independent reviewer rather than another self-applied control.

## Pending decisions

- Exact semantic identity serialization model.
- Exact ownership boundary between existing document model and new semantic metadata.
- Exact divergence budgets (`docs/upstream-sync.md` budget table is deliberately unset until real divergence data exists).
- Exact milestone numeric performance bars.
- Exact benchmark participant protocol.
- **Principled abstention thresholds.** Every repair result in rounds 003-004 is conditional on `score >= 0.50` and `margin >= 0.05`, both chosen rather than derived. This is the current largest gap.
