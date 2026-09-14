# Round 002 — Largest Gap

**Largest Gap:** when a reference breaks, nothing proposes the single face that still matches it — the information needed for repair exists, and is discarded.

## Why this one

Eight references broke across the six scenarios. In all eight, exactly one
face in the changed shape matched the stored descriptor. The baseline marks
the reference with `?` and stops. No candidate, no confidence, no repair, and
in practice almost nothing consumes even the `?`.

That is a gap the programme can close without touching identity, which is the
cheapest useful thing P0-A could have discovered.

## Gaps deliberately not chosen

| Gap | Why it waits |
|---|---|
| Ambiguity on hard geometry | The decisive follow-up, but it needs fillets, revolutions and coplanar faces, not boxes. It is round 003's question, and it can still kill the repair idea |
| Document-level measurement | `Part::Box` needs the material library, which a minimal build does not install. TopoShape level exercises the same element map |
| Edges and vertices | Only faces were measured. Edge references are what fillets and chamfers actually use |
| Sketch external geometry | The most cited real-world toponaming pain; needs Sketcher built |
| Save/reload of references | That is P0-D, not this round |
