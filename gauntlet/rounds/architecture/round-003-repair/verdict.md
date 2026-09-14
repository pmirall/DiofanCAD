# Round 003 — Repair on Hard Geometry — Verdict

## PASS on the bar. D-011 survives, but only in a constrained form.

No critical failure: the baseline never resolved to the wrong face, and the
matcher never completed a wrong repair. But the round found what it was built
to look for, and it changes what "repair" is allowed to mean.

| # | Requirement | Result |
|---|---|---|
| B1 | Matcher independent of ground truth | PASS — separate inputs; N1 proves the harness fires |
| B2 | Control passes | PASS — H0 |
| B3 | Baseline silent corruption measured | PASS — 0 in 8 references |
| B4 | Every broken reference gets a verdict | PASS — H4: abstained |
| B5 | Margin reported, abstains when thin | PASS — and this is what saved it |
| B6 | Non-planar faces handled | PASS — H3 fillet, H2/H4 cylinders |
| B7 | No upstream divergence | PASS — one new test file |

## Finding 1 — the baseline is stronger than round 002 suggested

Every one of the 8 tracked references resolved to the intended face, including
the cases built specifically to confuse it:

- two pockets with identical wall normals and areas, one widened, the block lengthened;
- four identical holes, the block lengthened **and** one hole deleted;
- a fillet whose radius changed, and whose base was then resized.

Round 002 found 8 breakages in 60 references. Round 003 found **zero in 8**,
on harder geometry. The difference is instructive: round 002's breakages all
came from a changed *generator* (tool replaced, feature inserted). Pure
geometric change, however aggressive, does not break these references.

## Finding 2 — the decisive one

H4 forced a real break: a hole deleted and re-created in the same place, so the
reference lost its anchor and repair was the only route left. Meanwhile the
block had been lengthened.

```
H4 matcher: top face=Face9 (x=12) score=1  margin=0.042  confident=0
            intended=Face8 (x=8)
```

**The matcher's best answer was the wrong hole, and it scored 1.0 — perfect.**

It scored perfectly because after the resize, the hole at x=12 sits at exactly
the relative position the stored reference remembers, with identical radius and
identical area. Every dimension of the descriptor agrees. The right answer, the
hole at x=8, scored 0.958.

A mis-repair was prevented by one thing: a margin threshold of 0.05 against a
measured margin of 0.042. That threshold has no principled basis. I chose it.

## What this means, stated plainly

**Score is not confidence. The margin is.** A repair layer that reports its
match quality will report 1.0 on a wrong answer. Only the distance to the
runner-up carries information about whether the answer can be trusted.

**And the margin here is too thin to carry a decision.** 0.042 separating right
from wrong is not a tuning problem that a better threshold fixes — it is an
information problem. The descriptor genuinely does not contain enough to tell
these two holes apart after a resize. No weighting of position, area and radius
will, because all three agree with the wrong answer.

**The uncomfortable combination:** repair is needed only when a generator
changes, and that is precisely when the geometric descriptor is least able to
disambiguate. The cases that need repair are the cases where repair is least
reliable.

## Decision

**D-011 survives, constrained.** Repair may never be automatic. It is:

> diagnosis + ranked candidates + an explicit statement of ambiguity, which the
> engineer confirms.

That is option 1 from this round's bar, and it is a real improvement over the
baseline, which today marks a broken reference `?` and says nothing else. But
it is a smaller and more honest claim than "DiofanCAD repairs your model".

**The competitive claim needs re-examination too.** The wedge was framed as
preserving engineering intent through change better than the benchmark. The
baseline already preserves it very well. Differentiation has to come from
**explaining what broke and why**, not from surviving more — which points at
Model Doctor (§19) rather than at a repair engine.

## Falsification conditions, revisited

| Condition | Outcome |
|---|---|
| Baseline silent corruption on hard geometry | Not found — 0 in 8, with a working negative control |
| Confident mis-repair | **Found in substance**: a perfect-scoring wrong answer, blocked only by an arbitrary threshold. Recorded as a near-miss, not as a clean pass |

## Caveats

Eight references, six scenarios, box-and-cylinder geometry. Only H4 forced an
actual break, so the repair path rests on a single data point — decisive in
what it shows, but singular. Edges, vertices, sketch external geometry and
document-level recompute remain unmeasured.
