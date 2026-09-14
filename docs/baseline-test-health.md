# DiofanCAD — Baseline Test Health

## Purpose

Measure the starting quality of the upstream/project test estate before new architectural work is introduced.

## Status

`PARTIAL — App-level C++ suite measured; everything else still UNKNOWN`

Measured at the fork baseline commit on 2026-09-14, in a console-only build
(`BUILD_GUI=OFF`, all workbenches OFF, `ENABLE_DEVELOPER_TESTS=ON`). See
`gauntlet/rounds/architecture/round-001-p0c/evidence/baseline-tests.txt`.

### Measured: `App_tests_run`

| Metric | Value |
|---|---:|
| Tests run | 553 |
| Passed | 548 |
| **Failed** | **3** |
| Skipped | 2 |
| Disabled | 7 |
| Wall time | ~0.3 s |

Three tests fail on a clean checkout, before any DiofanCAD change:

| Test | Status |
|---|---|
| `DocumentObjectTest.getSubObjectList` | failing at baseline, cause not investigated |
| `MoveProperty.staticProperty` | failing at baseline, cause not investigated |
| `PropertyExpressionEngineTest.executeCrossPropertyReference` | failing at baseline, cause not investigated |

This matters beyond bookkeeping: a project that does not know its baseline
failures will eventually blame them on its own changes. These three are now
named, so any future failure list can be diffed against them.

Not yet investigated, and deliberately not fixed in the same round that
measured them. Candidates for the upstream backlog once root-caused.

### Measured: `Part_tests_run` (corrected in round 006)

| Metric | Value |
|---|---:|
| Tests run | 331 |
| **Passed** | **331** |
| Failed | 0 |

**The Part suite is fully green.** Rounds 002 to 005 recorded "125 of 312
failing, an environment limitation". That description was accurate and never
investigated. Round 006 cleared every one of them with two symlinks.

The test binaries resolve data through `App::Application::getHomePath()`, which
for a build-tree binary is the **build root**, and `cmake --build` creates
neither directory the tests expect there:

| Path | Needed by | Tests affected |
|---|---|---:|
| `<build>/share/Mod/Material/Resources` | the material library; without it any `Part::Box` throws `Material not found` | 121 |
| `<build>/tests/brepfiles` | `FuzzyBooleanTest` fixtures | 4 |

`RESOURCEDIR` is a red herring: the project sets it to `share` during
configuration, overriding any command-line value.

Run `tools/diofancad/setup-test-resources.sh <build-dir>` before the suite.
Queued upstream as `U-002`, because a build tree that cannot run its own tests
— and says nothing about why — affects every FreeCAD developer, not just this
fork.

## Health dimensions

Track separately:

- deterministic failures;
- flaky failures;
- missing coverage on critical workflows;
- tests that cannot run in CI;
- slow tests that block iteration;
- tests that lack meaningful assertions;
- compatibility/regression tests.

## Baseline rule

Do not use a green CI result as proof of good health without inspecting scope and test coverage.

## Exit criteria

Baseline is complete when:

```text
[ ] test inventory is reproducible
[ ] critical workflow coverage is identified
[ ] flaky tests are classified
[ ] baseline failures are recorded
[ ] CI command is documented
[ ] artifact locations are recorded
```
