# Round 002 — P0-A — Verdict

## PASS on the bar. The hypothesis is REFRAMED.

Every bar item was met and no critical failure occurred. But the measurement
changed what P0-A should be about, and that is the real output of this round.

| # | Requirement | Result |
|---|---|---|
| B1 | Geometric ground truth | PASS — normal + outer/pocket, position deliberately excluded |
| B2 | Control scenario passes | PASS — S1 and S6: 10/10 correct |
| B3 | Silent corruption measured | PASS — measured in all six scenarios |
| B4 | Real topology change | PASS — S3, S4, S5 |
| B5 | Baseline recorded before any new mechanism | PASS — no new mechanism was built |
| B6 | No divergence in upstream code | PASS — one new test file, zero upstream lines |

## The headline

**Zero silent corruption in 60 references across 6 scenarios.**

| | count | rate |
|---|---:|---:|
| resolved correctly | 52 | 86.7% |
| **resolved to the wrong face** | **0** | **0%** |
| broken (visible failure) | 8 | 13.3% |

The baseline's element map never once pointed at a face other than the
intended one. When it could not follow a change, it said so. That is the safe
failure mode, and it is the opposite of the "toponaming is broken" narrative
the project was implicitly designing against.

Ordinary parameter changes — widening a pocket, resizing the base — were
survived completely (S2, S6: 20/20).

## What actually breaks, and why

All 8 failures are explicable, and none is a naming defect:

| Scenario | Broken | Why |
|---|---|---|
| S3 through → blind | the `-Z` outer face | it lost its hole; topologically a different face |
| S4 tool replaced | all 4 pocket walls | the tool that generated them no longer exists |
| S5 upstream feature inserted | the 3 outer faces the new notch cuts into | they were modified by a feature that did not exist at capture |

In each case the referenced face's **generator** changed. The element map
declines to guess, which is correct behaviour for an identity mechanism.

## Where the opportunity actually is

For all 8 broken references, exactly **one** face in the changed shape matches
the stored geometric descriptor. Not zero, not several — one.

So the gap is not identity. It is **repair**: the baseline has no layer that
says "this reference is broken, here is the single face that matches what you
described, with this confidence, apply or reject." That is precisely the half
of V7 §18 the baseline lacks (candidate matching, confidence, repair,
diagnostics) and precisely the half it already has (persistent identity,
topology mapping).

## Falsification conditions, revisited

| Condition | Outcome |
|---|---|
| No measurable improvement over baseline | **Not tested, because no new mechanism was justified.** The baseline leaves 0% corruption and 13% visible breakage; a replacement identity scheme has almost no headroom to win |
| Unacceptable ambiguity in candidate matching | **Not observed** — 8/8 unambiguous, with the caveat below |
| Complexity disproportionate to the gain | **Would have been true** for a new reference mechanism. It is not obviously true for a repair layer |

## Decision

**Do not build a new persistent-reference mechanism.** REUSE `ElementMap` /
`MappedName` as the identity substrate — this is a kill decision under V7 §77.6
("a simpler design achieves equivalent results"), and it removes the single
largest speculative workstream on the roadmap.

**Reframe the wedge** from *reference survival* to *reference repair*. Survival
is already good and, more importantly, already safe. Repair is absent.

## The caveat that keeps this honest

The candidate counter uses the same predicate as the ground truth, so "exactly
one candidate" is partly unambiguous by construction on axis-aligned boxes with
rectangular pockets. It is a **necessary condition** for unambiguous repair,
not a sufficient one. On fillets, revolved surfaces, or parts with many
coplanar faces, ambiguity is expected to rise — and a repair layer that
proposes a confident wrong answer would be worse than today's honest
breakage. Measuring that is the next round's job, not this one's conclusion.

Six scenarios on boxes is also not a general claim. It is enough to redirect
the programme, not enough to publish.

## A methodological note worth keeping

The first version of this harness reported **4 of 10 references in S2 as
silently resolving to the wrong face** — which would have been a serious
defect in FreeCAD, and a compelling headline for this project. It was wrong.
The ground truth required a face's plane to match exactly, so pocket walls
that had legitimately moved when the pocket widened were scored as corruption.

The control scenario is what exposed it, and the result was inverted by fixing
the measurement rather than reported by trusting it. Any future round claiming
a competitor or baseline defect should assume the instrument is wrong first.
