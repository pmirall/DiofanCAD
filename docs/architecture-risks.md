# DiofanCAD — Architecture Risks

## Status

- Document class: Architecture risk register
- Pass: 1
- Evidence status: INITIAL

## Risk scale

- Critical: can invalidate the program thesis or create uncontrolled fork divergence.
- High: can materially delay or reshape the architecture.
- Medium: localized but consequential.
- Low: manageable with normal engineering controls.

## Initial risk register

| ID | Risk | Why it matters | Evidence needed | Initial severity | Mitigation |
|---|---|---|---|---|---|
| AR-001 | Semantic layer becomes a second source of truth | Creates duplication and permanent divergence | architecture proof P0-B | Critical | explicit ownership boundary |
| AR-002 | Persistent references fail on ambiguous topology changes | Core product wedge depends on robust identity | P0-A + realistic change tests | Critical | confidence + diagnostics + transactional repair |
| AR-003 | Recompute redesign becomes too invasive | Could destabilize existing FreeCAD behavior | dependency map + P0-C | **Reduced: High** (P0-C) | instrument first; rewrite only with evidence |
| AR-004 | Assembly/configuration state explodes combinatorially | Cross-domain semantics can multiply state | model/provenance prototype | High | explicit configuration scope / inheritance |
| AR-005 | Native file compatibility breaks | Undermines user ownership and ecosystem | compatibility suite | Critical | round-trip tests + migration strategy |
| AR-006 | Fork divergence exceeds maintenance capacity | Upstream path becomes unrealistic | divergence report per milestone | Critical | divergence budget + extraction cadence |
| AR-007 | Governance overhead exceeds engineering throughput | Process becomes a bottleneck | milestone throughput data | Medium | evidence minimums, not paperwork maximums |
| AR-008 | Benchmarks optimize wrong behavior | Can produce false competitive wins | blind A/B + human testing | High | benchmark review and formative testing |
| AR-009 | Project scope expands into full CAD/PDM/CAE stack | Capacity becomes insufficient | capacity ledger | Critical | wedge gating + kill criteria |
| AR-010 | AI masks weak semantic APIs | Automation demo can hide architectural debt | API proof before AI | High | AI remains downstream of semantic API |

## Risk movement

### AR-003 — reduced from Critical to High on 2026-09-14

P0-C showed that the recompute layer can be made fully diagnosable — per-object
timing plus causality — with 36 guarded lines and no semantic change
(`gauntlet/rounds/architecture/round-001-p0c/verdict.md`). The fear behind
AR-003 was that understanding recompute would require rewriting it. It did not.

It is reduced, not closed: making recompute *diagnosable* is not the same as
making it *faster* or *deterministic*, and neither of those has been attempted.

### AR-010 note

Unchanged, but reinforced: this round put semantic APIs and measurement first
and added no AI, which is the ordering AR-010 demands.

## Mandatory architecture review triggers

Trigger review on:

- repeated failure of the same architecture proof;
- >2 major rewrites of the same boundary;
- compatibility regression that cannot be migrated safely;
- divergence budget breach;
- no reproducible evidence after the agreed iteration budget;
- discovery of a simpler equivalent architecture.

## Required next action

Turn each Critical and High risk into one executable proof, benchmark or decision record before the related subsystem becomes a committed production workstream.
