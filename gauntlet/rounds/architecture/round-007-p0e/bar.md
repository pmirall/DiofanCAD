# Round 007 — P0-E Realistic Change Challenge at Document Level — Bar

Written before the measurement was run.

## Restating P0-E

P0-E was written as: *"Sketch → Part → Modify → change challenge; measure what
intent and references survive"*, with falsification being *"no reproducible,
material improvement over the defined baseline"*.

That falsification assumed DiofanCAD would have built something to compare
against the baseline. D-010 decided it will not, so there is nothing to beat.
P0-E is therefore restated, in the same way P0-B needs restating:

> **Do the findings of rounds 002–005 transfer from `TopoShape` to a real
> document?**

Everything measured so far was built by hand: shapes constructed directly,
operations re-run manually, references resolved by calling `getIndexedName` on
a shape. A user does none of that. They change a parameter in a document and
press recompute, and their references live inside *features*.

## The question that makes this worth running

Rounds 002–005 measured the identity layer and found it strong. But a
document-level reference is only as robust as **the consumer that stores it**,
and the two consumers available store references in completely different ways:

| Consumer | Stores | Expected |
|---|---|---|
| `App::PropertyLinkSub` | a subname string plus a *shadow* mapped name, re-resolved through the element map | robust — this is the path rounds 002–005 measured |
| `Part::Fillet` | `FilletElement { int edgeid; double radius1, radius2; }` — a **raw index**, no name, no shadow | fragile by construction |

`Part::Fillet` is a shipped, everyday feature. If it stores raw indices, the
identity robustness measured over four rounds never reaches it.

**Prediction, recorded before measuring:** the `PropertyLinkSub` reference will
survive a parameter change; the `Part::Fillet` reference will either survive by
luck (because box edge numbering happens to be stable) or move to a different
physical edge. If it moves, that is a visible, document-level, product-facing
failure of exactly the kind the wedge claims to address — and it is not fixable
by a better identity layer, because the consumer does not use one.

## What gets measured

| # | Measurement |
|---|---|
| M1 | A `PropertyLinkSub` reference to a face survives a parameter change and recompute, resolving to the same physical face |
| M2 | A `Part::Fillet` stays on the same physical edge after the same change |
| M3 | Both survive a real document **save → close → reopen** cycle (`saveAs` / `openDocument`), closing round 005's document-level gap |
| M4 | When a reference does break, the `?` missing-element marker appears — the diagnostic D-013 says the baseline has and does not use |
| M5 | The P0-C recompute trace explains what recomputed and why during the change |

## Bar

| # | Requirement | Measured how |
|---|---|---|
| B1 | A real `App::Document` with real features and a real `recompute()` | not TopoShape calls |
| B2 | Ground truth is physical, not index-based | the filleted corner and referenced face identified by absolute geometry |
| B3 | Negative control fires | a deliberately wrong expectation must be reported |
| B4 | A genuine `.FCStd` round-trip | `saveAs` then `openDocument`, not an in-memory copy |
| B5 | Every measurement reports whether it was exercised (D-015) | explicit counters |
| B6 | No upstream divergence | test file only |

## Critical failures

- **M1 fails** → the document-level path is weaker than the shape-level path, and rounds 002–005 do not describe real use.
- **M3 fails** → references do not survive a real save/reload, which would invalidate round 005's transfer claim.

## What a `Part::Fillet` failure would mean

Not a critical failure of this round — it is a finding, and arguably the most
product-relevant one available. It would say the wedge's target is not the
identity layer at all but the **consumers**, and that the cheapest real
improvement to FreeCAD here is to make features store mapped names rather than
indices.

## Out of scope

Sketcher and PartDesign (not built), edges and vertices beyond the fillet case,
the string-table question from round 005.
