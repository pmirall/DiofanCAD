// SPDX-License-Identifier: LGPL-2.1-or-later
//
// DiofanCAD P0-C — recompute instrumentation.
//
// Records what a document recompute did: the plan (topological order), which
// objects actually executed, how long each took, and why each one was
// recomputed (pre-touched, or propagated from a specific object/property).
//
// Design constraints, from docs/architecture-boundary.md:
//   * observation only — this must never change recompute semantics;
//   * disabled by default, and cheap enough when disabled to be unmeasurable;
//   * the trace is a DERIVED artifact, never a source of truth about the model.

#pragma once

#include <chrono>
#include <string>
#include <vector>

#include <FCGlobal.h>

namespace App
{

class Document;
class DocumentObject;

/// Why an object was recomputed during a run.
enum class RecomputeCause
{
    NotRecomputed,  //< visited but `mustRecompute()` was false
    PreTouched,     //< already touched when the run started
    Propagated,     //< an upstream object enforced recompute during this run
    Unknown         //< recomputed without an observed cause (a gap worth reporting)
};

/// One visited object within a run.
struct AppExport RecomputeTraceObject
{
    std::string name;      //< full object name
    int order {0};         //< position in the topological plan
    bool recomputed {false};
    double seconds {0.0};  //< execution time; 0 when not recomputed
    int result {0};        //< 0 ok, 1 error, -1 abort
    RecomputeCause cause {RecomputeCause::NotRecomputed};
    std::string causeSource;    //< object that propagated to this one
    std::string causeProperty;  //< property carrying the propagation, when known
};

/// One propagation edge observed during a run.
struct AppExport RecomputeTraceEdge
{
    std::string from;
    std::string to;
    std::string property;  //< empty for coarse-grained propagation
};

/// One complete document recompute.
struct AppExport RecomputeTraceRun
{
    unsigned long id {0};
    std::string document;
    bool force {false};
    int options {0};
    std::vector<std::string> plan;  //< topological order, as executed
    std::vector<RecomputeTraceObject> objects;
    std::vector<RecomputeTraceEdge> edges;
    int recomputedCount {0};  //< as reported by Document::recompute
    double totalSeconds {0.0};
    double tracedSeconds {0.0};  //< sum of per-object times; the attributed part
    bool complete {false};
};

/**
 * Process-wide recompute trace.
 *
 * Every hook is a no-op unless the trace is enabled, which is checked inline at
 * the call site so a disabled trace costs one predictable branch.
 */
class AppExport RecomputeTrace
{
public:
    /// Inline so a disabled trace does not even cost a call.
    static bool isEnabled()
    {
        return enabled;
    }
    static void setEnabled(bool on);

    /// Write each completed run as JSON to this path ("" disables file output).
    /// Enabling an output path implies enabling the trace.
    static void setOutputPath(std::string path);
    static const std::string& outputPath();

    // --- run lifecycle; all no-ops when disabled -------------------------
    static void beginRun(const Document& doc, bool force, int options);
    static void recordPlan(const std::vector<DocumentObject*>& topoSorted);
    static void recordObject(const DocumentObject& obj,
                             int order,
                             bool recomputed,
                             double seconds,
                             int result);
    static void recordPropagation(const DocumentObject& from,
                                  const DocumentObject& to,
                                  const char* property);
    static void endRun(int recomputedCount, double totalSeconds);

    // --- access ----------------------------------------------------------
    static const RecomputeTraceRun& lastRun();
    static bool hasRun();
    static std::string lastRunAsJson();
    static void clear();

    /// Steady clock used for per-object attribution.
    using Clock = std::chrono::steady_clock;
    static double secondsSince(Clock::time_point start)
    {
        return std::chrono::duration<double>(Clock::now() - start).count();
    }

private:
    static bool enabled;
};

}  // namespace App
