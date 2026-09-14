# Round 004 — Largest Gap

**Largest Gap:** the abstention thresholds decide every result in these rounds, and nobody has a principled basis for them.

## Why this one

Round 003's near-miss and round 004's clean sheet both hinge on the same two
numbers: `score >= 0.50` and `margin >= 0.05`. I chose them. In round 003 a
margin of 0.042 against a threshold of 0.05 was the only thing standing between
a correct abstention and a confident mis-repair.

Every "0 wrong" in rounds 003 and 004 is therefore conditional on a guess. A
threshold slightly lower turns safe abstentions into wrong repairs; slightly
higher turns correct repairs into abstentions. The measurements cannot be
trusted as *rates* until the thresholds are derived rather than picked — for
example by calibrating the margin against observed correctness over a much
larger scenario set, so that a stated confidence means something.

## Gaps deliberately not chosen

| Gap | Why it waits |
|---|---|
| Provenance as a score bonus rather than a filter | Reasoned to be inert on these scenarios and untestable without a case where a tag survives alongside ambiguity |
| Edge and vertex references | Named since round 002, still unmeasured; fillets and chamfers reference edges |
| Sketch external geometry | The most cited real-world pain; needs Sketcher built |
| Document-level recompute | Still blocked by the material-library limitation |
| How often an engineer accepts a proposal | Round 003's gap, still open, and needs humans rather than a harness |
