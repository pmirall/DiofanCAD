# Round 009 — Largest Gap

**Largest Gap:** capability is not population — 48 headers *can* store mapped names, and nobody has measured how often they actually hold one in a real model.

## Why this one

The inventory shows the storage mechanism is right almost everywhere. But round
007 measured a `PropertyLinkSub` that stored an **empty** mapped name, because
the feature it referenced was a `Part::Box` — a primitive with no element map.
The property was capable; the target had nothing to give it.

So the chain has two links and only the first is now known:

```
consumer CAN store a mapped name     measured: 48 of 50 headers  ✓
the referenced feature HAS a map     measured only for Box/Cut/Fillet
the consumer DOES hold a name        measured once, and it was empty
```

Every survival number from rounds 002–005 assumes the third link. One
observation of it exists and it is a negative one.

## The measurement that would answer it

Build a representative model, walk every `PropertyLinkSub*` in the document,
and report what fraction hold a non-empty mapped name — with a positive control
proving the walk can see one, per D-018.

That needs no new build for the Part workbench. It needs PartDesign for the
more interesting half, which is still blocked.

## Gaps deliberately not chosen

| Gap | Why it waits |
|---|---|
| Why PartDesign does not run here | Blocks round 008 and the interesting half of the gap above, but is an environment problem, not a product question |
| Sketcher external geometry | The most cited real-world toponaming pain, and it has its own storage path worth a round |
| Principled abstention thresholds | Round 004's gap; limits repair rates |
| The three unexplained App failures | Open since round 001 |
