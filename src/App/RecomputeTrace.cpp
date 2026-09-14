// SPDX-License-Identifier: LGPL-2.1-or-later
//
// DiofanCAD P0-C — recompute instrumentation. See RecomputeTrace.h.

#include "PreCompiled.h"

#ifndef _PreComp_
#include <fstream>
#include <mutex>
#include <sstream>
#include <unordered_map>
#endif

#include <Base/Console.h>

#include "Document.h"
#include "DocumentObject.h"
#include "RecomputeTrace.h"

using namespace App;

namespace
{

std::mutex traceMutex;
RecomputeTraceRun currentRun;
RecomputeTraceRun completedRun;
bool runInProgress {false};
bool haveCompletedRun {false};
unsigned long runCounter {0};
std::string tracePath;
// name -> index of the first propagation edge that targets it
std::unordered_map<std::string, std::size_t> firstIncomingEdge;
// name -> index into currentRun.objects
std::unordered_map<std::string, std::size_t> objectIndex;

std::string jsonEscape(const std::string& in)
{
    std::string out;
    out.reserve(in.size() + 8);
    for (const char character : in) {
        switch (character) {
            case '"':
                out += "\\\"";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                if (static_cast<unsigned char>(character) < 0x20) {
                    char buf[7];
                    std::snprintf(buf, sizeof(buf), "\\u%04x", character);
                    out += buf;
                }
                else {
                    out += character;
                }
        }
    }
    return out;
}

const char* causeName(RecomputeCause cause)
{
    switch (cause) {
        case RecomputeCause::NotRecomputed:
            return "not-recomputed";
        case RecomputeCause::PreTouched:
            return "pre-touched";
        case RecomputeCause::Propagated:
            return "propagated";
        case RecomputeCause::Unknown:
        default:
            return "unknown";
    }
}

std::string runToJson(const RecomputeTraceRun& run)
{
    std::ostringstream out;
    out.setf(std::ios::fixed);
    out << "{\n";
    out << "  \"run\": " << run.id << ",\n";
    out << "  \"document\": \"" << jsonEscape(run.document) << "\",\n";
    out << "  \"force\": " << (run.force ? "true" : "false") << ",\n";
    out << "  \"options\": " << run.options << ",\n";
    out << "  \"complete\": " << (run.complete ? "true" : "false") << ",\n";
    out << "  \"planned\": " << run.plan.size() << ",\n";
    out << "  \"recomputed\": " << run.recomputedCount << ",\n";
    out.precision(9);
    out << "  \"total_seconds\": " << run.totalSeconds << ",\n";
    out << "  \"attributed_seconds\": " << run.tracedSeconds << ",\n";

    out << "  \"plan\": [";
    for (std::size_t i = 0; i < run.plan.size(); ++i) {
        out << (i != 0 ? ", " : "") << '"' << jsonEscape(run.plan[i]) << '"';
    }
    out << "],\n";

    out << "  \"objects\": [\n";
    for (std::size_t i = 0; i < run.objects.size(); ++i) {
        const auto& obj = run.objects[i];
        out << "    {\"name\": \"" << jsonEscape(obj.name) << "\", \"order\": " << obj.order
            << ", \"recomputed\": " << (obj.recomputed ? "true" : "false")
            << ", \"seconds\": " << obj.seconds << ", \"result\": " << obj.result
            << ", \"cause\": \"" << causeName(obj.cause) << "\"";
        if (!obj.causeSource.empty()) {
            out << ", \"cause_source\": \"" << jsonEscape(obj.causeSource) << "\"";
        }
        if (!obj.causeProperty.empty()) {
            out << ", \"cause_property\": \"" << jsonEscape(obj.causeProperty) << "\"";
        }
        out << "}" << (i + 1 < run.objects.size() ? "," : "") << "\n";
    }
    out << "  ],\n";

    out << "  \"edges\": [\n";
    for (std::size_t i = 0; i < run.edges.size(); ++i) {
        const auto& edge = run.edges[i];
        out << "    {\"from\": \"" << jsonEscape(edge.from) << "\", \"to\": \""
            << jsonEscape(edge.to) << "\", \"property\": \"" << jsonEscape(edge.property)
            << "\"}" << (i + 1 < run.edges.size() ? "," : "") << "\n";
    }
    out << "  ]\n";
    out << "}\n";
    return out.str();
}

}  // namespace

bool RecomputeTrace::enabled = false;

void RecomputeTrace::setEnabled(bool on)
{
    const std::lock_guard<std::mutex> guard(traceMutex);
    enabled = on;
}

void RecomputeTrace::setOutputPath(std::string path)
{
    const std::lock_guard<std::mutex> guard(traceMutex);
    tracePath = std::move(path);
    if (!tracePath.empty()) {
        enabled = true;
    }
}

const std::string& RecomputeTrace::outputPath()
{
    return tracePath;
}

void RecomputeTrace::beginRun(const Document& doc, bool force, int options)
{
    const std::lock_guard<std::mutex> guard(traceMutex);
    if (!enabled) {
        return;
    }
    currentRun = RecomputeTraceRun {};
    currentRun.id = ++runCounter;
    currentRun.document = doc.getName() != nullptr ? doc.getName() : "";
    currentRun.force = force;
    currentRun.options = options;
    firstIncomingEdge.clear();
    objectIndex.clear();
    runInProgress = true;
}

void RecomputeTrace::recordPlan(const std::vector<DocumentObject*>& topoSorted)
{
    const std::lock_guard<std::mutex> guard(traceMutex);
    if (!enabled || !runInProgress) {
        return;
    }
    currentRun.plan.reserve(topoSorted.size());
    currentRun.objects.reserve(topoSorted.size());
    int order = 0;
    for (const auto* obj : topoSorted) {
        if (obj == nullptr) {
            continue;
        }
        const std::string name = obj->getFullName();
        currentRun.plan.push_back(name);

        RecomputeTraceObject entry;
        entry.name = name;
        entry.order = order++;
        // Snapshot the causal starting condition: what was already dirty before
        // this run touched anything.
        entry.cause = obj->isTouched() ? RecomputeCause::PreTouched
                                       : RecomputeCause::NotRecomputed;
        objectIndex[name] = currentRun.objects.size();
        currentRun.objects.push_back(std::move(entry));
    }
}

void RecomputeTrace::recordObject(const DocumentObject& obj,
                                  int order,
                                  bool recomputed,
                                  double seconds,
                                  int result)
{
    const std::lock_guard<std::mutex> guard(traceMutex);
    if (!enabled || !runInProgress) {
        return;
    }
    const std::string name = obj.getFullName();
    auto found = objectIndex.find(name);
    if (found == objectIndex.end()) {
        // Not in the plan: record it anyway rather than losing the observation.
        RecomputeTraceObject entry;
        entry.name = name;
        entry.order = order;
        objectIndex[name] = currentRun.objects.size();
        currentRun.objects.push_back(std::move(entry));
        found = objectIndex.find(name);
    }

    auto& entry = currentRun.objects[found->second];
    entry.recomputed = recomputed;
    entry.seconds = seconds;
    entry.result = result;

    if (recomputed) {
        currentRun.tracedSeconds += seconds;
        // Causality: an incoming propagation edge observed earlier in this run
        // explains the recompute; otherwise it was already dirty when we
        // started. Anything else is an honest "unknown" rather than a guess.
        auto edge = firstIncomingEdge.find(name);
        if (edge != firstIncomingEdge.end()) {
            const auto& observed = currentRun.edges[edge->second];
            entry.cause = RecomputeCause::Propagated;
            entry.causeSource = observed.from;
            entry.causeProperty = observed.property;
        }
        else if (entry.cause != RecomputeCause::PreTouched) {
            entry.cause = RecomputeCause::Unknown;
        }
    }
}

void RecomputeTrace::recordPropagation(const DocumentObject& from,
                                       const DocumentObject& to,
                                       const char* property)
{
    const std::lock_guard<std::mutex> guard(traceMutex);
    if (!enabled || !runInProgress) {
        return;
    }
    RecomputeTraceEdge edge;
    edge.from = from.getFullName();
    edge.to = to.getFullName();
    edge.property = property != nullptr ? property : "";
    firstIncomingEdge.emplace(edge.to, currentRun.edges.size());
    currentRun.edges.push_back(std::move(edge));
}

void RecomputeTrace::endRun(int recomputedCount, double totalSeconds)
{
    std::string path;
    std::string payload;
    {
        const std::lock_guard<std::mutex> guard(traceMutex);
        if (!enabled || !runInProgress) {
            return;
        }
        currentRun.recomputedCount = recomputedCount;
        currentRun.totalSeconds = totalSeconds;
        currentRun.complete = true;
        completedRun = currentRun;
        haveCompletedRun = true;
        runInProgress = false;
        if (!tracePath.empty()) {
            path = tracePath;
            payload = runToJson(completedRun);
        }
    }

    if (!path.empty()) {
        std::ofstream out(path, std::ios::out | std::ios::trunc);
        if (out) {
            out << payload;
        }
        else {
            Base::Console().warning("RecomputeTrace: cannot write trace to %s\n", path.c_str());
        }
    }
}

const RecomputeTraceRun& RecomputeTrace::lastRun()
{
    const std::lock_guard<std::mutex> guard(traceMutex);
    return completedRun;
}

bool RecomputeTrace::hasRun()
{
    const std::lock_guard<std::mutex> guard(traceMutex);
    return haveCompletedRun;
}

std::string RecomputeTrace::lastRunAsJson()
{
    const std::lock_guard<std::mutex> guard(traceMutex);
    return runToJson(completedRun);
}

void RecomputeTrace::clear()
{
    const std::lock_guard<std::mutex> guard(traceMutex);
    currentRun = RecomputeTraceRun {};
    completedRun = RecomputeTraceRun {};
    runInProgress = false;
    haveCompletedRun = false;
    firstIncomingEdge.clear();
    objectIndex.clear();
}
