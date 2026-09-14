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
