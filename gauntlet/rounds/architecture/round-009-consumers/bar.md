# Round 009 — Consumer Inventory — Bar

Written before the measurement. First round under the D-019 methodology, so the
pre-flight is filled in rather than implied.

## Pre-flight

**The claim this round could make, in one sentence:**
> *N* of the *M* FreeCAD features that reference sub-elements store raw indices
> rather than mapped names, and therefore cannot benefit from the element map
> however good it is.

**ROUTE 2 — independent of the grep survey:**
1. For each property class found, read what its `Save()` actually persists. A
   class that writes an integer cannot be carrying a name.
2. Empirically confirm one consumer of each kind in the working Part build:
   a `PropertyLinkSub` (known to store `;Face2;:H346,F.Face6` from round 007's
   P2 probe) and a `PropertyFilletEdges`.
3. Completeness check by a different search axis: rather than enumerating
   property *classes*, search for the *behaviour* — features that call
   `getSubValues`, `setSubValues`, or persist a subname — and confirm the class
   list accounts for all of them.

**NEGATIVE control:** the classification must separate two cases already known
from earlier rounds — `Part::Fillet` (raw index, read from source in round 007)
and a `PropertyLinkSub` consumer (mapped name, measured in round 007 P2). A
method that cannot tell those apart is broken and its counts mean nothing.

**POSITIVE control:** mandatory, because "no consumer stores mapped names"
would be a dramatic absence claim. The probe must show a non-empty mapped name
where one is known to exist — round 007's P2 result is that case.

**EXERCISED check:** report counts of files and classes actually scanned, not
only the classification. A survey that scanned nothing also finds nothing.

**Would this result be GOOD for the programme?** Yes — "most consumers are
fragile" supports the wedge and is exactly the shape of result that has been
wrong four times. So route 2 is mandatory and so is the answer below.

**What would make this wrong?**
- `PropertyFilletEdges` might carry shadow names somewhere I have not read.
- Features might convert indices to mapped names at save or recompute time.
- **My enumeration may be incomplete.** This is the real risk: I would be
  counting the consumers I found, not the consumers that exist. Route 2's third
  item exists specifically to attack this, and if it finds a mechanism the class
  list missed, the counts are withdrawn rather than adjusted.

## Omission, found afterwards by the checker

`tools/diofancad/check-round.sh` flags this round for having **no recorded
prediction**. That is correct: the pre-flight states the claim the round could
make, but never commits to what the answer would be.

No prediction is added here retroactively. Writing one after seeing the result
would be worth less than nothing — it would make the record look disciplined
where it was not. Round 010 records a real prediction; this one did not.

The checker also found `evidence/` empty. That has been fixed with the raw
survey output, which is regenerable because the queries are deterministic.

## Bar

| # | Requirement |
|---|---|
| B1 | Every sub-element-referencing property class in the tree is enumerated, by two different search axes |
| B2 | Each class classified as mapped-name-capable or raw-index, from its storage type **and** its `Save()` |
| B3 | Feature-level counts: which features use which, not just which classes exist |
| B4 | The three controls reported |
| B5 | PartDesign classified by source reading only, and said so — it cannot be run here |
| B6 | No upstream divergence |

## Critical failures

- Route 2's completeness check finds a reference mechanism the class list missed → counts withdrawn, not patched.
- Any count reported without the file/class scan totals beside it.

## Out of scope

Expressions, spreadsheet references, and anything in the GUI layer. Sketcher
external geometry, which deserves its own round.
