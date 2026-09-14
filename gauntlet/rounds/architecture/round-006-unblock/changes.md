# Round 006 — Changes

## Divergence introduced

**None in upstream code.**

| File | Lines | Class | What |
|---|---|---|---|
| `tools/diofancad/setup-test-resources.sh` | +40 new | native | Creates the two links a build tree needs to run its own tests |

No test file, no `src/` change. The round was an investigation, and its output
is a corrected record plus one script.

## Records corrected

- `docs/baseline-test-health.md`: the Part suite is 331 of 331, not 206 of 331.
  The "125 environment failures" carried through rounds 002-005 were entirely a
  resource-path problem.
- `gauntlet/state/master.json`: largest gap moves to P0-E.
- `docs/upstream-backlog.md`: `U-002` added.

## Why this is an upstream candidate, not just a local fix

A developer who clones FreeCAD, configures a build tree and runs
`Part_tests_run` sees 125 failures that look like code defects. Nothing in the
build output, the test output or the documentation says a resource layout is
required. A CMake rule creating the two links at build time would fix it once
for everyone, and it is independent of anything DiofanCAD is doing.
