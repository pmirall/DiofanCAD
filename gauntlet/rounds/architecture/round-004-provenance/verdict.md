# Round 004 — Provenance vs Exclusion — Verdict

## PASS. The prediction held, and the loser was worse than predicted.

| # | Requirement | Result |
|---|---|---|
| B1 | Four+ scenarios that genuinely break references | PASS — 5 scenarios, 7 breaks |
| B2 | Negative control fires | PASS — N2 |
| B3 | Ground truth independent of every strategy | PASS — absolute predicates |
| B4 | All three strategies on identical breaks | PASS |
| B5 | Mis-repair rate per strategy | PASS — **0 in 21 strategy-decisions** |
| B6 | Lone-reference limit measured | PASS — R5 |
| B7 | No upstream divergence | PASS — one new test file |

## The result

Seven broken references, three strategies each:

| Strategy | correct | **wrong** | abstain |
|---|---:|---:|---:|
| G — geometry only | 3 | **0** | 4 |
| **G+E — geometry + exclusion** | **4** | **0** | 3 |
| G+P — geometry + provenance filter | 1 | **0** | 6 |

## Finding 1 — exclusion works, and is free

The prediction written before measuring was that exclusion would solve R1 where
provenance could not. It did: excluding the hole already claimed by a surviving
reference turned a 0.042 margin into a 0.958 one, and the repair was correct.

G+E matched or beat G on every break and was never worse. It needs no name
parsing, no new persistent data, no change to `ElementMap` — only the set of
references the document already holds.

## Finding 2 — provenance is worse than useless as a filter

G+P lost both R4 cases that geometry alone got right. When the cutting tool is
replaced, its tag no longer exists, so a tag filter rejects every candidate and
abstains on a question geometry could answer.

The reason is worth stating precisely, because it generalises:

> **Provenance is not independent information about the broken case.** When the
> originating tag survives, the reference itself usually survives too, so
> provenance has nothing to add. When the reference breaks because its generator
> changed, the tag has broken with it.

G+P's single success was R3 — the one scenario where the tag survived. And
there, geometry already worked.

This kills the hypothesis that round 003's largest gap pointed at. Provenance
was the obvious place to look for the missing information. It is not there.

## Finding 3 — exclusion's power is borrowed from the document

R5 is R1 with one reference instead of four. G+E degrades **exactly** to G and
abstains.

So exclusion does not make references more repairable; it exploits the fact
that a real model has many references, most of which survive. A sparsely
referenced model gets no benefit. That is a genuine design constraint, and it
means any benchmark of a repair feature must state how many references the
model carried.

## Finding 4 — abstention is the common outcome

Even with the best strategy, 3 of 7 breaks end in "I cannot tell". That is the
safe answer and the right one, but it is not a fix. A repair feature will spend
much of its time saying it does not know.

## Decision

- **Adopt exclusion by surviving references** as the disambiguator, if a repair
  feature is built. Cheap, strictly better, degrades safely.
- **Do not use provenance as a hard filter.** It rejects correct answers in
  precisely the cases repair exists for.
- **D-012 stands and is reinforced**: 0 mis-repairs in 21 decisions, but only
  because every strategy abstains readily. Repair proposes; the engineer
  confirms.

## What this means for the wedge

Rounds 002–004 together give the honest shape of the thing:

- references almost never break (8 of 60 in round 002; 0 of 8 on the hard
  geometry of round 003; round 004's breaks had to be engineered);
- when they do break, the best available strategy proposes a correct candidate
  about half the time and a wrong one never;
- the rest of the time it says it cannot tell.

That is a real feature, and a modest one. It is worth more than the baseline's
silent `?`, and it is not the transformative "change resilience" the wedge was
originally framed around. D-013's reading — that the differentiation is
**diagnosis**, not survival or repair — survives this round strengthened.

## Caveats

Provenance was tested as a hard filter. As a score bonus it would be inert on
these scenarios: no candidate carries the stored tag in any broken case, so the
bonus is zero for everything and the ranking is identical to G. Never harmful,
never helpful — but untested in the case where a tag survives alongside real
geometric ambiguity.

Seven breaks, five scenarios, box-and-cylinder geometry, faces only. The
abstention thresholds remain unprincipled.
