# Round 006 — Unblocking Document-Level Testing — Verdict

## The blocker was never a defect. The Part suite is fully green.

Round 002 recorded 125 of 312 Part tests failing with `Material not found` and
classified it as "an environment limitation, not defects". Rounds 003, 004 and
005 carried that number forward as a known constant, and round 005 promoted it
to the programme's largest gap because it blocked document-level persistence,
P0-E and the §52.5 round-trip matrix.

It took two symlinks.

| | Before | After |
|---|---:|---:|
| Part tests run | 331 | 331 |
| **Passed** | 206 | **331** |
| Failed | 125 | **0** |

## What was actually wrong

The test binaries resolve data through `App::Application::getHomePath()`, which
for a build-tree binary is the **build root**. Two directories are expected
there, and `cmake --build` creates neither:

| Path | Needed by | Symptom when missing |
|---|---|---|
| `<build>/share/Mod/Material/Resources` | `MaterialManagerLocal` | every test that constructs a `Part::Box` throws `Material not found` — 121 tests |
| `<build>/tests/brepfiles` | `FuzzyBooleanTest` | `.brep` fixtures load as empty shapes, so volumes are 0 — 4 tests |

`RESOURCEDIR` looked like the lever and is not: the project sets it to `share`
during configuration, overriding any value passed on the command line, so the
lookup is always `<AppHomePath>/share/...`.

Reproduced precisely by removing each link and watching the corresponding
failures return.

## Consequences

**The baseline was wrong in the project's own records.** Three rounds stated
"125 environment failures" as a fact about the Part suite. The true figure is
zero. `docs/baseline-test-health.md` is corrected.

**Document-level testing is unblocked.** `Part::Box` now constructs, so
documents, recompute and `.FCStd` round-trips are testable. That reopens P0-E,
round 005's `UNCLASSIFIED` string-table case, and the §52.5 matrix.

**An upstream candidate falls out of it.** A build tree cannot run its own
developer tests, and nothing says so. A CMake rule creating these two links at
build time would fix it for everyone. Queued as `U-002`.

## The uncomfortable part

This sat in the records for four rounds as an accepted constant. It was cheap,
it was in the way of everything, and it was described accurately each time —
"an environment limitation" — without anyone testing that description. A
correct label is not the same as an investigation.

The lesson generalises past this instance: a known blocker that gates several
workstreams should be attacked *before* the workstreams route around it, not
after they have all recorded it as a footnote.

## Tooling

`tools/diofancad/setup-test-resources.sh <build-dir>` creates both links.
