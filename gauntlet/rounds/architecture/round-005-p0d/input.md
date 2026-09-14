# Round 005 — Input

## Why P0-D rather than round 004's gap

Round 004's largest gap — unprincipled abstention thresholds — limits how far
the repair *rates* can be trusted. Repair is a modest feature (D-014).

P0-D sits underneath all of it. Rounds 002–004 measured reference survival on
shapes that had never been persisted. A real model is always saved and
reopened. If identity does not survive that boundary, three rounds of numbers
describe something that does not exist.

P0-D is also a required member of the Phase 0 proof set, which cannot exit
without it.

## Reframed by D-010

P0-D was written assuming DiofanCAD would add its own persistent semantic
metadata to test across a save. D-010 decided it will not: `ElementMap` and
`MappedName` are reused unchanged. So the proof becomes a question about the
baseline — does its identity survive its own persistence boundary, and do the
earlier rounds' findings hold after a reload?

## The persistence path

| API | Role |
|---|---|
| `ElementMap::beforeSave(hasher)` | marks StringIDs for saving |
| `ElementMap::save(ostream)` | writes the map |
| `ElementMap::restore(hasher, istream)` | reads it back |
| `ComplexGeoData::resetElementMap(ptr)` | detaches or attaches a map; the public accessor |

`ComplexGeoData::elementMap()` is protected, so the harness gets the map by
calling `resetElementMap()` on a copy, which returns the detached map.

## The string table is separate state

Mapped names can reference `App::StringID` values held by an `App::StringHasher`
that is persisted separately. That makes a partial restore — map present,
string table lost — a real compatibility question under §52.5, and it is why M4
and M5 exist.
