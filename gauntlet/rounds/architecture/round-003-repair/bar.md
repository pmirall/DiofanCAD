# Round 003 — Repair on Hard Geometry — Bar

Written before the measurement was run.

## What this round decides

Round 002 reframed the wedge to **reference repair** (D-011) on the strength of
one number: all 8 broken references had exactly one matching descriptor
candidate. That number carried a caveat big enough to kill the decision — the
candidate counter shared a predicate with the ground truth, and the geometry
was axis-aligned boxes, where unambiguous identity is close to guaranteed.

This round is the test that can kill D-011. It asks:

> On geometry where features genuinely resemble each other, does a
> descriptor-based repair propose **confident wrong answers**?

A repair layer that confidently points at the wrong face is worse than the
honest breakage the baseline gives today. So the primary metric is the
**mis-repair rate**, not the repair rate.

## The methodological fix this round must carry

Round 002's matcher and ground truth shared a predicate. Here they must be
independent, and the independence must be structural rather than promised:

| | Uses | Never sees |
|---|---|---|
| **Ground truth** | absolute coordinates derived by hand from each scenario's construction | anything the matcher computes |
| **Matcher** | generic descriptors only: surface type, axis, area, **bounding-box-relative** position, radius | the scenario, its labels, or any absolute expectation |

The matcher is scenario-blind: it receives a stored descriptor and a shape, and
ranks every face. It cannot know which answer is wanted.

## Scenarios — geometry chosen to create ambiguity

| ID | Shape | Change | Why it is hard |
|---|---|---|---|
| H1 | block with **two similar pockets** | widen one pocket **and lengthen the block** | the two pockets' walls have identical normals and areas; lengthening shifts every relative position, so a position-normalised matcher can confuse pocket A with pocket B |
| H2 | block with **four identical holes** | lengthen the block, then delete one hole | four near-identical cylinders; deleting one shifts the rest. This is the classic "which hole did I mean?" |
| H3 | block with a **fillet** | change the fillet radius, then resize the base | non-planar faces; the fillet's cylindrical face has no normal, only an axis |

## Bar

| # | Requirement | Measured how |
|---|---|---|
| B1 | Matcher is scenario-blind and independent of ground truth | structural: separate inputs, documented in the code |
| B2 | Control passes | unchanged recompute: 0 wrong, 0 broken |
| B3 | Baseline silent corruption measured on hard geometry | every resolved reference checked against analytic ground truth |
| B4 | Every broken reference gets a verdict | repair-correct, **repair-wrong**, or abstain |
| B5 | The matcher reports a top-1/top-2 margin and abstains when it is thin | margin printed per proposal |
| B6 | Non-planar faces handled, not skipped | cylinders carry axis + radius |
| B7 | No divergence in upstream code | test file only |

## Critical failures

- **Baseline silent corruption > 0** on hard geometry → a real FreeCAD defect, and the headline regardless of everything else.
- **Mis-repair rate > 0 at high confidence** → D-011's premise is unsafe as stated. The wedge does not simply proceed; repair must either abstain under ambiguity or the wedge moves again.

## What a kill looks like

If the matcher confidently mis-repairs on repeated features, the honest
outcomes are, in order of preference:

1. repair is redefined as *ranked candidates the engineer confirms*, never an automatic fix;
2. the wedge moves off repair entirely (§77.6 again).

Neither is a failure of the idea. Both are cheaper than discovering it after
building the layer.

## Out of scope

Edge and vertex references (named in round 002's gap list and still unmeasured),
sketch external geometry, document-level recompute, save/reload.
