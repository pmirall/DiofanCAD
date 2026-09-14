# DiofanCAD — Upstream Synchronization Record

## Purpose

V7 §14 and §61 make fork divergence a first-class program metric. This file is the durable record of where the fork was cut, when it was last synchronized, and what it currently costs to stay diverged.

The rule: **no large experiment starts on a stale baseline.**

```text
sync upstream
↓
baseline
↓
experiment
```

## Upstream identity

| Field | Value |
|---|---|
| Upstream project | FreeCAD |
| Canonical upstream | `https://github.com/FreeCAD/FreeCAD` |
| Fork remote (`origin`) | `https://github.com/pmirall/DiofanCAD` |
| Upstream licence | LGPL-2.1 (see `LICENSE`) |
| Integration branch | `main` (tracks upstream) |
| Lab branch concept | `feature/diofancad-evolution` |

## Fork baseline

| Field | Value |
|---|---|
| Baseline commit | `6608bfc249f460efd5f99765304d0ee9b5d06658` |
| Baseline commit date | 2026-09-14 |
| Baseline subject | `Bump actions/setup-dotnet from 5.4.0 to 6.0.0` |
| Baseline version (`version.json`) | 26.3.0dev |
| DiofanCAD commits on top of baseline at cut | 0 |
| Baseline verified against upstream remote | NO — `origin` is the fork; upstream has not been fetched in this environment |

That last row matters. The fork's `main` is an import of FreeCAD history, but nothing here has yet compared it commit-for-commit against `FreeCAD/FreeCAD`. Until someone runs the procedure below with the upstream remote configured, "we are at upstream parity" is `UNKNOWN`, not a fact.

## One-time setup

```bash
git remote add upstream https://github.com/FreeCAD/FreeCAD.git
git fetch upstream main
```

## Sync procedure

```bash
git fetch upstream main
git checkout main
git merge --ff-only upstream/main      # integration branch must stay a clean mirror
git push origin main
# then rebase or merge the lab branch onto the refreshed main, and re-run the proofs
```

If `--ff-only` fails, `main` has drifted: the drift is a defect to be explained and removed, not merged over. DiofanCAD work belongs on lab branches, never as unreviewed commits on the mirror.

## Divergence report

Generate with:

```bash
tools/diofancad/divergence-report.sh            # defaults to upstream/main vs HEAD
tools/diofancad/divergence-report.sh upstream/main feature/diofancad-evolution
```

Record each milestone's output here. Do not summarize it from memory.

| Date | Upstream ref | DiofanCAD ref | Upstream age (commits behind) | DiofanCAD commits ahead | Files changed | Subsystems changed | Conflict cost | Upstream candidates | Verdict |
|---|---|---|---|---|---:|---|---|---:|---|
| 2026-09-14 | not fetched | `main` @ `6608bfc2` | UNKNOWN | 0 | 0 | none | none | 0 | BASELINE CUT |
| 2026-09-14 | `main` @ `6608bfc2` (local mirror, not verified against upstream) | terrain branch @ `5b524e18` | 0 | 1 | 37 (all DiofanCAD-native) | none | clean | 0 | TERRAIN PREPARED |
| 2026-09-14 | `main` @ `6608bfc2` (local mirror, not verified against upstream) | P0-C @ `5f34f0f1` | 0 | 2 | 55 total — 48 native, **7 in upstream code** (+752/−3) | `src/App`, `tests/src/App`, `cMake/FreeCAD_Helpers` | clean | 1 (U-001) | FIRST UPSTREAM-CODE DIVERGENCE |
| 2026-09-14 | `main` @ `6608bfc2` (local mirror, not verified against upstream) | P0-A @ `b5f87409` | 0 | 4 | 65 total — 56 native, **9 in upstream code** (+1156/−4) | `src/App`, `tests/src/App`, `tests/src/Mod/Part`, `cMake/FreeCAD_Helpers` | clean | 1 (U-001) | P0-A ADDED TESTS ONLY |
| 2026-09-14 | `main` @ `6608bfc2` (local mirror, not verified against upstream) | round 003 @ `a95bb784` | 0 | 6 | 74 total — 64 native, **10 in upstream code** (+1867/−3) | unchanged set | clean | 1 (U-001) | ROUND 003 ADDED TESTS ONLY |
| 2026-09-14 | `main` @ `6608bfc2` (local mirror, not verified against upstream) | round 004 @ `03830bdc` | 0 | 8 | 83 total — 72 native, **11
| 2026-09-14 | `main` @ `6608bfc2` (local mirror, not verified against upstream) | P0-D @ `bda3217a` | 0 | 10 | 91 total — 79 native, **12
| 2026-09-14 | `main` @ `6608bfc2` (local mirror, not verified against upstream) | round 006 @ `5379356b` | 0 | 12 | 96 total — 84 native, **12
line in upstream code** (+2922/-3) | unchanged set | clean | 2 (U-001, U-003) | ROUND 006 ADDED NO CODE |
line in upstream code** (+2922/-3) | unchanged set | clean | 1 (U-001) | P0-D ADDED TESTS ONLY |
line in upstream code** (+2518/-3) | unchanged set | clean | 1 (U-001) | ROUND 004 ADDED TESTS ONLY |

### Reading the P0-C row

Seven upstream files, +752/−3 lines. Most of that is two new files
(`src/App/RecomputeTrace.*`, 451 lines) and a test (235 lines) that exist in
their own right and can be lifted out as a unit. The part that is genuinely
entangled with upstream code is **36 lines in `Document.cpp` and 3 lines of
CMake**, plus a 5-line CMake fix that upstream should want back (U-001).

That distinction is the one worth tracking. A thousand lines of new files
alongside upstream code costs little to carry; thirty lines inside a hot
function costs every rebase. The budget below should eventually be expressed
in the second kind of line, not the first.

## Divergence budget

The budget is deliberately empty. V7 §11 requires a threshold that stops feature expansion, but a number invented before any measurement exists would be theatre. Set it at the first milestone that produces real divergence data, and record the decision in `docs/decisions.md`.

| Metric | Budget | Status |
|---|---|---|
| Upstream age (days behind) | TO SET | UNKNOWN |
| Changed files outside `docs/`, `gauntlet/`, `tools/diofancad/` | TO SET | UNKNOWN |
| Changed upstream subsystems | TO SET | UNKNOWN |
| API divergence | TO SET | UNKNOWN |
| Data-model divergence | TO SET | UNKNOWN |
| Merge conflict rate | TO SET | UNKNOWN |

## Breach protocol

```text
STOP FEATURE EXPANSION
↓
SYNC UPSTREAM
↓
ASSESS DIVERGENCE
↓
EXTRACT / REFACTOR / DROP
↓
CONTINUE
```

Fork rot is an engineering defect, tracked like any other.
