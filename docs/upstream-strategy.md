# DiofanCAD — Upstream Strategy

## Constitutional objective

DiofanCAD exists to expand what FreeCAD can become, not to destroy the project it came from.

## Change classes

```text
U0 — Upstream-ready
U1 — Upstream candidate
U2 — Experimental
U3 — DiofanCAD-only
```

Add an explicit state for external reality:

```text
UPSTREAM-ACCEPTED
UPSTREAM-MAINTAINED
```

`U0/U1` describe readiness; they do not imply acceptance by upstream.

## Extraction pipeline

```text
Experimental code
 ↓
Stabilize
 ↓
Remove lab-only assumptions
 ↓
Reduce scope
 ↓
Split by concern
 ↓
Tests
 ↓
Docs
 ↓
Compatibility audit
 ↓
Performance audit
 ↓
Upstream Gauntlet
 ↓
Issue / RFC
 ↓
Small PR series
 ↓
Upstream review
```

## Divergence record

At each milestone record:

```text
upstream commit
DiofanCAD commit
upstream age
changed files
changed subsystems
API divergence
data-model divergence
patch count
conflict rate
merge cost
migration cost
upstream candidates
```

## Permanent divergence

Permanent divergence is allowed only when:

```text
user/product value is material
AND
upstream architecture cannot reasonably host the capability
AND
a concrete integration boundary exists
AND
maintenance cost is explicitly accepted
AND
migration/export behavior is documented
AND
an architecture decision records the exception
```

No permanent divergence is justified merely because it is faster to implement locally.

## Gates

A change cannot be promoted toward upstream without the V7 Upstream Gauntlet:

```text
[ ] problem statement
[ ] reproduction
[ ] narrow scope
[ ] tests
[ ] documentation
[ ] API impact understood
[ ] compatibility impact understood
[ ] addon/workbench impact assessed
[ ] performance measured
[ ] license/dependency audit
[ ] reviewable boundary
[ ] rollback strategy
```

## Source of truth

Track upstream work in `docs/upstream-backlog.md`; maintain evidence links in `docs/evidence-ledger.md`.
