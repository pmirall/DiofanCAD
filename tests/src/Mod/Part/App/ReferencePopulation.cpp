// SPDX-License-Identifier: LGPL-2.1-or-later
//
// DiofanCAD round 010 — when does a reference actually hold a mapped name?
//
// Round 009 showed 48 headers CAN store mapped names. Round 007 measured one
// that held an empty one. Reading PropertyLinkBase::_updateElementReference
// gives the rule: a mapped name is stored only when GeoFeature::resolveElement
// returns a non-empty newName, which happens only when the referenced
// feature's element map resolves the subname.
//
// So this measures the CORRELATION between a target's element map and its
// references' populated state - not a "population rate", which would only
// describe the fixture.

#include <gtest/gtest.h>

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

#include <App/Application.h>
#include <App/Document.h>
#include <App/FeatureTest.h>
#include <Base/Vector3D.h>
#include <Mod/Part/App/FeatureFillet.h>
#include <Mod/Part/App/FeaturePartBox.h>
#include <Mod/Part/App/FeaturePartCut.h>
#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/App/TopoShape.h>
#include <src/App/InitApplication.h>

namespace
{

double faceCentreX(const Part::TopoShape& shape, int index)
{
    GProp_GProps props;
    BRepGProp::SurfaceProperties(shape.getSubShape(TopAbs_FACE, index), props);
    return props.CentreOfMass().X();
}

bool near(double value, double target, double tolerance = 1e-6)
{
    return std::abs(value - target) < tolerance;
}

/// One observation: what the target carries, and what its reference holds.
struct Observation
{
    std::string target;
    bool targetHasMap {false};
    std::size_t targetMapSize {0};
    bool referencePopulated {false};
    std::string mappedName;
    std::string oldName;
};

void report(const Observation& obs)
{
    std::cout << "  target=" << obs.target << " has_map=" << obs.targetHasMap
              << " map_size=" << obs.targetMapSize << " populated=" << obs.referencePopulated
              << " mapped='" << obs.mappedName << "' old='" << obs.oldName << "'" << std::endl;
}

}  // namespace

class ReferencePopulationTest: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
    }

    void SetUp() override
    {
        _docName = App::GetApplication().getUniqueDocumentName("pop");
        _doc = App::GetApplication().newDocument(_docName.c_str(), "testUser");
    }

    void TearDown() override
    {
        App::GetApplication().closeDocument(_docName.c_str());
    }

    App::Document* doc()
    {
        return _doc;
    }

    /// Reference the face of `target` whose centre sits at x, and report what
    /// the target carries and what the reference holds.
    Observation observe(Part::Feature* target, const char* label, double faceAtX,
                        const char* consumerName)
    {
        Observation obs;
        obs.target = label;
        // getShape(), never getValue(): the latter drops the element map.
        const Part::TopoShape& shape = target->Shape.getShape();
        obs.targetHasMap = shape.hasElementMap();
        obs.targetMapSize = shape.getElementMapSize(false);

        int index = 0;
        for (unsigned long i = 1; i <= shape.countSubShapes(TopAbs_FACE); ++i) {
            if (near(faceCentreX(shape, static_cast<int>(i)), faceAtX, 1e-3)) {
                index = static_cast<int>(i);
            }
        }
        if (index == 0) {
            obs.target += " (NO FACE FOUND)";
            return obs;
        }

        auto* consumer = _doc->addObject<App::FeatureTest>(consumerName);
        consumer->LinkSub.setValue(target,
                                   std::vector<std::string> {"Face" + std::to_string(index)});
        _doc->recompute();

        const auto shadow = consumer->LinkSub.getShadowSubs();
        if (!shadow.empty()) {
            obs.mappedName = shadow.front().newName;
            obs.oldName = shadow.front().oldName;
            obs.referencePopulated = !obs.mappedName.empty();
        }
        return obs;
    }

private:
    std::string _docName;
    App::Document* _doc {};
};

// M1 + M2: the correlation, across three kinds of target.
TEST_F(ReferencePopulationTest, M1_PopulationFollowsTheTarget)  // NOLINT
{
    auto* box = doc()->addObject<Part::Box>("Box");
    box->Length.setValue(20);
    box->Width.setValue(10);
    box->Height.setValue(10);
    auto* tool = doc()->addObject<Part::Box>("Tool");
    tool->Length.setValue(4);
    tool->Width.setValue(4);
    tool->Height.setValue(20);
    Base::Placement placement;
    placement.setPosition(Base::Vector3d(3, 3, -5));
    tool->Placement.setValue(placement);
    auto* cut = doc()->addObject<Part::Cut>("Cut");
    cut->Base.setValue(box);
    cut->Tool.setValue(tool);
    doc()->recompute();

    auto* fillet = doc()->addObject<Part::Fillet>("Fillet");
    fillet->Base.setValue(cut);
    std::vector<Part::FilletElement> edges;
    edges.emplace_back(1, 1.0, 1.0);
    fillet->Edges.setValues(edges);
    doc()->recompute();

    std::vector<Observation> observations;
    observations.push_back(observe(box, "Part::Box (primitive)", 20, "ConsumerBox"));
    observations.push_back(observe(cut, "Part::Cut (boolean)", 20, "ConsumerCut"));
    observations.push_back(observe(fillet, "Part::Fillet (dress-up)", 20, "ConsumerFillet"));

    std::cout << "M1 references_walked=" << observations.size()
              << " targets_inspected=" << observations.size() << std::endl;
    for (const auto& obs : observations) {
        report(obs);
    }

    // POSITIVE control: at least one populated name must appear, or every
    // "empty" reading in this file is an untested probe (D-018).
    const bool anyPopulated =
        std::any_of(observations.begin(), observations.end(), [](const Observation& o) {
            return o.referencePopulated;
        });
    ASSERT_TRUE(anyPopulated) << "POSITIVE CONTROL FAILED: the probe never saw a populated "
                                 "reference, so it cannot be trusted to report emptiness";

    // M2: the correlation must be exact, in both directions.
    for (const auto& obs : observations) {
        EXPECT_EQ(obs.targetHasMap, obs.referencePopulated)
            << obs.target << ": target map and reference population disagree, so the "
                             "source-derived rule and the harness contradict each other";
    }
}

// NEGATIVE control: the harness must be able to report a mismatch.
TEST_F(ReferencePopulationTest, N5_HarnessDetectsAWrongExpectation)  // NOLINT
{
    auto* box = doc()->addObject<Part::Box>("Box");
    box->Length.setValue(20);
    box->Width.setValue(10);
    box->Height.setValue(10);
    auto* tool = doc()->addObject<Part::Box>("Tool");
    tool->Length.setValue(4);
    tool->Width.setValue(4);
    tool->Height.setValue(20);
    Base::Placement placement;
    placement.setPosition(Base::Vector3d(3, 3, -5));
    tool->Placement.setValue(placement);
    auto* cut = doc()->addObject<Part::Cut>("Cut");
    cut->Base.setValue(box);
    cut->Tool.setValue(tool);
    doc()->recompute();

    const Observation obs = observe(cut, "Part::Cut", 20, "ConsumerNeg");
    report(obs);
    // Deliberately wrong expectation: a Cut reference is NOT populated.
    const bool wrongExpectation = !obs.referencePopulated;
    std::cout << "N5 wrong_expectation_holds=" << wrongExpectation << " (must be 0)" << std::endl;
    EXPECT_FALSE(wrongExpectation)
        << "the harness cannot tell populated from empty, so M1 means nothing";
}

// M3: is population durable, or transient state that a change wipes?
TEST_F(ReferencePopulationTest, M3_PopulationSurvivesRecomputeAndChange)  // NOLINT
{
    auto* box = doc()->addObject<Part::Box>("Box");
    box->Length.setValue(20);
    box->Width.setValue(10);
    box->Height.setValue(10);
    auto* tool = doc()->addObject<Part::Box>("Tool");
    tool->Length.setValue(4);
    tool->Width.setValue(4);
    tool->Height.setValue(20);
    Base::Placement placement;
    placement.setPosition(Base::Vector3d(3, 3, -5));
    tool->Placement.setValue(placement);
    auto* cut = doc()->addObject<Part::Cut>("Cut");
    cut->Base.setValue(box);
    cut->Tool.setValue(tool);
    doc()->recompute();

    const Part::TopoShape& shape = cut->Shape.getShape();
    int index = 0;
    for (unsigned long i = 1; i <= shape.countSubShapes(TopAbs_FACE); ++i) {
        if (near(faceCentreX(shape, static_cast<int>(i)), 20, 1e-3)) {
            index = static_cast<int>(i);
        }
    }
    ASSERT_GT(index, 0);

    auto* consumer = doc()->addObject<App::FeatureTest>("Consumer");
    consumer->LinkSub.setValue(cut, std::vector<std::string> {"Face" + std::to_string(index)});
    doc()->recompute();

    const auto initial = consumer->LinkSub.getShadowSubs();
    ASSERT_FALSE(initial.empty());
    const std::string first = initial.front().newName;
    std::cout << "M3 after_first_recompute='" << first << "'" << std::endl;
    ASSERT_FALSE(first.empty()) << "POSITIVE CONTROL FAILED before the change was applied";

    doc()->recompute();
    const std::string afterIdle = consumer->LinkSub.getShadowSubs().front().newName;
    std::cout << "M3 after_idle_recompute='" << afterIdle << "'" << std::endl;

    box->Length.setValue(30);
    doc()->recompute();
    const std::string afterChange = consumer->LinkSub.getShadowSubs().front().newName;
    std::cout << "M3 after_parameter_change='" << afterChange << "'" << std::endl;

    EXPECT_FALSE(afterIdle.empty()) << "an idle recompute cleared the mapped name";
    EXPECT_FALSE(afterChange.empty()) << "a parameter change cleared the mapped name";
}
