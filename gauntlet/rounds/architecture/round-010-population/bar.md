# Round 010 — Mapped-Name Population — Bar

Written before the measurement.

## A correction to the question before it is asked

Round 009's gap was phrased as "how often do the 48 capable headers actually
hold a mapped name in a real model?" — as if there were a population *rate* to
measure.

Reading `PropertyLinkBase::_updateElementReference` shows there is not. It
stores a mapped name only when `GeoFeature::resolveElement` returns a non-empty
`newName`, and that happens only when the referenced feature's element map
resolves the subname. So the fraction of populated references in any model is
just a function of **what that model happens to reference**. A single number
would say more about my choice of fixture than about FreeCAD.

The measurable thing is the **rule**, not the rate.

## Prediction, from source reading, recorded before measuring

> A reference holds a mapped name **iff** the feature it points at has an
> element map that resolves the subname.
>
> Concretely: a reference to a `Part::Box` face is empty; a reference to a
> `Part::Cut` face or a `Part::Fillet` face is populated. Population follows the
> target, not the consumer.

This is route 2 — derived from the code path, independent of the harness. If
the harness shows a populated reference to an unmapped feature, or an empty one
to a mapped feature, then one of the two is wrong and the round stops until
that is resolved.

## What gets measured

| # | Measurement |
|---|---|
| M1 | For references to a primitive, a boolean result and a dress-up result: target `hasElementMap()` and whether the shadow `newName` is non-empty |
| M2 | Whether the correlation is exact — no populated reference to an unmapped target, no empty reference to a mapped one |
| M3 | Whether population survives a recompute and a change (it is recomputed state, so it could be transient) |
| M4 | What a reference to an unmapped target falls back to, and whether that fallback is visible or silent |

## Pre-flight

**NEGATIVE control:** assert a deliberately wrong expectation — that a
reference to a `Part::Cut` face is empty — and require the harness to report the
mismatch. Without it, "empty" results cannot be trusted.

**POSITIVE control:** mandatory, because M1 will report emptiness for at least
one case. Round 007's P2 established `;Face2;:Hce7,F.Face6` against a
`Part::Cut`; the same must show here, in the same run, so the probe proves it
can see a populated name before any emptiness is believed.

**EXERCISED check:** report how many references were walked and how many
targets were inspected. A walk that visited nothing also finds nothing.

**Would this be GOOD for the programme?** A finding that references are widely
unpopulated would support a gap, so yes — route 2 is mandatory and the answer
below is required.

**What would make this wrong?**
- The shadow may be populated lazily, so reading it at the wrong moment shows
  empty. M3 exists to attack this: if population appears only after a
  recompute, a single reading proves nothing.
- `getShadowSubs()` may not be the only place a mapped name lives.
- My fixture chooses the targets, so any *rate* I compute is an artefact. Only
  the correlation is reportable.

## Bar

| # | Requirement |
|---|---|
| B1 | Three target kinds: unmapped primitive, boolean result, dress-up result |
| B2 | The rule reported as a correlation, never as a bare population rate |
| B3 | All three controls reported in the same run |
| B4 | Population re-checked after a recompute and after a parameter change |
| B5 | The fallback for an unmapped target described, and classified as visible or silent |
| B6 | No upstream divergence |

## Critical failures

- The correlation is not exact → the source-derived prediction and the harness disagree, and neither can be trusted until that is explained.
- A bare population rate appears anywhere in the write-up.
