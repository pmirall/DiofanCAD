# Start here

You are continuing an engineering programme on a FreeCAD fork. Read this file,
then `CLAUDE.md` section 0. Nothing else is required before you can work.

## 1. Get it building (one command)

```sh
sudo tools/diofancad/bootstrap.sh build
```

45–90 minutes the first time on 4 cores. When it finishes it prints the test
results. **Expected: Part suite 331/331 passing, App suite 3 failing.**

Those 3 App failures are old and unexplained — they are not yours:

```
DocumentObjectTest.getSubObjectList
MoveProperty.staticProperty
PropertyExpressionEngineTest.executeCrossPropertyReference
```

If you see ~125 Part failures instead, the resource links are missing:
`tools/diofancad/setup-test-resources.sh build`

## 2. What this project has established

Ten rounds, all measuring FreeCAD's element-map identity system. The short
version:

- Derived features (`Part::Cut`, `Part::Fillet`) carry element maps. Primitives (`Part::Box`) do not.
- A reference stores a mapped name **iff** the feature it points at has a map. Population follows the target.
- References survive parameter changes, topology changes, save/reload. **0 silent corruption in 68 tracked references.**
- When a reference does break, a geometric matcher can propose the right candidate about half the time and a wrong one never — but only because it abstains readily, on thresholds that were guessed.
- Exactly 2 features store raw indices instead of mapped names: `Part::Fillet` and `Part::Chamfer`.

Full record: `docs/evidence-ledger.md` (34 entries), `docs/decisions.md` (20
decisions), `gauntlet/rounds/`.

## 3. What has NOT been done

**No competitive claim exists.** `docs/fusion-benchmark-plan.md` has never been
run. No golden workflow has been executed. Nothing has been compared against
Fusion 360 or any other CAD system.

This is the programme's largest gap and it is recorded as such in
`gauntlet/state/master.json`. Ten rounds characterised a mechanism; none
measured anything a user would notice.

## 4. The decision waiting for you

Round 011 should be one of two things, and the choice is a judgement call a
human should make:

**(a) Run a golden workflow end to end.** `docs/golden-workflow-suite.md` lists
thirteen; the bracket is the simplest. Model it, change it, measure the time
and the failures. First thing in this repo a user would recognise.

**(b) Stop, explicitly.** Record a kill decision against the charter's §77.
The comparison the charter demands needs Fusion 360 access and human testers.
Ten rounds produced no plan to obtain either. Choosing (b) is a legitimate
engineering outcome, not a failure — see `docs/kill-criteria.md`.

Do not drift into an eleventh round of measuring the element map. That is the
comfortable option and it is why this gap is now the largest one.

## 5. How to do a round

```sh
cp gauntlet/protocols/round-template.md gauntlet/rounds/<area>/round-011-<name>/bar.md
# fill in the PRE-FLIGHT section BEFORE running anything
# ... measure ...
# ... write the verdict ...
tools/diofancad/check-round.sh gauntlet/rounds/<area>/round-011-<name>
```

The checker returns non-zero if the round is missing its controls. Run it
before committing.

It was validated by running it against round 007 — the round that was later
retracted — where it reports exactly the defect that caused the retraction:
*"verdict makes an absence claim with no positive control declared"*.

Rounds 001–008 fail it because they predate the rules; that is expected, and
they are not being back-filled. Round 009 failed it too, for a real omission
(no prediction recorded), which is now noted in that round rather than
retroactively fixed. Round 010 passes.

## 6. Rules that are not optional

Four of ten rounds produced a **wrong result**, every one a bug in this
project's own test code, every one pointing at a more dramatic conclusion than
the truth. One reached a commit and a push before being caught.

So:

1. **Two independent routes before any headline claim.** The harness is one. The second must be different in kind: read the source, compute the number by hand, or measure something else that must move with it.
2. **A result of "absent / zero / none" needs a positive control** — proof the probe can see the thing when it *is* there. This is the exact check round 007 skipped.
3. **Never write the verdict in the same step as the measurement.** Re-read the raw numbers first and ask whether they are physically possible.
4. **The more a result flatters this project, the more evidence it needs.**

`CLAUDE.md` section 1 has the full version with the history behind each rule.

## 7. Things that will waste your time if you do not know them

All in `CLAUDE.md` section 2. The two that cost the most:

- `Shape.getValue()` returns a `TopoDS_Shape`; assigning it to a `TopoShape` **silently drops the element map**. Use `Shape.getShape()`. This caused the round 007 retraction.
- `src/App/Document.cpp` is CRLF. Edit it with `newline=''` in Python or the whole file reformats.

## 8. Repository layout

| Path | What |
|---|---|
| `docs/` | The programme: baselines, decisions, evidence ledger, upstream strategy |
| `gauntlet/` | Rounds, protocols, current state (`state/master.json`) |
| `tools/diofancad/` | bootstrap, test resources, round checker, divergence report |
| everything else | Upstream FreeCAD. 13 files changed, all additive tests plus one 5-line CMake fix |

Run `tools/diofancad/divergence-report.sh main HEAD` after touching `src/`,
`tests/` or `cMake/`, and paste the output into `docs/upstream-sync.md`.

## 9. Two things worth sending upstream

Both are independent of this project and ready:

- **U-001** — `BUILD_GUI=OFF` cannot configure at all without the `SetupQt.cmake` fix (already applied here).
- **U-003** — a build tree cannot run its own tests; three resource paths are missing and nothing says so.

See `docs/upstream-backlog.md`.
