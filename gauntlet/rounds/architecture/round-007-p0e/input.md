# Round 007 — Input

## P0-E, restated

P0-E was written as a comparison: measure what survives a change challenge, and
falsify if there is *"no reproducible, material improvement over the defined
baseline"*. That assumed DiofanCAD would have built something to compare. D-010
decided it will not.

So P0-E becomes a transfer test: **do rounds 002–005's findings hold in a real
document?** Everything before this was hand-built — shapes constructed
directly, operations re-run manually, references resolved by calling
`getIndexedName` on a shape. A user changes a parameter and presses recompute,
and their references live inside features.

## The two consumers, and why the contrast matters

| Consumer | Stores |
|---|---|
| `App::PropertyLinkSub` | a subname plus a *shadow* mapped name, re-resolved through the element map |
| `Part::Fillet` | `FilletElement { int edgeid; double radius1, radius2; }` — a raw index |

`Part::Fillet` is a shipped, everyday feature. Its reference storage was read
from the source before the round was designed, and it is what prompted the
prediction in `bar.md`: a raw-index consumer cannot benefit from any identity
layer, however good.

## What became measurable only after round 006

Every test here constructs a `Part::Box`. Until round 006 supplied the two
missing symlinks, that threw `Material not found` and this round was
impossible. The blocker had been recorded as an accepted constant since round
002.

## Method

- a real `App::Document`, real features, a real `recompute()`;
- ground truth by absolute geometry — which physical corner the fillet sits on,
  which physical face a reference points at — never by index;
- a genuine `.FCStd` round-trip via `saveAs` / `openDocument`, not an in-memory copy;
- every measurement reports whether the mechanism under test was exercised (D-015).

That last rule is what turned M1 from a pass into a finding.
