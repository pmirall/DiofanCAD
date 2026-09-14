# Round 004 — Does Provenance Disambiguate What Geometry Cannot? — Bar

Written before the measurement was run, including a prediction that the
measurement can contradict.

## The question

Round 003 left one gap: a geometric descriptor scored 1.0 on the **wrong** hole
after a resize, and only an arbitrary margin threshold prevented a confident
mis-repair. The descriptor does not contain enough information to tell two
repeated features apart.

So: is there information elsewhere that does?

## Two candidate mechanisms

**M1 — Provenance.** The baseline already encodes which feature generated an
element: the naming grammar carries tags (`;:H`) and operation codes, and
`ElementMap::getElementHistory()` returns an element's originating tag and
ancestry. A stored reference can therefore remember *what made* its face, not
only what the face looks like.

**M2 — Exclusion by surviving references.** A document holds many references,
not one. If another reference still resolves to a candidate face, that face is
already claimed and cannot be the repair target for a different broken
reference. This needs no name parsing at all — only the set of references the
document already has.

## Prediction, recorded before measuring

> **M1 alone will not disambiguate H4. M2 will.**
>
> In H4 the stored reference's generator (tag 3) no longer exists, so
> provenance can say *"your generator is gone"* — diagnostic, but not
> identifying. Neither surviving candidate carries tag 3. Exclusion, by
> contrast, should remove the hole at x=12 because its own reference still
> resolves, leaving the correct hole as the only unclaimed candidate.

If the measurement contradicts this, the prediction is what was wrong, and it
stays in the record either way.

## Scenarios — all must actually break references

Round 003's weakness was that only one scenario forced a break. This round
needs several:

| ID | Break cause | Expected difficulty |
|---|---|---|
| R1 | one hole deleted and re-created, block lengthened (round 003's H4) | exclusion should solve it |
| R2 | **two** holes deleted and re-created, block lengthened | both claimants gone, so exclusion has less to work with — the honest limit |
| R3 | a feature inserted upstream, modifying outer faces | breaks faces whose generator is unchanged but whose shape was modified |
| R4 | the cutting tool replaced by a different solid | breaks every face the old tool generated |

## Three strategies compared on the same breaks

| Strategy | Uses |
|---|---|
| **G** | geometry only — round 003's matcher, unchanged |
| **G+E** | geometry, then candidates claimed by a surviving reference are excluded |
| **G+P** | geometry, then candidates filtered by originating tag |

## Bar

| # | Requirement | Measured how |
|---|---|---|
| B1 | At least four scenarios that genuinely break references | break count reported per scenario |
| B2 | Negative control still fires | N1 carried forward |
| B3 | Ground truth remains independent of every strategy | absolute predicates, as in round 003 |
| B4 | All three strategies measured on identical breaks | one table, same inputs |
| B5 | **Mis-repair rate reported per strategy** | the decisive number |
| B6 | The lone-reference limit is measured, not assumed | R2, where exclusion has little to exclude |
| B7 | No upstream divergence | test file only |

## Critical failures

- **Any strategy mis-repairs at high confidence** → that strategy is unusable, whatever its success rate.
- Exclusion succeeds only because the harness feeds it information a real system would not have → the result is void. Exclusion may use only references the document itself holds.

## What a kill looks like

If neither mechanism beats geometry alone, then candidate-ranked repair has no
reliable disambiguator, and D-012's "engineer confirms" becomes the *whole*
feature rather than a safety net: the system reports what broke and shows
candidates it explicitly cannot rank. That is still worth more than today's
silent `?`, but it is a much smaller claim, and the wedge should move to
diagnosis (D-013) rather than repair.
