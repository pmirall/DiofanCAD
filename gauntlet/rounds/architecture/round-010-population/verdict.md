# Round 010 — Mapped-Name Population — Verdict

## The rule holds exactly, and it is simple.

> **A reference holds a mapped name iff the feature it points at has an element
> map that resolves the subname.** Population follows the target, not the
> consumer.

Predicted from reading `PropertyLinkBase::_updateElementReference` before
measuring; confirmed in both directions on all three target kinds.

| Target | element map | reference populated | stored |
|---|---:|---:|---|
| `Part::Box` (primitive) | none | **no** | `old='Face2'` only |
| `Part::Cut` (boolean) | 50 | **yes** | `;Face2;:H1073,F.Face6` |
| `Part::Fillet` (dress-up) | 56 | **yes** | `;Face2;:H1073,F;:H1075,F.Face7` |

| # | Requirement | Result |
|---|---|---|
| B1 | Three target kinds | PASS |
| B2 | Reported as a correlation, never a bare rate | PASS |
| B3 | All three controls in the same run | PASS |
| B4 | Re-checked after recompute and after a change | PASS — M3 |
| B5 | Fallback described and classified | PASS — **SILENT** |
| B6 | No upstream divergence | PASS — one new test file |

## What this closes

Round 009's gap was the third link in a three-link chain, and it is now
measured:

```
the consumer CAN store a mapped name      48 of 50 headers        round 009
the referenced feature HAS a map          derived features do     round 007 (corrected)
the consumer DOES hold a name             iff the target has one  round 010
```

So the survival numbers in rounds 002–005 describe real behaviour **wherever
the referenced feature is a derived one**, which is everywhere except
primitives. That is a much narrower caveat than round 007's retracted headline
implied, and it is now positively established rather than assumed.

## Population is durable

A mapped name is recomputed state, so it could in principle be transient. It is
not: identical across an idle recompute and across a parameter change that
rebuilt the shape.

## The one finding worth keeping

**The fallback is silent.** A reference to an unmapped target is stored as a
bare index name with an empty mapped name, and nothing marks it as resting on a
weaker identity. The `?` missing marker appears only when a reference *breaks* —
never when one is *created* without the stronger backing.

So two references that look identical in the UI can have quite different
robustness, and the system does not say which is which. That is small, real,
and squarely in D-013's territory: it is a **diagnosis** gap, not a mechanism
gap. A Model Doctor that reported "this reference is backed by identity, that
one by an index" would be telling the user something FreeCAD currently knows
and does not surface.

## What is deliberately not reported

**Any population rate.** The bar committed to this before measuring: the
fraction of populated references in a model is a function of what the model
references, so a number would describe my fixture. If a rate appears anywhere
downstream of this round, it was invented.

## Verification pass

Per §1.4, before writing this: `Cut` map size 50 here matches round 007's
corrected probe exactly — independent reproduction with a different harness.
`Fillet` is 56 here against 32 in round 007, explained by the fixture (round 007
filleted a `Box`, this round fillets a `Cut`). The hash segment varies between
tests but is identical across three readings inside one document, consistent
with it encoding the object tag — stated as the plausible reading, not proven.
And the fillet's name chains two hashes, corroborating round 004's reading that
the grammar encodes the generator.

## Caveats

Part workbench only; PartDesign still cannot be run here. Three targets, one
reference each. Cross-session stability of the hash is untested: it is stable
within a document, and round 007 saw a fillet survive `saveAs`/reopen, but that
is not the same measurement.
