# DiofanCAD Gauntlet

The Gauntlet is the program's reproducible evaluation system. It is the reason a DiofanCAD claim can be trusted, and the mechanism that stops the program from scaling on assumptions.

## Golden rule

The conversation is not the source of truth. The versioned artifacts in this repository are.

A run that left no artifact did not happen.

## Operating rule

```text
PROVE
 ↓
MEASURE
 ↓
DECIDE
 ↓
SCALE
```

The inverse flow is forbidden:

```text
DESIGN
 ↓
ASSUME
 ↓
IMPLEMENT
 ↓
DEFEND
```

## Structure

```text
gauntlet/
  README.md                    this file
  state/master.json            current program state; the single status record
  protocols/
    master-flow.md             the OBSERVE → ... → UPSTREAM GAUNTLET cycle
    round-template.md          the form every round fills in
    architecture-proofs.md     the P0-A … P0-E proof set that gates the semantic bet
  workflows/                   golden workflow definitions (bar, fixtures, change challenge)
  rounds/<workstream>/round-XXX/
    bar.md input.md output.md benchmark.json critic-*.md
    verdict.md largest-gap.md changes.md evidence/
```

## Running a round

Complete the pre-flight checklist in `protocols/round-template.md` first. It
exists because eight rounds produced four wrong results, every one a defect in
this project's own instrument and every one biased towards a more dramatic
conclusion. `CLAUDE.md` §1 has the rules; the short version is that a headline
claim needs **two independent routes** and three controls, and none of it is
optional for a result that would flatter the programme.

1. Read the bar in `workflows/` — or write it first, before building anything.
2. Create `rounds/<workstream>/round-XXX/` from `protocols/round-template.md`.
3. Record the baseline before the change, not after.
4. Build, then hand the artifact to a **fresh** critic that has not seen the builder's rationale.
5. Record one `largest-gap.md`: a single sentence naming the dominant failure.
6. Update `state/master.json` and, when a claim moves, `docs/evidence-ledger.md`.

A verdict is `PASS`, `FAIL` or `UNKNOWN`. There is no fourth option, and `UNKNOWN` never becomes `SUPERIOR` in one step.

## Current state

`UNKNOWN`. No baseline has been measured, no proof has been run, and no superiority claim exists. See `state/master.json`.
