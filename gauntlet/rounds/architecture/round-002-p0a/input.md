# Round 002 — Input: what the baseline does with element identity

Inspection at the fork baseline, before writing the harness.

## The machinery that already exists

| Component | Lines | What it does |
|---|---:|---|
| `src/App/ElementMap.*` | 1908 | persistent map between `MappedName` (stable) and `IndexedName` (Face3, Edge7) |
| `src/App/MappedName.*` | 1501 | the stable name type, with encoding and hashing |
| `src/App/MappedElement.*` | 419 | the pair |
| `src/App/ElementNamingUtils.*` | 270 | naming grammar: tags, postfixes, prefixes |
| `src/App/ComplexGeoData.cpp` | 748 | `getMappedName`, `getIndexedName`, `getElementName` |
| tests | 1933 | `tests/src/App/{ElementMap,MappedName,MappedElement,ElementNamingUtils}.cpp` |

The naming grammar is richer than "toponaming is broken" suggests. It encodes
provenance directly in the name: `;:H` tag, `;:M` modified, `;:G` generated,
`;:U`/`;:L` hierarchy, `;:C` child, `;:I` array index. `ElementMap` also
exposes `getElementHistory()`, so an element's ancestry is queryable.

## What happens when a reference breaks

`App::GeoFeature::_getElementName` (`src/App/GeoFeature.cpp:113`):

```cpp
else if (mapped.name) {
    // deliberately mangle the old style element name to signal a
    // missing reference
    ss << Data::MISSING_PREFIX << dot + 1;
```

The baseline's complete answer to a broken reference is to prefix it with `?`,
detectable via `Data::hasMissingElement()`. Across the whole tree, exactly one
place consumes that signal to do anything for the user
(`src/Mod/PartDesign/Gui/TaskDressUpParameters.cpp:507`).

## The gap, stated against V7 §18

| §18 requirement | Baseline |
|---|---|
| Persistent identity | **present** — `MappedName` |
| Topology mapping | **present** — `ElementMap` |
| Geometry descriptor | absent |
| Semantic role | absent |
| Candidate matching | absent |
| Confidence | absent |
| Repair | absent |
| Diagnostics | `?` prefix, essentially unconsumed |

So the honest framing is not "FreeCAD has no stable references". It is:
**the baseline can persist and detect, but cannot explain, rank or repair.**

## What is unmeasured, and why it matters most

Every existing test asks whether a *name* is correct. None asks whether the
name still points at the *face the engineer meant* after a change. Those differ
precisely in the dangerous case: a name that resolves to the wrong face passes
every name-level check.

That is the measurement this round builds, and it is the reason Phase 1 comes
before any new mechanism.

## REUSE / BUILD / INTEGRATE / DEFER

| Concern | Decision | Rationale |
|---|---|---|
| Persistent identity | **REUSE** | `MappedName`/`ElementMap` exist, are tested, and are the upstream direction of travel |
| Element history | **REUSE** | `getElementHistory()` already tracks ancestry |
| Change-challenge measurement | **BUILD** | nothing measures semantic survival; this is the instrument the wedge needs |
| Geometric descriptors | **BUILD (test-only prototype)** | needed for ground truth; whether it belongs in the product is a later decision |
| Candidate matching / confidence / repair | **DEFER to Phase 2** | only justified if Phase 1 finds real failures |
| A replacement naming scheme | **NEVER** on current evidence | the baseline's grammar is richer than the alternatives being imagined |
