# Round 009 — Consumer Inventory — Verdict

## The fragile consumers are exactly two, and both have a modern replacement that is not fragile.

| # | Requirement | Result |
|---|---|---|
| B1 | Every class enumerated, by two search axes | PASS — class axis and behaviour axis agree |
| B2 | Classified from storage type **and** `Save()` | PASS |
| B3 | Feature-level counts, not just classes | PASS |
| B4 | Three controls reported | PASS |
| B5 | PartDesign by source reading only, and said so | PASS |
| B6 | No upstream divergence | PASS — no files changed in `src/` |

## The count

| Storage | Features | Where |
|---|---:|---|
| Mapped-name capable (`PropertyLinkSub*`, `PropertyXLinkSub*`) | **48 headers** | Part 12, PartDesign 11, Measure 8, Fem 6, Surface 6, TechDraw 4, Sketcher 1 |
| **Raw index** (`PropertyFilletEdges`) | **2 features** | `Part::Fillet`, `Part::Chamfer` |

Both raw-index features derive from `Part::FilletBase`, and `FilletBase` has
exactly two derivatives. The modern equivalents — PartDesign's dress-up
features — use `App::PropertyLinkSub Base`, the mapped-name path.

## What this answers

Round 007's surviving finding was that `Part::Fillet` stores
`FilletElement { int edgeid; }` and therefore cannot benefit from the element
map. That was true, and it prompted the question of how many consumers are
built that way.

The answer is **two**, in the legacy Part workbench, each with a PartDesign
successor on the robust path. The "consumers are the problem" reading of
D-013 does not survive this.

## Route 2, three ways

Per D-019 this claim needed establishing independently of the survey:

1. **What the class persists.** `PropertyFilletEdges::SaveDocFile` writes
   `it.edgeid << it.radius1 << it.radius2` — an integer and two doubles, no
   name. Read from the persistence path rather than the struct declaration, so
   a shadow stored elsewhere would have shown up.
2. **Empirically, on the robust side.** The P2 probe in the working Part build
   returns `mapped=';Face2;:Hce7,F.Face6'` against a `Part::Cut`.
3. **Completeness, by a different axis.** 51 `.cpp` files manipulate subnames
   against 55 class-uses — same order. The only sub-element members outside the
   class list are `Part::Attacher::subnames`, populated from
   `AttachmentSupport` (a `PropertyLinkSubList`, and it carries `shadowSubs`),
   and `Sketcher::ExternalToAdd::subname`, a transient argument struct. Neither
   is independent storage.

The third was the one that mattered. The bar said that if it found a mechanism
the class list had missed, the counts would be **withdrawn rather than
adjusted**. It did not.

## Scrutiny note

Under §1.3 this result gets *less* scrutiny than the alternative would have,
and deliberately so. "Two features are fragile" says the baseline is broadly
fine — the direction the four previous errors never went. Had the survey
returned "most consumers store raw indices", the same evidence would not have
been enough.

## What this does NOT establish

**Capability is not population.** Forty-eight headers *can* store mapped names.
Whether they *do* in a real model is a different question, and round 007 found
a case where a `PropertyLinkSub` stored an **empty** mapped name because the
feature it referenced was a primitive with no element map.

So the inventory says the storage mechanism is right almost everywhere. It says
nothing about how often that mechanism is actually populated — which is the
next question and this round's largest gap.

## Decision

The consumer-side fragility is real but narrow: two legacy features. It is not
a programme-level target, and it is a plausible small upstream contribution
(make `Part::Fillet` and `Part::Chamfer` store mapped names like their
PartDesign counterparts) rather than a DiofanCAD differentiator.

`docs/decisions.md` D-013 — "the differentiation is diagnosis, not survival" —
stands, but its consumer-coverage reading is closed by this round.
