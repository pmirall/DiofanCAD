# Round 004 — Input

## Where the question came from

Round 003's largest gap: a geometric descriptor cannot distinguish repeated
features after a resize. The obvious place to look for the missing information
was provenance — the baseline already encodes which feature generated an
element, and `ElementMap::getElementHistory()` returns an element's originating
tag and ancestry.

This round tests that, and one alternative that occurred to me while writing
the bar: a document holds *many* references, and a face already claimed by a
reference that still resolves cannot be the repair target for a different
broken one.

## What the baseline provides

| Mechanism | Access | What it gives |
|---|---|---|
| `ComplexGeoData::getElementHistory(name, original*, history*)` | `TopoShape` | the originating tag and the ancestry chain |
| `MappedName::toString()` | any mapped name | the encoded name, including tags (`;:H`) and operation codes |
| the set of a document's own references | the document | which faces are already spoken for |

The third is not a FreeCAD mechanism at all. It is a property of how models are
actually used, and it costs nothing to exploit.

## Three strategies

```
G    geometry only (round 003's matcher, unchanged)
G+E  geometry, minus candidates claimed by a surviving reference
G+P  geometry, filtered to candidates whose originating tag matches
```

## Scenarios, all of which must break references

Round 003 could only force one break. This round engineers five:

| ID | Break cause |
|---|---|
| R1 | one hole deleted and re-created, block lengthened |
| R2 | **two** holes re-created — fewer surviving claimants |
| R3 | a feature inserted upstream, modifying an outer face |
| R4 | the cutting tool replaced by a different solid |
| R5 | R1 again, but the document holds a **single** reference |

R5 exists to stop exclusion from looking better than it is.
