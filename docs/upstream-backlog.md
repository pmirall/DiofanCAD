# DiofanCAD — Upstream Backlog

## Purpose

Maintain an explicit queue of changes that may be extracted, proposed, or deliberately kept experimental.

## Record format

| ID | Change | Class | Problem | Evidence | Dependencies | Compatibility | Performance | Upstream boundary | Status |
|---|---|---|---|---|---|---|---|---|---|
| U-001 | Request Qt `LinguistTools` regardless of `BUILD_GUI` | **U0** | `BUILD_GUI=OFF` cannot configure: `src/App` calls `qt_add_translation()` unconditionally but the component is only requested under `BUILD_GUI`; no CI job builds GUI-off | reproducible at the baseline commit; round 001 `changes.md` | none | none — additive Qt component | none | `cMake/FreeCAD_Helpers/SetupQt.cmake`, +5/−2 | **READY** — extractable as a standalone PR unrelated to DiofanCAD |
| U-002 | `App::RecomputeTrace` — per-object recompute time and causality | U2 | recompute is diagnosable only as a whole; no per-object time, no record of why an object recomputed | E-0004 … E-0008, round 001 | none | additive; off by default; no file-format change | ~1.09 µs/object when enabled, unmeasurable when disabled | `src/App/RecomputeTrace.*` + 36 guarded lines in `Document.cpp` | EXPERIMENTAL — revisit for U1 after P0-E consumes it |
| U-003 | Create the developer tests' expected resource layout at build time | **U0** | a configured build tree cannot run its own test suite: 125 of 331 Part tests fail with `Material not found` or empty brep fixtures, and nothing in the build, test output or docs says a layout is required | round 006, reproduced by removing each link and watching the failures return | none | none — build-system only | none | three links under the build root: `share/Mod/Material/Resources`, `tests/brepfiles` and `tests/TestModels` | **READY** — independent of DiofanCAD |

## Notes on U-003

Found the same way as U-001: by trying to do something else. A developer who
clones FreeCAD, configures a build tree and runs `Part_tests_run` sees 125
failures that look like code defects. The fix is a CMake rule, and
`tools/diofancad/setup-test-resources.sh` already documents the exact layout.

A third path turned up later: `tests/TestModels`, needed by PartDesign's
`BackwardCompatibilityTest`. Without it the test throws; with it, the same test
**segfaults**, which is a separate problem and not part of this candidate.

## Notes on U-001

U-001 is the kind of change the extraction pipeline exists for: found by
accident while doing something else, narrow, reproducible, and useful to
upstream whether or not DiofanCAD survives. It should be offered upstream
before it accumulates context from this fork.

It also carries a second, cheaper suggestion for upstream: a CI job that
configures with `BUILD_GUI=OFF` would have caught this. That is a separate
proposal, not part of the fix.

## Promotion rules

A candidate moves only with evidence:

```text
U3 experimental
  ↓ evidence
U2 experimental
  ↓ stabilization
U1 upstream candidate
  ↓ reviewable extraction
U0 upstream-ready
  ↓ upstream decision
UPSTREAM-ACCEPTED
  ↓ sustained maintenance
UPSTREAM-MAINTAINED
```

## Drop / defer rules

A candidate should be dropped or deferred when its measured value no longer justifies:

- implementation cost;
- maintenance cost;
- divergence cost;
- compatibility complexity;
- upstream extraction cost.
