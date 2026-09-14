# Gauntlet Round Template

Copy this into `rounds/<workstream>/round-XXX/` and fill it in. The pre-flight
section is filled in **before** the round runs.

## Pre-flight — complete before measuring

A round that cannot answer these is not ready to run. See `CLAUDE.md` §1.

```text
[ ] The claim this round could make, stated in one sentence
[ ] ROUTE 2: how the claim will be established independently of the harness
        (source reading / hand-computed value / independent quantity /
         inverted test — name which, and what it will show)
[ ] NEGATIVE control: the deliberately wrong expectation the harness must report
[ ] POSITIVE control: where the mechanism is known present, so the probe proves
        it can see it. Mandatory if any result could be "absent/zero/none/clean"
[ ] EXERCISED check: the counter that shows the mechanism actually ran
[ ] Would this result be GOOD for the programme? If yes, the second route is
        mandatory and so is the "what would make this wrong?" answer below
[ ] What would make this wrong?
```

## Metadata

```text
round_id:
workstream:
commit:
build:
hardware:
dataset:
operator:
critic:
```

## Bar

```text
task:
starting_state:
target_state:
constraints:
metrics:
tolerance:
critical_failures:
```

## Prediction

Recorded before measuring, so the measurement can contradict it.

```text
prediction:
what would refute it:
```

## Evidence

```text
artifacts:
logs:
raw output (re-read before writing the verdict, per CLAUDE.md 1.4):
benchmark:
```

## Verification — complete after measuring, before the verdict

```text
[ ] Raw numbers re-read; are they physically possible?
[ ] Route 2 executed, and it agrees
[ ] All three controls behaved as required
[ ] Anything dramatic re-checked against the instrument first
```

## Verdict

```text
PASS | FAIL | UNKNOWN
Largest Gap:
Scope — what this does NOT establish:
```
