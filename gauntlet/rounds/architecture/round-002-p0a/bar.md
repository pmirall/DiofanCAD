# Round 002 — P0-A Persistent Reference Prototype — Bar

Written before the measurement was run.

## Proof

**Hypothesis:** a persistent/semantic reference can survive a representative topology-changing edit more reliably than the baseline.

**Falsification:** no measurable improvement over baseline, unacceptable ambiguity in candidate matching, or complexity disproportionate to the gain.

## The question this round actually asks

Before building any new reference mechanism, the round must establish **whether there is a gap at all**. FreeCAD 1.0+ ships a substantial element-identity system (`src/App/ElementMap.*`, ~4800 lines with ~1900 lines of tests). V7 §68 forbids assuming a capability is absent.

So round 002 is sequenced deliberately:

```text
Phase 1  build a change-challenge harness and measure the BASELINE
Phase 2  only if Phase 1 finds real failures, prototype recovery and measure again
```

If Phase 1 shows the baseline already survives realistic change, the correct outcome is **not** to build a reference layer. That is a kill decision under §77.6 ("a simpler design achieves equivalent results") and it saves the most expensive workstream in the programme.

## What counts as a failure

The harness distinguishes four outcomes per reference, and the ranking matters:

| Outcome | Meaning | Severity |
|---|---|---|
| `resolved-correct` | still points at the intended face | success |
| `RESOLVED-WRONG` | resolves, but to a **different** face | **critical** — silent corruption |
| `broken` | no longer resolves, target still exists | recoverable failure |
| `target-gone` | the intended face genuinely no longer exists | not a failure |

`RESOLVED-WRONG` is worse than `broken`. A broken reference tells the engineer something is wrong. A wrong one puts a fillet on the wrong edge and says nothing. Existing name-level tests cannot separate these two, because both produce a name that looks valid.

## Bar

| # | Requirement | Measured how |
|---|---|---|
| B1 | Ground truth is geometric, not name-based | face identity = outward normal + side, computed from OCCT |
| B2 | The control scenario passes | topology-preserving parameter change: 0 wrong, 0 broken |
| B3 | Silent corruption is measured, not assumed absent | every scenario reports `wrong` explicitly |
| B4 | At least one scenario changes topology for real | through-pocket becomes blind; operand replaced |
| B5 | Baseline result recorded before any new mechanism | Phase 1 numbers committed before Phase 2 starts |
| B6 | Harness adds no divergence to upstream code | prototype lives in a test file |

## Critical failures

- The control scenario (B2) fails → the harness or the capture is wrong, and no other number can be trusted.
- Any scenario reports `RESOLVED-WRONG` > 0 → a real silent-corruption defect in the baseline, which becomes the headline finding regardless of everything else.

## Out of scope

PartDesign features, sketch external geometry, assembly references, save/reload of references (that is P0-D). Named here so their absence is a choice rather than an oversight.
