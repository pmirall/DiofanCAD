# DIOFANCAD — FUSION-KILLER SPEC V7

**Master engineering specification — V7 audited execution charter**  
**Project name:** DiofanCAD  
**Upstream project:** FreeCAD  
**Status:** Audited design / execution charter  
**Branch concept:** `feature/diofancad-evolution`  
**Mission:** Build an open-source engineering CAD platform derived from FreeCAD that can become demonstrably superior to Fusion 360 on important engineering workflows while strengthening the upstream FreeCAD ecosystem.

---

# 0. PROJECT IDENTITY

## 0.1 Product name

**DiofanCAD**

## 0.2 Product descriptor

**Open Engineering CAD**

## 0.3 Project relationship to FreeCAD

DiofanCAD is an experimental/product evolution layer derived from the FreeCAD codebase.

The relationship is:

```text
                    FREECAD UPSTREAM
                           ▲
                           │
                    useful contributions
                           │
                   DIOFANCAD LAB
                           │
                  experiments / product
                           │
                   benchmarks / Gauntlet
                           │
                       agents
```

DiofanCAD must not be positioned as a hostile replacement for FreeCAD.

Its purpose is to explore how far an open-source engineering CAD platform can go while continuously producing useful, reviewable improvements that can return upstream.

## 0.4 Naming rule

Use:

- **DiofanCAD** for the product/branch/lab.
- **FreeCAD** for the upstream project.
- **Fusion 360** for the primary commercial benchmark.
- **Fusion Killer** only as an internal mission/evaluation label, never as the primary public product identity.

---

# 0.5 V7 AUDIT AMENDMENT

V7 preserves the V6 product thesis, evaluation discipline and open-source posture, but adds explicit execution controls for the risks identified in the audit.

V7 is governed by five additional principles:

```text
1. Prove architecture before scaling scope.
2. Prefer one source of truth over parallel models.
3. Win one workflow decisively before expanding the platform.
4. Every major idea needs a kill criterion.
5. Fork divergence is budgeted like any other engineering resource.
```

The following controls are mandatory for major workstreams:

- architecture boundary and ownership map;
- measurable exit criteria;
- explicit reuse / build / integrate / defer decision;
- documented divergence budget;
- compatibility and round-trip policy;
- program-capacity and maintenance estimate;
- rollback and kill criteria;
- executable proof-of-value before broad implementation.

V7 therefore treats the repository, benchmarks, Gauntlet state, evidence ledger and architecture records as one engineering control system rather than separate documentation artifacts.

# 1. NAME RATIONALE

The name **DiofanCAD** is inspired by Diophantus, whose work is strongly associated with solving problems through algebraic relationships and unknowns.

This is conceptually aligned with the product thesis:

> Engineering CAD should understand relationships, constraints, intent and dependencies, not only geometry.

This naming rationale is branding guidance, not a requirement that the implementation mimic mathematics literally.

---

# 2. EXECUTIVE DIRECTIVE

You are operating an engineering program, not a feature-generation exercise.

The objective is not:

> make FreeCAD look like Fusion.

The objective is:

> Build DiofanCAD as a materially better engineering system on important workflows, with evidence, while preserving open source, user ownership, interoperability, extensibility, offline operation and a credible path for mature improvements to return to FreeCAD upstream.

The branch may experiment aggressively.

The upstream project remains sovereign.

No architectural or product decision is successful merely because it works locally.

Every major change must be evaluated for:

- correctness;
- robustness;
- maintainability;
- interoperability;
- performance;
- usability;
- testability;
- security;
- upstreamability.

---

# 3. DIOFANCAD NORTH STAR

The ideal workflow:

```text
I know what I want to engineer
              ↓
DiofanCAD understands the context
              ↓
The right action is discoverable
              ↓
The model changes predictably
              ↓
Engineering intent survives
              ↓
Downstream artifacts update
              ↓
The system verifies itself
              ↓
I own the data
              ↓
I can automate the entire process
```

The experience should be:

> engineering-first, not workbench-first.

---

# 4. THE FUSION-KILLER THESIS

Fusion 360 is the benchmark, not the blueprint.

DiofanCAD should compete through:

1. Engineering semantics
2. Robustness under change
3. Low cognitive load
4. Unified workflows
5. Automation
6. Performance and scale
7. Open data ownership
8. Offline operation
9. Open interoperability
10. Verifiable quality

The key product promise is:

> **One engineering model, one semantic identity, one coherent workflow.**

---

# 5. WHAT “FUSION KILLER” MEANS

“Fusion Killer” is a falsifiable internal mission.

It does NOT mean:

- more commands;
- more workbenches;
- more features;
- more code;
- prettier screenshots.

It means:

> For a defined suite of real-world engineering workflows, DiofanCAD demonstrates equal or superior correctness, robustness, efficiency, recovery, usability and integration compared with Fusion 360 while retaining open-source and data-ownership advantages.

A benchmark must be able to conclude:

```text
DiofanCAD wins
Fusion wins
Tie
Unknown
```

---

# 6. COMPETITIVE LANDSCAPE

DiofanCAD must track at least:

- FreeCAD upstream;
- Fusion 360;
- FuCad;
- UniCAD;
- relevant RealThunder/Assembly3 work;
- historical Ondsel work where technically relevant.

These represent different strategies:

```text
FuCad
→ Fusion-like UX

UniCAD
→ Fusion-style unified interaction

RealThunder
→ deep FreeCAD architectural experimentation

Ondsel
→ professional productization + upstream contribution

Fusion
→ mature commercial integrated platform

FreeCAD
→ open-source foundation and ecosystem

DiofanCAD
→ semantic engineering platform + measurable superiority
```

---

# 7. COMPETITIVE LEARNING RULE

For every competitor capability:

```text
Observed behavior
 ↓
User problem
 ↓
Underlying mechanism
 ↓
Our alternative
 ↓
Benchmark
 ↓
Decision
```

Do not copy implementation details blindly.

Copy useful principles only when the underlying problem and architecture justify them.

---

# 8. COMPETITIVE LANDSCAPE GAUNTLET

At milestone boundaries compare:

```text
DiofanCAD
Fusion 360
FuCad
UniCAD
Relevant FreeCAD experimental capabilities
FreeCAD upstream
```

Report:

```text
Our wins
Our losses
Their advantages
Unknowns
Useful techniques
Potential upstream opportunities
Potential differentiation
```

Never optimize against an imaginary competitor.

---

# 9. OPEN-SOURCE STRATEGY

## 9.1 Constitutional rule

> **DiofanCAD exists to expand what FreeCAD can become, not to destroy the project it came from.**

The lab may experiment more radically than upstream.

Mature work should be decomposed into upstream-quality contributions whenever practical.

## 9.2 Three-layer model

```text
                         FREECAD UPSTREAM
                               ▲
                               │
                           PR series
                               │
                  ┌────────────┴────────────┐
                  │                         │
          STABLE CONTRIBUTIONS        DIOFANCAD LAB
                                           │
                                       experiments
                                           │
                                       Gauntlets
                                           │
                                         agents
```

---

# 10. DIOFANCAD CHANGE CLASSES

### U0 — Upstream-ready

Small, isolated, low-risk.

### U1 — Upstream candidate

Requires multiple coordinated PRs.

### U2 — Experimental

High-value, architecture still being proven.

### U3 — DiofanCAD-only

Product experiments that should remain isolated until proven.

Every major subsystem must have a current class.

---

# 11. ANTI-FORK-ROT POLICY

Permanent divergence is a measurable risk.

Track:

```text
upstream age
changed files
changed subsystems
API divergence
data-model divergence
patch count
conflict rate
migration cost
```

If divergence rises beyond an agreed threshold:

```text
STOP FEATURE EXPANSION
↓
SYNC UPSTREAM
↓
ASSESS DIVERGENCE
↓
EXTRACT / REFACTOR / DROP
↓
CONTINUE
```

Fork rot is an engineering defect.

---

# 12. UPSTREAM EXTRACTION PIPELINE

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

Maintain:

`docs/upstream-backlog.md`

---

# 13. UPSTREAM GAUNTLET

A change cannot become U0/U1 without:

```text
[ ] Clear problem statement
[ ] Reproduction
[ ] Narrow scope
[ ] Tests
[ ] Documentation
[ ] API impact understood
[ ] Compatibility impact understood
[ ] Addon/workbench impact assessed
[ ] Performance measured
[ ] License/dependency audit
[ ] Reviewable PR boundary
[ ] Rollback strategy
```

Product quality and upstream quality are different gates.

---

# 14. UPSTREAM SYNCHRONIZATION

Before large experiments:

```text
sync upstream
↓
baseline
↓
experiment
```

Record:

```text
upstream commit
DiofanCAD commit
delta age
conflicts
migration notes
```

Do not silently allow long-lived divergence.

---

# 15. ARCHITECTURAL NORTH STAR

The target architecture is:

```text
                    USER INTENT
                         │
                  CONTEXT ENGINE
                         │
                 SEMANTIC MODEL
                         │
       ┌─────────────────┼─────────────────┐
       │                 │                 │
      CAD             Assembly          Drawing
       │                 │                 │
       ├─────────────────┼─────────────────┤
       │                 │                 │
      CAM               FEM              ECAD
       │                 │                 │
       └─────────────────┼─────────────────┘
                         │
                 ENGINEERING DATA
                         │
                 AGENT / API LAYER
```

Internally modular.

Externally integrated.

---

# 16. SEMANTIC ENGINEERING MODEL

This is DiofanCAD's primary architectural bet.

Target:

```text
Engineering Intent
       ↓
Semantic Objects
       ↓
Geometry + References
       ↓
CAD / Assembly / Drawing / CAM / FEM / ECAD
```

A semantic object contains:

```text
identity
meaning
geometry
parameters
relationships
references
metadata
domain-specific properties
```

Example:

```text
Hole
  semantic_id
  diameter
  depth
  thread
  tolerance
  geometric representation
  assembly interface
  drawing representation
  manufacturing representation
  references
```

---

# 16.5 SEMANTIC OWNERSHIP / SINGLE SOURCE OF TRUTH

The semantic model must not become a second CAD model that duplicates FreeCAD or kernel state.

For every semantic field, record exactly one ownership class:

```text
AUTHORITATIVE
DERIVED
REFERENCED
CACHED
EPHEMERAL
```

Rules:

1. Geometry and kernel topology are never silently duplicated as independent authoritative state.
2. Parameters must have one authoritative owner.
3. Semantic identity may decorate existing objects or reference them, but must not create an untracked parallel object graph.
4. Cached semantic results must be invalidatable and reproducible.
5. Every cross-domain representation must declare whether it is source, projection, or derived artifact.
6. Migration code must preserve ownership semantics across save/reload.

Before introducing a new persistent semantic entity, the implementation must document:

- owner;
- identity source;
- persistence format;
- lifecycle;
- invalidation rules;
- serialization version;
- FreeCAD/kernel relationship;
- rollback behavior.

A semantic subsystem fails architecture review if it creates an undocumented second source of truth.

# 17. DESIGN-INTENT GRAPH

Represent important engineering relationships explicitly:

```text
functional dimension
functional surface
mating interface
hole family
pattern relationship
manufacturing constraint
drawing requirement
simulation requirement
configuration dependency
```

The same intent must be consumable by multiple domains.

---

# 18. STABLE SEMANTIC REFERENCES

Topological naming is one piece of a bigger problem.

Implement:

```text
Persistent identity
+
Semantic role
+
Geometry descriptor
+
Topology mapping
+
Candidate matching
+
Confidence
+
Repair
+
Diagnostics
```

The system should differentiate:

```text
same geometry
```

from:

```text
same engineering meaning
```

References should be explainable and, when safe, repairable.

All repairs are transactional and undoable.

---

# 19. MODEL DOCTOR

Build a first-class health system.

Example:

```text
MODEL HEALTH 96 / 100

✓ 342 stable references
✓ 0 broken dependencies
✓ 0 circular dependencies
⚠ 3 fragile references
⚠ 1 external dependency
✓ Assembly solved
✓ Drawing references valid
```

Actions:

- locate;
- explain;
- repair;
- revalidate;
- export report;
- predict risk.

Long-term:

> predict likely failure before recompute.

---

# 19.5 ARCHITECTURE BOUNDARY CONTRACT

Every major subsystem must publish an ownership boundary before implementation.

Minimum map:

```text
Kernel / geometry library
        │
FreeCAD document model
        │
DiofanCAD semantic services
        │
Domain projections
        │
UI / API / agents
```

For each proposed change record:

```text
What remains in FreeCAD?
What is introduced in DiofanCAD?
What is wrapped?
What is replaced?
What is duplicated?
What is forbidden to duplicate?
```

Duplicating authoritative geometry, parameters or dependency state requires an Architecture Gauntlet exception with explicit evidence.

The default strategy is:

> extend first, wrap second, replace only with evidence.

# 20. DEPENDENCY / RECOMPUTE PLATFORM

Target:

- dependency DAG;
- dirty tracking;
- incremental recompute;
- invalidation;
- caching;
- asynchronous jobs;
- safe parallelism;
- deterministic recompute.

Rule:

> Do not optimize around speed while losing observability or correctness.

---

# 21. PERFORMANCE CONTRACT

Track independently:

```text
startup
open
save
selection
viewport
Sketcher solve
recompute
assembly solve
drawing update
CAM generation
FEM preparation
memory
```

Default regression policy:

```text
<5% slower      acceptable
5–10%            warning
>10%             fail
```

Exceptions require evidence.

---

# 21.5 PERFORMANCE MEASUREMENT CONTRACT

Performance is evaluated by latency class, not by one universal percentage threshold.

```text
INTERACTIVE
  p50 / p95 latency
  frame stability
  input-to-feedback latency

BATCH
  wall-clock time
  throughput

MEMORY
  peak RSS
  steady-state footprint

LARGE MODEL
  time-to-first-visible
  time-to-select
  time-to-edit
  recompute completion
```

Default regression thresholds remain:

```text
<5%      acceptable
5–10%    warning
>10%     fail
```

but thresholds are applied per benchmark class. Critical interactive regressions may fail even when aggregate performance improves.

All performance claims must identify hardware, OS, build, dataset, run count and statistical summary.

# 22. UNIFIED CONTEXT ENGINE

Inputs:

```text
Selection
Active object
Domain
Mode
Document state
Capabilities
```

Outputs:

```text
Relevant actions
Properties
Diagnostics
Suggested next steps
```

Do not create a giant command menu.

Use progressive disclosure.

---

# 23. SKETCHER SUPERIORITY PROGRAM

Priorities:

- snapping;
- external references;
- constraint discoverability;
- centering;
- symmetry;
- patterns;
- interactive feedback;
- semantic references;
- solver performance;
- diagnostics.

Target:

> Create, modify, re-constrain and resize a realistic part while preserving engineering intent.

Measure:

- time;
- interaction count;
- error count;
- recovery time;
- final correctness;
- reference health.

---

# 24. PART DESIGN SUPERIORITY PROGRAM

Cover:

- base features;
- holes;
- pockets;
- fillets;
- chamfers;
- patterns;
- mirrors;
- datums;
- multi-body;
- parameter changes;
- topology-changing edits.

Primary metric:

> How much survives when the engineer changes the design?

---

# 25. ASSEMBLY 2.0

Required:

- source component;
- occurrence;
- subassembly;
- joint;
- grounded state;
- flexible component;
- suppression;
- replacement;
- configurations;
- BOM;
- in-context editing;
- motion;
- interference.

Separate:

```text
source document
```

from:

```text
occurrence
```

Large assembly benchmark:

```text
2500+ occurrences
300+ unique components
100+ joints
3+ nesting levels
```

Measure:

- open;
- first display;
- selection;
- joint editing;
- recompute;
- save;
- BOM.

---

# 26. CONFIGURATIONS

Model:

```text
Product
 ├─ Configuration A
 ├─ Configuration B
 └─ Configuration C
```

A configuration can control:

- dimensions;
- suppression;
- component choice;
- material;
- BOM;
- drawing;
- CAM;
- metadata.

Avoid unnecessary duplication.

---

# 26.5 CONFIGURATION COMPLEXITY CONTROL

Configurations are treated as a state-space multiplier and must not be introduced without explicit complexity accounting.

For each configuration system change record:

```text
configuration scope
inheritance
override precedence
reference semantics
persistence
cache invalidation
cross-configuration references
drawing behavior
BOM behavior
CAM behavior
```

A configuration feature requires a bounded-state strategy. Combinatorial expansion must be measured rather than assumed away.

# 27. TECHDRAW / PRODUCT DOCUMENTATION

Target:

```text
Model
 ↓
Drawing
 ↓
Dimensions
 ↓
Tolerances
 ↓
BOM
 ↓
Revision
 ↓
Release
```

Requirements:

- robust associativity;
- sections;
- details;
- tables;
- BOM;
- templates;
- revision metadata;
- export;
- save/reload verification.

---

# 28. MANUFACTURING / CAM

Target:

```text
Manufacturing Setup
 ├─ Stock
 ├─ Fixtures
 ├─ WCS
 ├─ Machine
 ├─ Tools
 ├─ Operations
 ├─ Post
 └─ Verification
```

Cover progressively:

- 2.5D;
- 3-axis;
- 3+2;
- 4-axis;
- 5-axis;
- turning;
- mill-turn.

Verification should account for:

- stock;
- fixture;
- holder;
- tool;
- machine kinematics where available;
- collision;
- material removal.

Postprocessors are versioned and tested artifacts.

---

# 29. CAE / FEM

Preserve provenance:

```text
Model revision
 ↓
Material
 ↓
Loads
 ↓
Fixtures
 ↓
Mesh
 ↓
Solver
 ↓
Results
```

Study results must identify exactly which model revision generated them.

---

# 30. SURFACES / FREEFORM

Priorities:

- accessible NURBS;
- robust trimming;
- healing;
- continuity;
- surface-solid transitions;
- blends;
- interactive shaping.

Compete on workflow quality, not proprietary imitation.

---

# 31. ECAD ↔ MCAD

Prefer open ecosystems such as KiCad.

Support:

- board outline;
- mounting holes;
- connectors;
- keepouts;
- component envelopes;
- clearances;
- alignment.

Synchronization is explicit and traceable.

---

# 32. OPEN ENGINEERING DATA LAYER

Do not make Git the conceptual PDM model.

Define:

```text
Project
 ├─ Documents
 ├─ Revisions
 ├─ Dependencies
 ├─ Branches
 ├─ Locks
 ├─ Reviews
 └─ Releases
```

Possible providers:

- filesystem;
- SQLite;
- Git;
- object storage;
- self-hosted server.

The platform must work with zero servers.

---

# 33. COMPONENT / SUPPLIER SEMANTICS

A commercial part should have identity:

```text
Part Number
Standard
Supplier
Material
Mass
Cost
Datasheet
Geometry
Metadata
```

Especially:

- fasteners;
- bearings;
- motors;
- connectors;
- profiles.

---

# 34. ENGINEERING API

Expose first-class semantic operations:

```text
query
select
create
modify
constrain
assemble
measure
diagnose
repair
simulate
manufacture
document
validate
rollback
```

The API should be consumable from:

- Python;
- plugins;
- external applications;
- agents.

---

# 35. AI / AGENT LAYER

AI is P3.

Semantic APIs are P0/P1.

AI workflow:

```text
proposal
 ↓
preview
 ↓
permission
 ↓
transaction
 ↓
validation
 ↓
commit / rollback
```

Examples:

> Why does recompute fail?

> Which components depend on this face?

> Create an 8-hole pattern.

> Find fragile references.

> Prepare a manufacturing setup.

AI must never bypass transactions or permissions.

---

# 36. AGENT ORCHESTRATION

Roles:

```text
ORCHESTRATOR
    │
    ├── ARCHITECT
    ├── BUILDER
    ├── RESEARCHER
    ├── UX CRITIC
    ├── ENGINEERING CRITIC
    ├── PERFORMANCE CRITIC
    ├── ADVERSARIAL CRITIC
    ├── UPSTREAM GUARDIAN
    └── JUDGE
```

The roles may use the same foundation model, but their contexts and objectives must remain logically distinct.

---

# 37. AGENT CONTEXT DISCIPLINE

Do not provide the entire project context to every agent.

Prefer:

```text
relevant architecture
+
task
+
artifact
+
evidence
+
required constraints
```

Fresh critics must be especially isolated from builder rationale.

---

# 38. GAUNTLET AS DEVELOPMENT OPERATING SYSTEM

Every major workstream follows:

```text
OBSERVE
 ↓
BASELINE
 ↓
DEFINE BAR
 ↓
BUILD
 ↓
FRESH BLIND CRITIC
 ↓
LARGEST GAP
 ↓
REVISE
 ↓
FRESH CRITIC
 ↓
PASS?
 ├─ NO → ITERATE
 └─ YES
      ↓
    RED TEAM
      ↓
    INTEGRATE
      ↓
    BENCHMARK
      ↓
 HUMAN BLIND TEST
      ↓
 UPSTREAM GAUNTLET
```

---

# 39. GAUNTLET BAR

A bar must be:

- concrete;
- reproducible;
- externally inspectable;
- difficult to game.

Define:

```text
task
starting state
target state
constraints
timing
error tolerance
robustness checks
expected outputs
```

---

# 40. BLIND A/B

For competitor comparison:

```text
Candidate A
Candidate B
```

Hide identity.

Randomize order.

Record:

```text
run_id
seed
mapping
evaluator
hardware
software versions
```

Reveal identity only after judgment.

---

# 41. FRESH CRITIC RULE

Do not automatically reuse the previous critic.

Prefer:

```text
Round 1 → Critic A
Round 2 → Critic B
Round 3 → Critic C
```

Fresh critics assess against the bar.

---

# 42. LARGEST GAP RULE

Every failed round produces one dominant statement:

```text
Largest Gap:
<single sentence>
```

The next cycle primarily addresses that gap.

Avoid scope expansion unless necessary for:

- blocker;
- architecture;
- compatibility;
- security;
- integration.

---

# 43. STALL PROTOCOL

Repeated failures trigger architecture review.

```text
FAIL
 ↓
FAIL
 ↓
FAIL
 ↓
ROOT CAUSE
 ↓
ALTERNATIVES
 ↓
ARCHITECTURE CRITIC
 ↓
DECISION
 ↓
NEW BASELINE
```

---

# 44. RED-TEAM GAUNTLET

Attack passed workflows with:

- dimension changes;
- topology changes;
- feature reorder;
- suppression;
- dependency deletion;
- component replacement;
- configuration changes;
- unit changes;
- save/reload;
- external dependency failures;
- large models;
- pathological constraints;
- assembly modifications.

A red-team failure is a real failure.

---

# 45. MASTER INTEGRATION GAUNTLET

Test:

```text
Sketch
 ↓
Part
 ↓
Assembly
 ↓
Drawing
 ↓
BOM
 ↓
CAM
 ↓
Revision
 ↓
Export
 ↓
Reload
```

Module-level PASS does not imply product-level PASS.

---

# 46. HUMAN BLIND GAUNTLET

For major milestones:

1. prepare candidate A;
2. prepare candidate B;
3. hide identity;
4. give the same task to real engineers;
5. record outcomes;
6. reveal identity only after judgment.

Measure:

- ease;
- speed;
- trust;
- recovery;
- preference.

Agents are not the user.

---

# 47. GAUNTLET STATE

Persist:

```text
gauntlet/
  state/
    master.json
    sketcher.json
    assembly.json
    techdraw.json
    cam.json
    integration.json

  rounds/
    <workstream>/
      round-XXX/
        bar.md
        input.md
        output.md
        benchmark.json
        critic-*.md
        verdict.md
        largest-gap.md
        changes.md
        evidence/
```

The repository is the source of truth, not chat context.

---

# 48. EVIDENCE LEDGER

Maintain:

`docs/evidence-ledger.md`

Fields:

```text
ID
Claim
Scope
Baseline
Method
Evidence
Independent verification
Status
Date
```

Statuses:

```text
UNKNOWN
IN_PROGRESS
FAIL
PASS
SUPERIOR
REGRESSED
```

Never jump from UNKNOWN to SUPERIOR.

---

# 49. FAILURE TAXONOMY

```text
F1 Geometry
F2 Reference
F3 Recompute
F4 UX
F5 Performance
F6 Integration
F7 Compatibility
F8 Data
F9 Manufacturing
F10 Governance / upstreamability
```

Each significant failure gets one or more categories.

---

# 50. ARCHITECTURE GAUNTLET

Major architecture changes require:

```text
Problem
Current architecture
Evidence
Proposal
Alternatives
Performance impact
Compatibility impact
Migration cost
Rollback
Upstream strategy
```

Prefer two independent architecture critiques before irreversible core changes.

---

# 51. COMMUNITY FEEDBACK LOOP

Community feedback is signal, not specification.

Process:

```text
signal
 ↓
reproduce
 ↓
classify
 ↓
impact/frequency
 ↓
requirement
 ↓
benchmark
 ↓
Gauntlet
 ↓
community validation
```

Do not implement every frequently requested feature.

---

# 52. COMPATIBILITY CONTRACT

Protect where practical:

- existing FreeCAD files;
- STEP;
- IGES;
- STL;
- DXF;
- IFC;
- Python API;
- macros;
- addons;
- workbench ecosystem.

Intentional breaking changes require:

- justification;
- migration;
- documentation;
- tests;
- explicit status.

---

# 52.5 COMPATIBILITY ROUND-TRIP MATRIX

Compatibility is not defined as merely opening files. For each supported artifact type evaluate:

```text
OPEN
READ
EDIT
RECOMPUTE
SAVE
RELOAD
ROUND-TRIP
CROSS-VERSION
DOWNGRADE / BACKWARD VIEW
```

For native FreeCAD documents, explicitly test what happens when DiofanCAD semantic metadata is present and the file is reopened by FreeCAD upstream.

Each incompatibility must be classified:

```text
SAFE LOSS
VISIBLE LOSS
UNSAFE CHANGE
BLOCKED
```

No compatibility claim may be marked PASS without a concrete round-trip test.

# 53. SECURITY CONTRACT

Special care for:

- Python;
- plugins;
- external files;
- AI;
- self-hosting;
- CAM.

Rules:

- no silent external command execution;
- explicit permission;
- transactional agent changes;
- audit trail;
- dependency review.

---

# 54. LICENSE GOVERNANCE

Maintain:

`docs/dependencies-license-matrix.md`

For each dependency:

```text
name
version
license
linking
distribution
modification rights
security
maintenance
replacement path
risk
```

Verify actual licensing and distribution implications.

---

# 55. OBSERVABILITY

Provide structured traces for:

```text
recompute
dependency propagation
references
assembly solving
performance
AI transactions
```

The system should answer:

> What is it doing, why, and where is time going?

---

# 56. GOLDEN WORKFLOW SUITE

## Mechanical

1. Bracket
2. Parametric enclosure
3. Gearbox
4. Mounting assembly
5. Motion assembly

## Product

6. Complex housing
7. Surface-heavy consumer object

## Manufacturing

8. 3-axis milling part
9. Multiaxis candidate

## CAE

10. Structural simulation

## Electronics

11. PCB enclosure with connectors and keepouts

## Documentation

12. Released drawing + BOM + revision

## Integration

13. End-to-end product release

---

# 57. BENCHMARK PROTOCOL

For each workflow:

```text
1. Prepare identical starting conditions
2. Define target state
3. Define assumptions
4. Execute
5. Record timing
6. Record interaction count
7. Record failures
8. Verify final state
9. Save/reload
10. Apply change challenge
11. Run Model Doctor
12. Run performance check
13. Produce evidence bundle
```

---

# 58. FUSION SUPREMACY SCORE

Evaluate:

```text
Correctness
Robustness
Efficiency
Discoverability
Interaction Quality
Recovery
Performance
Automation
Data Ownership
Integration
```

Critical failures override arithmetic.

---

# 59. PRODUCT STATUS

A benchmark may be:

### SUPERIOR

DiofanCAD is materially better without critical failure.

### TIED

Practically indistinguishable.

### INFERIOR

Fusion is materially better.

### UNKNOWN

Not enough evidence.

---

# 60. MASTER COMPETITIVE REPORT

Required output:

```text
DiofanCAD wins:
...

Fusion wins:
...

Ties:
...

Unknown:
...

Critical failures:
...

Largest remaining gaps:
...
```

Never conceal losses.

---

# 61. FORK-ROT / DIVERGENCE REPORT

At defined milestones report:

```text
upstream age
commits
files changed
subsystems changed
API divergence
data-model divergence
merge cost
number of upstream candidates
```

This is a first-class program metric.

---

# 62. DIOFANCAD RELEASE CHANNELS

Use conceptual channels:

### Upstream-compatible

Small improvements with direct contribution paths.

### DiofanCAD Experimental

Major architectural/product experimentation.

### DiofanCAD Preview

Gauntlet-passed vertical slices intended for broader testing.

### DiofanCAD Stable

Only after master integration and compatibility gates.

---

# 63. FEATURE MATURITY

A feature progresses:

```text
IDEA
 ↓
PROTOTYPE
 ↓
EXPERIMENTAL
 ↓
GAUNTLET PASS
 ↓
PREVIEW
 ↓
STABLE
 ↓
UPSTREAM CANDIDATE
 ↓
UPSTREAM
```

Not every feature must reach UPSTREAM.

---

# 64. DEFINITION OF DONE

For each applicable feature:

```text
[ ] implementation
[ ] semantic model
[ ] persistence
[ ] undo/redo
[ ] recompute
[ ] API
[ ] tests
[ ] docs
[ ] save/reload
[ ] compatibility review
[ ] UX integration
[ ] performance benchmark
[ ] Gauntlet PASS
[ ] Red Team PASS
[ ] Integration PASS
[ ] upstream assessment
```

---

# 65. NON-GOALS

Do not:

- blindly clone Fusion;
- rewrite the entire kernel without evidence;
- create mandatory cloud infrastructure;
- add AI before semantic APIs;
- merge every experiment upstream;
- create a massive permanent diff;
- optimize screenshots over robustness;
- hide Fusion wins;
- claim victory from a demo;
- casually break FreeCAD compatibility.

---

# 66. ROADMAP

V7 roadmap rule: phases are not a purely linear feature queue. Three tracks run in parallel:

```text
TRACK A — PRODUCT
competitive workflow value

TRACK B — ARCHITECTURE
semantic / reference / recompute foundation

TRACK C — UPSTREAM
minimize divergence / extract useful changes
```

A milestone advances only when all applicable tracks meet their exit bars.


## Phase 0 — Reality Map + Architecture Proof

Produce the V6 reality-map artifacts **and** the minimum executable architecture proofs defined in Sections 75 and 76.

Produce:

- repository map;
- architecture map;
- feature matrix;
- performance baseline;
- test baseline;
- compatibility baseline;
- dependency/license matrix;
- community gap map;
- competitor landscape;
- Fusion benchmark plan;
- upstream strategy;
- architecture risks;
- upstream backlog;
- evidence ledger;
- Gauntlet harness.

## Phase 1 — Engineering Core

Build or improve:

- semantic foundations;
- reference service;
- dependency/recompute;
- Model Doctor;
- Context Engine;
- instrumentation;
- plugin/API foundations.

First vertical slice:

```text
Sketch → Part → Modify → Save → Reload
```

## Phase 2 — Mechanical Core

Improve:

- Sketcher;
- Part Design;
- design intent;
- change resilience.

## Phase 3 — Assembly/Product Definition

Build:

- source/occurrence model;
- joints;
- configurations;
- BOM;
- in-context editing;
- large assembly robustness.

## Phase 4 — Documentation/Release

Build:

- TechDraw;
- BOM;
- revisions;
- release states.

## Phase 5 — Manufacturing

Build:

- setup;
- machine;
- tools;
- verification;
- post discipline.

## Phase 6 — CAE / Surfaces / ECAD

Proceed by vertical slice.

## Phase 7 — Open Engineering Data

Build:

- project model;
- revisions;
- dependencies;
- self-hosting option.

## Phase 8 — Intelligence

Build:

- engineering agent API;
- controlled automation;
- AI copilot.

## Phase 9 — Fusion Killer Evaluation

Run:

- competitive suite;
- blind A/B;
- red team;
- master integration;
- human blind;
- upstream extraction audit.

---

# 67. FIRST THREE VERTICAL SLICES

## Slice A — Sketch → Part → Modify → Validate

Also runs the primary change-resilience proof defined in Section 75.

Proves:

- Sketcher;
- semantic references;
- recompute;
- Model Doctor;
- Context Engine.

## Slice B — Part → Assembly → Drawing → BOM

Proves downstream semantic survival under change, not merely module connectivity.

Proves:

- semantic components;
- source/occurrence;
- joints;
- configuration;
- TechDraw;
- BOM.

## Slice C — Part → CAM → Verify

Proves:

- manufacturing semantics;
- setup;
- tools;
- verification.

---

# 68. AGENT ENGINEERING OPERATING CONTRACT

Before substantial implementation, every agent or human implementation cycle must:

```text
Inspect
Measure
Explain
Propose
Critique
Implement
Test
Benchmark
Record
```

Do not:

- assume APIs;
- assume features are absent;
- invent architecture from memory;
- rewrite before profiling;
- silently break compatibility;
- self-certify PASS.

---

# 69. FIRST AGENT MISSION

The first mission is NOT to implement a major feature or claim architectural certainty.

The first mission is:

# BUILD THE ENGINEERING REALITY MAP

Required:

```text
docs/baseline-architecture.md
docs/baseline-performance.md
docs/baseline-feature-matrix.md
docs/baseline-compatibility.md
docs/baseline-test-health.md
docs/dependencies-license-matrix.md
docs/community-gap-map.md
docs/competitor-landscape.md
docs/fusion-benchmark-plan.md
docs/upstream-strategy.md
docs/architecture-risks.md
docs/upstream-backlog.md
docs/evidence-ledger.md
```

And:

```text
gauntlet/
  README.md
  state/master.json
```

Identify:

- top 20 product gaps;
- top 10 architecture risks;
- top 10 performance risks;
- top 10 compatibility risks;
- top 10 upstreamability risks;
- first 3 vertical slices;
- first benchmark suite;
- first Gauntlet bars.

Only then finalize implementation priorities.

---

# 70. LONG-RUN CONTEXT / PERSISTENCE

The program must survive:

- context compaction;
- agent replacement;
- interrupted runs;
- branch rebases;
- upstream sync;
- failed experiments.

Source of truth:

```text
repository
+
docs
+
Gauntlet state
+
benchmarks
+
evidence
```

Never chat history alone.

---

# 71. MASTER SUCCESS CONDITION

DiofanCAD may internally earn the label **FUSION KILLER** only after:

```text
P0 Engineering Core                 PASS
Mechanical workflows                PASS
Assembly                            PASS
Documentation                       PASS
Manufacturing                       PASS
Integration                         PASS
Red Team                            PASS
Human Blind                         PASS
Compatibility                       PASS
License / Dependency                PASS
Upstream Strategy                   PASS
```

And the benchmark demonstrates repeatable competitive superiority.

---

# 72. EXAMPLE FUTURE RESULT

Example only:

```text
Mechanical CAD       12 wins / 3 ties / 1 loss
Assembly              8 wins / 2 ties / 2 losses
Documentation         6 wins / 1 tie  / 0 losses
CAM                   4 wins / 3 ties / 2 losses
CAE                   3 wins / 2 ties / 4 losses
ECAD                  2 wins / 2 ties / 3 losses

Critical failures: 0
Offline: PASS
Compatibility: PASS
Upstream extraction: PASS
Human blind preference: measured
```

The numbers must always come from real measurement.

---

# 73. PROGRAM ECONOMICS / CAPACITY CONTROL

DiofanCAD is a long-lived engineering program. Technical ambition must be matched by explicit capacity accounting.

Track at each milestone:

```text
core engineers
domain specialists
UX / research capacity
QA / release engineering
benchmark infrastructure
community / upstream capacity
maintenance load
expected implementation throughput
```

For every major workstream estimate:

```text
prototype cost
production implementation cost
test cost
maintenance cost
upstream extraction cost
divergence cost
```

No roadmap item becomes committed solely because it is strategically attractive. It must fit the available capacity or explicitly displace another item.

# 74. WEDGE STRATEGY / FIRST PROOF OF VALUE

The first competitive wedge is **change resilience through engineering semantics**.

Primary proof workflow:

```text
Create part
 ↓
Create downstream artifact
 ↓
Change parameter / topology / feature order
 ↓
Measure surviving references
 ↓
Measure downstream validity
 ↓
Recover failures
 ↓
Save / reload
```

The first major product claim is:

> DiofanCAD preserves engineering intent through realistic change better than the benchmark baseline.

No broad expansion into CAM, FEM, ECAD or advanced AI is justified by architecture alone. The wedge must first produce measurable value.

# 75. ARCHITECTURE PROOF GATES

Before scaling a major architecture, prove the smallest version that could falsify the idea.

Minimum proof set for the semantic core:

```text
P0-A persistent-reference prototype
P0-B semantic-object prototype
P0-C recompute instrumentation prototype
P0-D save/reload prototype
P0-E realistic change-challenge prototype
```

Each proof must state:

```text
hypothesis
minimum implementation
expected result
falsification condition
measured result
decision
```

A failed proof does not trigger automatic scope expansion. It triggers architecture review.

# 76. REUSE / BUILD / INTEGRATE / DEFER MATRIX

Every major subsystem must be classified before implementation:

```text
REUSE     existing FreeCAD / kernel capability with minimal adaptation
BUILD     new capability where differentiated value is required
INTEGRATE external/open subsystem behind a defined boundary
DEFER     strategically useful but not necessary for current wedge
NEVER     outside product scope or unjustified duplication
```

The decision and its rationale belong in the architecture record.

# 77. KILL CRITERIA

A feature, subsystem or architecture experiment must be abandoned, replaced or frozen when one or more of the following are demonstrated:

```text
1. Benchmark gain remains below the predefined bar after the agreed iteration budget.
2. Architecture cost exceeds the approved complexity boundary.
3. Maintenance burden materially exceeds its measurable value.
4. Upstream divergence exceeds its budget without compensating benefit.
5. Compatibility damage cannot be contained or migrated safely.
6. A simpler design achieves equivalent results.
7. The experiment cannot produce reproducible evidence.
```

Kill decisions are recorded as first-class engineering outcomes, not treated as failures of people.

# 78. MILESTONE EXIT CONTRACT

A phase cannot be considered complete because its planned tasks were implemented. It exits only when its evidence gates pass.

Every milestone must provide:

```text
PRODUCT BAR
ARCHITECTURE BAR
PERFORMANCE BAR
COMPATIBILITY BAR
RED-TEAM BAR
DIVERGENCE BAR
DOCUMENTATION BAR
```

Example:

```text
Feature complete       ≠ milestone PASS
Unit tests green        ≠ product PASS
Benchmark improvement   ≠ superiority
Upstream-ready          ≠ upstream-accepted
```

# 79. PERMANENT DIVERGENCE POLICY

Most DiofanCAD changes should remain extractable. Permanent divergence is allowed only when all of the following hold:

```text
clear product benefit
no acceptable upstream-compatible design
measured competitive value
bounded maintenance burden
explicit compatibility strategy
explicit long-term ownership
architecture review approval
```

Permanent divergence is a managed exception, not a default outcome.

# 80. HUMAN VALIDATION EARLIER IN THE LOOP

Human engineers are not a final certification step. They are a discovery instrument throughout product development.

For major UX changes:

```text
observe baseline
 ↓
prototype
 ↓
human test
 ↓
critic
 ↓
revise
 ↓
benchmark
```

Blind A/B remains required for major competitive claims, but earlier formative testing is encouraged before architecture hardens around an interaction model.

# 81. DETERMINISTIC RECOMPUTE / REPRODUCIBILITY CONTRACT

The platform should strive for:

```text
same input
 ↓
same dependency interpretation
 ↓
same recompute semantics
 ↓
same result
```

Where exact determinism is impossible, the system must define bounded equivalence and identify non-deterministic components.

This requirement supports reproducible benchmarks, agent transactions, save/reload testing, provenance and debugging.

# 82. FAILURE-TO-LEARNING LOOP

Every significant failure should produce reusable knowledge:

```text
FAILURE
 ↓
CLASSIFY
 ↓
REPRODUCE
 ↓
ROOT CAUSE
 ↓
ARCHITECTURE / UX / IMPLEMENTATION DECISION
 ↓
TEST
 ↓
EVIDENCE ENTRY
```

Fixes without regression tests are incomplete. Repeated failures without architecture review are a process defect.

# 83. V7 MASTER EXECUTION RULE

The program is not allowed to scale because the specification is comprehensive. It scales only when the evidence base is strong enough.

```text
PROVE
 ↓
MEASURE
 ↓
DECIDE
 ↓
SCALE
```

The inverse is forbidden:

```text
DESIGN
 ↓
ASSUME
 ↓
IMPLEMENT
 ↓
DEFEND
```

# 84. CONSTITUTION OF DIOFANCAD

### Rule 1
Evidence beats opinion.

### Rule 2
Correctness beats demos.

### Rule 3
Robustness beats feature count.

### Rule 4
Integration beats isolated excellence.

### Rule 5
Open source beats proprietary convenience where technically viable.

### Rule 6
Upstreamability beats permanent divergence.

### Rule 7
Fresh critics beat self-certification.

### Rule 8
The largest gap determines the next iteration.

### Rule 9
Fusion wins whenever evidence says Fusion wins.

### Rule 10
Competitors are sources of information, not templates.

### Rule 11
The lab may be radical; upstream contributions must be reviewable.

### Rule 12
DiofanCAD exists to expand what FreeCAD can become.

---

# 85. FINAL VISION

The ideal end state is not:

> a fork that Autodesk fears.

It is:

> **DiofanCAD: an open engineering CAD platform that proves how far FreeCAD can go, wins important real-world workflows against commercial CAD, and continuously returns useful improvements to the open-source ecosystem.**

The ultimate experience:

```text
Model
 ↓
Change
 ↓
Assemble
 ↓
Document
 ↓
Manufacture
 ↓
Simulate
 ↓
Review
 ↓
Release
 ↓
Automate
```

with:

```text
Open source
Offline use
User-owned data
Interoperability
Extensibility
Upstream contribution
```

The benchmark, not the slogan, decides whether DiofanCAD has earned the name.
