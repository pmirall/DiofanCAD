# Round 007 — P0-E at Document Level — Verdict

## PASS on the transfer test. And the transfer test found that there was nothing to transfer.

| # | Requirement | Result |
|---|---|---|
| B1 | Real document, real features, real `recompute()` | PASS |
| B2 | Ground truth physical, not index-based | PASS — and it caught my own error, see below |
| B3 | Negative control fires | PASS — N4 |
| B4 | Genuine `.FCStd` round-trip | PASS — `saveAs` → `closeDocument` → `openDocument` |
| B5 | Every measurement says whether it was exercised | PASS — and M1 says it was **not** |
| B6 | No upstream divergence | PASS — one new test file |

## What held

- **M2**: a `Part::Fillet` stayed on the same physical corner when the base box was lengthened. Nearest corner before `(20,10)`, after `(30,10)` — it followed.
- **M3**: after `saveAs` → close → reopen, the fillet is at coordinates identical to six decimal places. This closes round 005's document-level gap.
- **M1**: the `PropertyLinkSub` reference still pointed at the correct physical face after the change.

On the everyday path, a parameter change and a full save/reload cycle preserved
both references. That is a real result and it is the one a user would feel.

## The headline

```
P1 Part::Box    has_element_map=0  element_map_size=0  faces=6
P1 Part::Cut    has_element_map=0  element_map_size=0  faces=10
P1 Part::Fillet has_element_map=0  element_map_size=0  faces=7
```

**In this configuration, no document-level Part feature carried an element map.**

The mechanism is visible in the source. `Part::Box::execute()` assigns a raw
`TopoDS_Shape`:

```cpp
BRepPrimAPI_MakeBox mkBox(L, W, H);
TopoDS_Shape ResultShape = mkBox.Shape();
this->Shape.setValue(ResultShape, false);
```

`Part::Cut::execute()` *does* use `makeElementShape`, the element-mapping API —
but booleans propagate maps from their inputs, and an unmapped input yields an
unmapped result. The chain starts empty and stays empty.

The consumer side matches. `PropertyLinkSub` stored a shadow entry whose mapped
name was **empty**, so it resolved by raw index. `Part::Fillet` stores
`FilletElement { int edgeid; double radius1, radius2; }` — an index, with no
name and no shadow, by design.

So both references survived **by stable indexing**, not by the element map.
`Part::Box` happens to number its faces and edges the same way at any length.

## What this means for rounds 002–005

It does not invalidate them. They measured the element map correctly, and
everything they found about it stands.

What changes is what they *mean*. Four rounds characterised a mechanism that,
in the document path tested here, is not active and reaches no consumer. The
survival rates, the 0% silent corruption, the exclusion disambiguator — all of
it describes `ElementMap`'s behaviour, not necessarily what a user experiences
in a Part-workbench document.

## Scope — what this round does NOT establish

This is the part that decides whether the finding is a curiosity or a crisis,
and it is genuinely open:

- **Configuration.** `BUILD_GUI=OFF`, default parameters, no user preferences. A GUI build with default preferences was not tested.
- **Workbench.** Part only, with a chain starting from primitives. Sketcher and PartDesign are not built. PartDesign is where FreeCAD's toponaming work is aimed, and a chain starting from a Sketch was not tested.
- **One change** on one shape family.

The obvious next question — does a PartDesign chain starting from a Sketch
populate element maps? — is unanswered, and until it is, "the identity layer
reaches no consumer" is true *of what was measured here* and nothing wider.

## The third measurement error

M2 first reported the fillet moving to a different corner: face centre
`(19.27, 9.27)` before, `(29.27, 9.27)` after, against an expectation of
`(30, 10)`. That would have been a headline product failure.

It was wrong. A fillet's cylindrical face centre sits *inside* its corner by a
radius-dependent offset, so the expectation compared a face centre against
corner coordinates. The corrected comparison — nearest corner — shows the
fillet followed exactly.

Third time in seven rounds that my own ground truth encoded a false assumption
about geometry, and the third time the error pointed towards a *more*
dramatic conclusion than the truth. Round 003's did too, and so did round 005's
unexercised hasher. The bias is consistent and worth naming: **a measurement
error is more likely to manufacture a finding than to hide one.**

## Decision

P0-E **PASS**. Phase 0's proof set is now P0-A, P0-C, P0-D, P0-E passing, with
P0-B still needing restatement after D-010.

But the programme's target moves again, and this time not because a measurement
refined a claim — because a measurement found the claim was aimed at a layer
the user never touches.
