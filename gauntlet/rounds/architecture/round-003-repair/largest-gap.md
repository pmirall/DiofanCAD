# Round 003 — Largest Gap

**Largest Gap:** a geometric descriptor cannot distinguish repeated features after a resize, so the only safe repair is one a human confirms — and nothing yet measures how often an engineer would accept the proposal.

## Why this one

H4 showed the wrong hole scoring 1.0. The fix is not a better threshold; the
descriptor does not contain the information. What could contain it is
provenance — which feature generated this face, what its parameters were,
where it sat in the feature order — and the baseline already encodes some of
that in the naming grammar (`;:H` tag, `;:M` modified, `;:G` generated) and in
`ElementMap::getElementHistory()`.

So the next question is whether provenance disambiguates what geometry cannot.
That is measurable with the harness already built, and it decides whether
ranked-candidate repair is worth building at all.

## Gaps deliberately not chosen

| Gap | Why it waits |
|---|---|
| Edge and vertex references | Named in round 002 and still unmeasured. Fillets and chamfers reference edges, so this is real, but it does not change the repair decision |
| More break scenarios | The repair path has one data point. More would sharpen the rate but not the conclusion |
| Sketch external geometry | The most cited real-world pain; needs Sketcher built |
| Document-level recompute | Blocked by the material-library limitation from round 002 |
| Model Doctor framing | The verdict points at it, but it is a product question, not a proof |
