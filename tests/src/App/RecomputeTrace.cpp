// SPDX-License-Identifier: LGPL-2.1-or-later
//
// DiofanCAD P0-C — tests for the recompute trace.
//
// The proof's falsification conditions are that the instrumentation cannot
// attribute time or causality, or that it perturbs what it measures. These
// tests exist to make both failures visible.

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "App/Application.h"
#include "App/Document.h"
#include "App/DocumentObject.h"
#include "App/FeatureTest.h"
#include "App/RecomputeTrace.h"
#include <src/App/InitApplication.h>

class RecomputeTraceTest: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
    }

    void SetUp() override
    {
        App::RecomputeTrace::setEnabled(false);
        App::RecomputeTrace::setOutputPath("");
        App::RecomputeTrace::clear();
        _docName = App::GetApplication().getUniqueDocumentName("tracetest");
        _doc = App::GetApplication().newDocument(_docName.c_str(), "testUser");
    }

    void TearDown() override
    {
        App::RecomputeTrace::setEnabled(false);
        App::RecomputeTrace::setOutputPath("");
        App::RecomputeTrace::clear();
        App::GetApplication().closeDocument(_docName.c_str());
    }

    App::Document* doc()
    {
        return _doc;
    }

    /// A -> B -> C dependency chain, settled (nothing touched).
    void buildChain(App::Document* document)
    {
        auto* objA = dynamic_cast<App::FeatureTest*>(
            document->addObject("App::FeatureTest", "A"));
        auto* objB = dynamic_cast<App::FeatureTest*>(
            document->addObject("App::FeatureTest", "B"));
        auto* objC = dynamic_cast<App::FeatureTest*>(
            document->addObject("App::FeatureTest", "C"));
        objB->Link.setValue(objA);
        objC->Link.setValue(objB);
        document->recompute();
        _a = objA;
        _b = objB;
        _c = objC;
    }

    App::FeatureTest* a()
    {
        return _a;
    }

private:
    std::string _docName;
    App::Document* _doc {};
    App::FeatureTest* _a {};
    App::FeatureTest* _b {};
    App::FeatureTest* _c {};
};

// The trace must cost nothing and record nothing unless asked.
TEST_F(RecomputeTraceTest, DisabledByDefaultRecordsNothing)  // NOLINT
{
    EXPECT_FALSE(App::RecomputeTrace::isEnabled());
    buildChain(doc());
    a()->touch();
    doc()->recompute();
    EXPECT_FALSE(App::RecomputeTrace::hasRun());
}

// Falsification condition 1a: can it attribute time?
TEST_F(RecomputeTraceTest, AttributesTimePerObject)  // NOLINT
{
    buildChain(doc());
    App::RecomputeTrace::setEnabled(true);
    a()->touch();
    const int count = doc()->recompute();

    ASSERT_TRUE(App::RecomputeTrace::hasRun());
    const auto& run = App::RecomputeTrace::lastRun();

    EXPECT_TRUE(run.complete);
    EXPECT_EQ(run.recomputedCount, count);
    EXPECT_EQ(run.plan.size(), 3U);

    int recomputed = 0;
    double summed = 0.0;
    for (const auto& obj : run.objects) {
        if (obj.recomputed) {
            ++recomputed;
            EXPECT_GT(obj.seconds, 0.0) << obj.name << " recomputed in zero time";
            summed += obj.seconds;
        }
    }
    EXPECT_EQ(recomputed, count);
    EXPECT_DOUBLE_EQ(summed, run.tracedSeconds);
    // Per-object time must account for a real share of the run, and cannot
    // exceed it.
    EXPECT_LE(run.tracedSeconds, run.totalSeconds);
    EXPECT_GT(run.totalSeconds, 0.0);
}

// Falsification condition 1b: can it attribute causality?
TEST_F(RecomputeTraceTest, AttributesCausalityToPropagation)  // NOLINT
{
    buildChain(doc());
    App::RecomputeTrace::setEnabled(true);
    a()->touch();
    doc()->recompute();

    const auto& run = App::RecomputeTrace::lastRun();
    std::cout << "--- trace ---\n" << App::RecomputeTrace::lastRunAsJson() << "-------------\n";

    // The object the user touched is the root cause; the rest must be
    // explained by an observed propagation edge, not guessed.
    for (const auto& obj : run.objects) {
        if (!obj.recomputed) {
            continue;
        }
        EXPECT_NE(obj.cause, App::RecomputeCause::Unknown)
            << obj.name << " recomputed without an attributable cause";
        if (obj.cause == App::RecomputeCause::Propagated) {
            EXPECT_FALSE(obj.causeSource.empty());
        }
    }
    EXPECT_FALSE(run.edges.empty()) << "no propagation observed in a 3-object chain";
}

// Falsification condition 2: does observing change the observed?
TEST_F(RecomputeTraceTest, DoesNotChangeRecomputeSemantics)  // NOLINT
{
    buildChain(doc());
    a()->touch();
    const int untracedCount = doc()->recompute();
    const bool untracedTouched = a()->isTouched();
    const bool untracedError = a()->isError();

    App::RecomputeTrace::setEnabled(true);
    a()->touch();
    const int tracedCount = doc()->recompute();

    EXPECT_EQ(tracedCount, untracedCount);
    EXPECT_EQ(a()->isTouched(), untracedTouched);
    EXPECT_EQ(a()->isError(), untracedError);
}

// The trace must be consumable by a benchmark harness without Python.
TEST_F(RecomputeTraceTest, WritesJsonToOutputPath)  // NOLINT
{
    const std::string path = "/tmp/diofancad_recompute_trace_test.json";
    std::remove(path.c_str());

    buildChain(doc());
    App::RecomputeTrace::setOutputPath(path);
    EXPECT_TRUE(App::RecomputeTrace::isEnabled()) << "setting a path must enable the trace";
    a()->touch();
    doc()->recompute();

    std::ifstream in(path);
    ASSERT_TRUE(in.good()) << "trace file not written to " << path;
    std::stringstream buffer;
    buffer << in.rdbuf();
    const std::string json = buffer.str();

    EXPECT_NE(json.find("\"plan\""), std::string::npos);
    EXPECT_NE(json.find("\"objects\""), std::string::npos);
    EXPECT_NE(json.find("\"edges\""), std::string::npos);
    EXPECT_NE(json.find("\"cause\""), std::string::npos);
    std::remove(path.c_str());
}

// Overhead measurement for the P0-C evidence bundle. Disabled in normal runs
// because timing assertions are unreliable on shared CI machines; run with
//   ./App_tests_run --gtest_also_run_disabled_tests \
//                   --gtest_filter='RecomputeTraceTest.DISABLED_OverheadBenchmark'
// FeatureTest executes in microseconds, so this is close to the worst case for
// relative overhead: real features take milliseconds or more.
TEST_F(RecomputeTraceTest, DISABLED_OverheadBenchmark)  // NOLINT
{
    constexpr int objectCount = 2000;
    constexpr int repetitions = 7;

    App::FeatureTest* previous = nullptr;
    App::FeatureTest* root = nullptr;
    for (int i = 0; i < objectCount; ++i) {
        const std::string name = "F" + std::to_string(i);
        auto* obj =
            dynamic_cast<App::FeatureTest*>(doc()->addObject("App::FeatureTest", name.c_str()));
        if (previous != nullptr) {
            obj->Link.setValue(previous);
        }
        else {
            root = obj;
        }
        previous = obj;
    }
    doc()->recompute();

    auto measure = [&](bool traced) {
        App::RecomputeTrace::setEnabled(traced);
        std::vector<double> samples;
        samples.reserve(repetitions);
        for (int run = 0; run < repetitions; ++run) {
            root->touch();
            const auto start = std::chrono::steady_clock::now();
            doc()->recompute();
            samples.push_back(
                std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count());
        }
        std::sort(samples.begin(), samples.end());
        return samples[samples.size() / 2];  // median
    };

    // Interleave so machine drift hits both modes equally.
    const double offFirst = measure(false);
    const double onFirst = measure(true);
    const double offSecond = measure(false);
    const double onSecond = measure(true);

    const double off = std::min(offFirst, offSecond);
    const double on = std::min(onFirst, onSecond);

    std::cout << "objects=" << objectCount << " repetitions=" << repetitions << "\n"
              << "trace_disabled_median_s=" << off << "\n"
              << "trace_enabled_median_s=" << on << "\n"
              << "overhead_enabled_pct=" << ((on - off) / off) * 100.0 << "\n"
              << "overhead_enabled_ns_per_object=" << ((on - off) / objectCount) * 1e9 << "\n";

    const auto& run = App::RecomputeTrace::lastRun();
    std::cout << "attributed_fraction=" << (run.tracedSeconds / run.totalSeconds) << "\n"
              << "objects_traced=" << run.objects.size() << " edges=" << run.edges.size() << "\n";
}
