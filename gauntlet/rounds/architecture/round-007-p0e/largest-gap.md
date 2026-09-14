# Round 007 — Largest Gap

**Largest Gap:** nobody has established where element maps are actually active, so four rounds of measurement have an unknown relationship to what a user experiences.

## Why this one

Round 007 found that in a Part-workbench document built from primitives, no
feature carried an element map and no consumer used one. That is either:

1. **narrow** — an artefact of this configuration, or of primitives being the
   wrong starting point, and PartDesign chains from a Sketch are fully mapped; or
2. **wide** — element maps are far less present in real documents than the
   programme assumed, and rounds 002–005 characterised a mechanism largely on
   the shelf.

The difference decides whether four rounds of work describe the product or a
component. Nothing currently distinguishes them, and it is cheap to find out:
build Sketcher and PartDesign, make a pad from a sketch, and print
`hasElementMap()` at each step.

## The measurement that would answer it

```
Sketch -> PartDesign::Pad -> PartDesign::Pocket
  at each step: hasElementMap(), getElementMapSize()
  then: does a PropertyLinkSub to a pad face store a non-empty mapped name?
```

If maps are present there, the finding is narrow and the wedge returns to
consumers and coverage. If they are absent there too, the programme needs to
re-examine what it has been measuring.

## Gaps deliberately not chosen

| Gap | Why it waits |
|---|---|
| P0-B restatement | It is the last unrun proof, but D-010 removed its purpose and it needs a reason to exist before it gets one |
| The string-table case from round 005 | Now testable at document level, but it is a compatibility question |
| Principled abstention thresholds | Round 004's gap; it limits repair rates, and repair may be aimed at the wrong layer entirely |
| The three unexplained App failures | Open since round 001, still not blocking |
