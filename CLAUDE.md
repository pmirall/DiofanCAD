# DiofanCAD — working agreement

## 0. If you read nothing else

New here? Read [`HANDOFF.md`](HANDOFF.md) first. It is shorter than this file
and tells you what to do.

Six rules, in order of how much trouble breaking them has caused:

1. **Two independent routes before any headline claim.** The test harness is one route. The second must differ in kind — read the source, compute the number by hand, or measure something else that must move with it.
2. **"Absent / zero / none / empty" needs a positive control** — proof the probe sees the thing when it *is* present. Skipping this caused a retraction.
3. **Write the verdict in a separate step from the measurement.** Re-read the raw numbers first and ask if they are physically possible.
4. **The more a result flatters this project, the more evidence it needs.** All four past errors pointed at a more dramatic conclusion than the truth. None pointed the boring way.
5. **Use `Shape.getShape()`, never `Shape.getValue()`**, when you care about element maps. `getValue()` silently drops them.
6. **Run `tools/diofancad/check-round.sh <round-dir>` before committing a round.** Non-zero exit means it is not ready.

Everything below is the reasoning behind those. Sections 1 and 2 are the ones
worth reading in full.

---

This repository is a FreeCAD fork run as an evidence-driven engineering
programme. The charter is `docs/spec/diofancad-fusion-killer-spec-v7.md`; the
control system is `docs/`, `gauntlet/` and `tools/diofancad/`. Start at
[`DIOFANCAD.md`](DIOFANCAD.md) and `gauntlet/state/master.json`.

Almost everything here is measurement. So the rules that matter most are about
how to measure without fooling yourself.

---

## 1. The measurement rules

Eight rounds produced **four** wrong results. Every one was a defect in this
project's own instrument, and every one pointed towards a *more dramatic*
conclusion than the truth:

| Round | The instrument's flaw | It would have claimed |
|---|---|---|
| 003 | ground truth required an exact plane match | a silent-corruption defect in FreeCAD |
| 005 | the string-table case never ran | a clean pass on an untested question |
| 007 | compared a cylindrical face centre to corner coordinates | a fillet jumping corners |
| 007 | `getValue()` drops the element map | element maps absent from all documents |

The last one was committed, pushed and reported before being caught.

### 1.1 Two independent routes — the rule that would have caught all four

**No headline claim is written down until it has been established twice, by
routes that could not fail the same way.**

Route 1 is the harness. Route 2 must be structurally different:

- read the source that implements the thing;
- compute the expected number by hand;
- measure an independent quantity that must move with the claim;
- invert the test so the opposite result is what gets asserted.

This is not aspirational — it is how all four errors were actually found:
physical reasoning (003), an independent quantity (005), hand arithmetic (007
M2), source reading (007 P1). The fix is to do it *before* the write-up rather
than afterwards.

### 1.2 Three controls, declared in the bar before the round runs

They answer different questions and none substitutes for another:

| Control | Proves | Blind to |
|---|---|---|
| **Negative** — assert a deliberately wrong expectation | the harness can report a difference | whether it can see the mechanism |
| **Positive** — run the probe where the mechanism is known present | the probe can detect the mechanism | whether it ran in this case |
| **Exercised** — report a counter showing the mechanism ran | it ran here | whether the reading is meaningful |

A result of "absent", "zero", "none" or "clean" without a positive control is
not a finding. It is an untested probe.

### 1.3 Scrutiny proportional to how much you want the result

The bias is one-directional: errors here manufacture findings, they do not hide
them. So the bar is asymmetric, deliberately.

A result that is **good for the programme** — a defect in the baseline or a
competitor, a dramatic gap, a confirmation of the thesis — needs the second
route *and* an explicit written answer to "what would make this wrong?" before
it is written up.

A result that says the baseline is fine needs less. It is the direction the
errors do not go.

### 1.4 Never measure and conclude in the same step

Running the measurement and writing the verdict are separate actions with
verification between them. Every one of the four errors was visible in the raw
output; three were missed because the write-up followed immediately.

Before writing a verdict, re-read the raw numbers and ask whether they are
physically possible.

### 1.5 The record keeps its mistakes

A failed or retracted result stays in `docs/evidence-ledger.md`, struck through
rather than deleted, with a pointer to the correction. Retractions go in a
`CORRECTION.md` in the round's directory and a banner at the top of its
verdict. See `gauntlet/rounds/architecture/round-007-p0e/`.

State what was measured, in what configuration, and what was not. Scope claims
narrowly enough that they survive contact with the next round.

---

## 2. Traps in this codebase, paid for once

**`PropertyPartShape` has two accessors and they are not equivalent.**
`getValue()` returns a `TopoDS_Shape`; binding it to a `TopoShape` parameter
constructs a new one and **silently drops the element map**. `getShape()`
returns the stored `TopoShape`. This caused the round 007 retraction.

**Element map coverage varies by feature.** Primitives carry none —
`Part::Box::execute()` assigns a raw `TopoDS_Shape`. Derived features do:
`Part::Cut` ~50 entries, `Part::Fillet` ~32. Probe before interpreting.

**`Part::Fillet` stores raw edge indices.** `FilletElement { int edgeid; double
radius1, radius2; }` — no name, no shadow. A consumer built this way cannot
benefit from the identity layer however good it is.

**`ComplexGeoData::elementMap()` is protected.** The public route is
`resetElementMap()`, which detaches the map and returns it; do it on a copy.

**`src/App/Document.cpp` is CRLF.** Edit with `newline=''` or the whole file
reformats and the diff becomes useless.

**A build tree cannot run its own tests** until
`tools/diofancad/setup-test-resources.sh <build-dir>` creates
`<build>/share/Mod/Material/Resources`, `<build>/tests/brepfiles` and
`<build>/tests/TestModels`. Without them 125 of 331 Part tests fail in ways that
look like code defects. Queued upstream as U-003.

**PartDesign does not work in this console configuration.** 0 of 20 tests pass:
the 16 that create a `PartDesign::Body` all throw a non-`std` exception in
`SetUp()`, and `BackwardCompatibilityTest` segfaults opening a saved model.
Cause unknown — do not assume it is another resource path without measuring.
Any round needing a `Sketch → Pad` chain is blocked on this.

**Build configuration:** `BUILD_GUI=OFF` needs the `SetupQt.cmake` fix (U-001,
already applied here). `BUILD_PART=ON` requires `BUILD_MATERIAL=ON`. OCCT needs
`libtbb-dev` at link time. `BUILD_SKETCHER=ON` needs `swig`. Full dependency
lists are in each round's `reproduce.sh`.

---

## 3. Divergence

DiofanCAD-native content lives only in `docs/`, `gauntlet/`,
`tools/diofancad/`, `DIOFANCAD.md` and `CLAUDE.md`. Everything else is upstream
FreeCAD, and every line changed there is a cost.

Run `tools/diofancad/divergence-report.sh` after any change that touches `src/`,
`tests/` or `cMake/`, and record the output in `docs/upstream-sync.md` — copied
from the tool, never summarised from memory.

Prefer a test file over a `src/` change. A proof that might conclude "build
nothing" should not have built something first.

---

## 4. Decisions

Decisions live in `docs/decisions.md` with evidence and a revisit trigger. The
ones that constrain day-to-day work:

- **D-010** reuse `ElementMap`; do not build a competing reference mechanism.
- **D-012** repair is never automatic — it proposes, the engineer confirms. Score is not confidence; the margin is.
- **D-015** every measurement reports whether it was exercised.
- **D-016** establish where a mechanism is active before measuring how well it works.
- **D-017** a measurement error is more likely to manufacture a finding than to hide one.
- **D-018** a probe reporting absence must first prove it can detect presence.
- **D-019** two independent routes before any headline claim.

Nothing here claims superiority over anything. No benchmark has been run
against Fusion 360, and `UNKNOWN` is the honest status of most questions the
charter asks.
