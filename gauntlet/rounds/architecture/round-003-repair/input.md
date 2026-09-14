# Round 003 — Input: what had to be fixed from round 002

## The flaw this round exists to correct

Round 002 concluded that all 8 broken references had exactly one matching
candidate, and reframed the wedge to repair on that basis. Its own verdict
recorded the problem: the candidate counter and the ground truth shared a
predicate, so "exactly one candidate" was partly true by construction, and the
geometry was axis-aligned boxes where unambiguous identity is close to
guaranteed.

An unchecked version of that conclusion would have sent the programme off to
build a repair layer on a number that could not support it.

## How independence is enforced here

| | Reads | Never sees |
|---|---|---|
| Ground truth | absolute coordinates, written by hand per scenario (`near(geom.center.x, 8)`) | anything the matcher computes |
| Matcher | surface type, axis, area, **bounding-box-relative** position, radius | the scenario, its labels, the expected answer |

Both read the same OCCT geometry. What keeps them independent is what each is
allowed to *use*: absolute position for the expectation, normalised descriptors
for the search.

## The negative control, and why it is not optional

A measurement that never fires looks identical to a broken one. Round 003's
first full run reported zero failures everywhere, which is exactly what a
harness with a broken comparison would also report.

`N1_HarnessDetectsWrongResolution` tracks one face while declaring a different
face to be the intended target, and requires the harness to report
`resolved_WRONG=1`. It does. Without that test, every zero in this round would
be uninterpretable.

## Geometry chosen to be ambiguous

| ID | Construction | Change | Ambiguity it creates |
|---|---|---|---|
| H1 | block, two 4×4 pockets at x=3 and x=13 | widen pocket B to 6×6, lengthen block 20→24 | pocket walls share normals and areas; every relative position shifts |
| H2 | block, four ø3 holes at x=4,8,12,16 | lengthen block 20→30, delete the hole at x=4 | four near-identical cylinders |
| H3 | block with one filleted edge | radius 2→3, then base 10→16 | non-planar face: axis and radius, no normal |
| H4 | as H2 | lengthen block **and re-create the hole at x=8 under a new tag** | forces a real break, and puts the wrong hole at the remembered relative position |

H4's numbers are deliberate. In a 20-long block the hole at x=8 has relative
position 0.40. In a 30-long block, 0.40 is where the hole at **x=12** sits.
A position-normalised matcher should be pulled to the wrong answer.

## The matcher under test

It is the layer D-011 proposed to build, written in the smallest form that can
be judged:

```
reject on surface type mismatch
reject on axis misalignment (dot < 0.99)
score = 0.55 * position + 0.25 * area + 0.20 * radius
propose only when score >= 0.50 AND margin to runner-up >= 0.05
```

The thresholds are guesses. Whether that matters is one of the things the round
finds out.
