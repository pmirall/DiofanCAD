# Round 010 — Largest Gap

**Largest Gap:** the programme has spent eight rounds characterising a mechanism and has yet to measure anything a user would notice.

## Why this one

The identity layer is now well understood: it exists on derived features,
references populate from it exactly, population is durable, references survive
change and save/reload, repair is possible but must abstain under ambiguity,
and exactly two legacy features bypass it.

None of that is a product claim. The charter's wedge asks whether DiofanCAD
preserves engineering intent through change **better than a benchmark**, and
across ten rounds the only comparison ever made was against FreeCAD itself.
`docs/fusion-benchmark-plan.md` has never been run. No golden workflow has been
executed. `docs/evidence-ledger.md` contains thirty-one entries and not one is a
competitive claim.

The honest reading is that the programme has been doing the cheap, safe half of
V7 — characterising the baseline — and has not started the expensive half.

## What the next round should be

Not another element-map round. Either:

1. **A golden workflow, end to end.** `docs/golden-workflow-suite.md` lists
   thirteen; the bracket is the simplest. Model it, change it, document it,
   measure the time and the failures. That is the first thing in this
   repository a user would recognise.
2. **Or an explicit decision to stop**, recorded against V7 §77's kill
   criteria, on the grounds that a fork cannot run the comparison the charter
   demands without Fusion 360 access and human testers — neither of which this
   programme has.

Option 2 is not defeatism. `docs/fusion-benchmark-plan.md` requires blind A/B
against a commercial product and `docs/human-blind-protocol.md` requires real
engineers. Ten rounds have produced no plan for obtaining either, and
continuing to measure the baseline while the competitive half stays untouched
is how a programme quietly becomes a research project.

## Gaps deliberately not chosen

| Gap | Why it waits |
|---|---|
| Why PartDesign does not run here | An environment problem; it blocks rounds, not the product question |
| Sketcher external geometry | Another baseline-characterisation round |
| Principled abstention thresholds | Round 004's gap; repair is modest and may not be built |
| P0-B restatement | D-010 removed its purpose |
