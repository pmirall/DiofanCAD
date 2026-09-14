# Round 001 — Input: what the baseline actually does

Inspection of `src/App/Document.cpp` at the fork baseline, before writing any code. V7 §68: *do not assume features are absent.*

## Observability that already exists

| Mechanism | Location | What it gives | What it does not give |
|---|---|---|---|
| `Base::TimeTracker` | `Document::recompute` | 3 coarse checkpoints: topo sort, recompute, total | nothing per object; free text to the console log; float seconds |
| `ZoneScoped` (Tracy) | `Document::recompute` | frame-profiler zone | needs `BUILD_TRACY_FRAME_PROFILER` and an external profiler; not queryable in-process |
| `FC_LOG("Recomputing X")` | `_recomputeFeature` | which objects ran, as text at LOG level | no timing; unstructured; not machine-readable |
| `_RecomputeLog` | `DocumentP` | per-object **error** messages | errors only — no timing, no causality, nothing about successful recomputes |
| `signalBeforeRecompute`, `signalRecomputedObject`, `signalRecomputed`, `signalBecameStable` | `Document` | observer hooks around the run | **no "about to recompute object X" signal**, so an observer cannot bound an object's execution |
| `exportGraphviz` | `Document` | static dependency graph | the graph, not the run |
| fine-grained recompute | `getInListProp`, `DepEdge`, `touchedProps` | property-level propagation exists in the engine | it is applied, never recorded |

## The gap this proof addresses

The baseline can already tell you *that* a recompute happened, roughly how long the whole thing took, and which objects failed. It cannot tell you **how long each object took** or **why each object recomputed**.

## REUSE / BUILD / INTEGRATE / DEFER (V7 §76)

| Concern | Decision | Rationale |
|---|---|---|
| Dependency graph, topological sort, propagation | **REUSE** | Already correct and battle-tested. The proof observes it; it does not replace it. |
| Error reporting | **REUSE** | `_RecomputeLog` already owns errors. The trace records the result code and does not duplicate the message text. |
| Coarse run timing | **REUSE** | `TimeTracker` keeps its checkpoints, untouched. |
| Per-object timing clock | **BUILD** (thin) | `Base::TimeElapsed::diffTimeF` returns **float seconds**, which loses precision at the microsecond scale this needs. `std::chrono::steady_clock` is used directly. |
| Structured per-object + causality record | **BUILD** | Nothing in the baseline records it, and no existing signal can reconstruct it — there is no pre-object signal. |
| Python binding for the trace | **DEFER** | Not needed to falsify the hypothesis. JSON output covers the benchmark harness. |
| Tracy / flamegraph integration | **DEFER** | Already exists for those who want a profiler; orthogonal to in-process attribution. |
| Replacing the recompute engine | **NEVER** (this round) | No evidence justifies it, and §19.5 says extend first. |

## Why an observer-only implementation was rejected

The lowest-divergence option would add zero lines to upstream code and subscribe to existing signals. It was rejected on a specific, checkable fact: `signalRecomputedObject` fires *after* an object recomputes and only when it was touched or recomputed, and there is no corresponding pre-object signal. An observer can therefore measure the interval between successive emissions, which includes every skipped object in between, but cannot bound any single object's execution. Per-object attribution requires a hook inside the loop.
