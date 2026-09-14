# Round 008 — Where Are Element Maps Actually Active? — Bar

Written before the measurement was run.

## The question

Round 007 found that in a Part-workbench document built from primitives, no
feature carried an element map: `Part::Box`, `Part::Cut` and `Part::Fillet` all
reported `hasElementMap()` false, and both document consumers survived change
by stable indexing rather than by identity.

That leaves the programme with two possible worlds and no way to tell them
apart:

1. **Narrow.** An artefact of primitives and the Part workbench. PartDesign
   chains starting from a Sketch — where FreeCAD's toponaming work is actually
   aimed — are fully mapped, and rounds 002–005 describe something real.
2. **Wide.** Element maps are far less present in real documents than the
   programme assumed, and four rounds characterised a mechanism largely on the
   shelf.

The difference decides whether four rounds of measurement describe the product
or a component nobody's model reaches.

## Prediction, recorded before measuring

> **Narrow.** PartDesign is the workbench the March 2024 toponaming project
> targeted — the source is full of comments about it — so a `Sketch → Pad`
> chain should carry element maps, and a `PropertyLinkSub` to a pad face
> should store a non-empty mapped name.
>
> If that holds, round 007's finding is about Part primitives specifically, and
> the honest conclusion is that element-map coverage varies by workbench: the
> identity layer is real where it was built for, and absent where it was not.

If PartDesign is *also* unmapped, the prediction is wrong and the programme has
a much larger problem than a scoping caveat.

## What gets measured

| # | Measurement |
|---|---|
| M1 | `hasElementMap()` and `getElementMapSize()` at each step of a `Sketch → Body → Pad` chain |
| M2 | Whether a `PropertyLinkSub` to a pad face stores a **non-empty** mapped name — the thing round 007 found empty |
| M3 | The same probe on a second PartDesign feature downstream of the pad, to see whether maps propagate or only appear |
| M4 | A change challenge in the mapped world, if M1 shows maps exist: change a sketch dimension, recompute, and check a stored reference against physical ground truth |

## Bar

| # | Requirement | Measured how |
|---|---|---|
| B1 | A real `Sketch → Pad` chain in a document, recomputed | not hand-built shapes |
| B2 | The coverage probe runs **first** and its result gates the interpretation of everything else (D-016) | M1 before M4 |
| B3 | Ground truth physical, never index-based | absolute geometry |
| B4 | Negative control fires | a deliberately wrong expectation must be reported |
| B5 | Every measurement reports whether it was exercised (D-015) | explicit counters |
| B6 | Suspicion proportional to how good the result would be (D-017) | any dramatic result re-checked before write-up |
| B7 | No upstream divergence | test file only |

## Critical failures

- **M1 shows maps present but M2 shows the consumer still stores an empty mapped name** → the identity layer exists and still does not reach the user, which is worse than either world above and would be the round's headline.
- A dramatic result written up without the instrument being re-checked → a process failure, given three such errors in seven rounds.

## Out of scope

The GUI build and user preferences, other workbenches, and the string-table
question from round 005.
