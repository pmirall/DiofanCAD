// SPDX-License-Identifier: LGPL-2.1-or-later
//
// DiofanCAD round 007 — P0-E: the change challenge at document level.
//
// Rounds 002-005 measured the identity layer by building shapes by hand and
// resolving references directly on them. A user does none of that: they change
// a parameter in a document, press recompute, and their references live inside
// features.
//
// A document reference is only as robust as the consumer that stores it, and
// the two available consumers store them very differently:
//
//   App::PropertyLinkSub   a subname plus a shadow mapped name, re-resolved
//                          through the element map
//   Part::Fillet           FilletElement { int edgeid; ... } - a raw index
//
// This measures both across the same change, and across a real save/reload.

#include <gtest/gtest.h>

#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include <BRepAdaptor_Surface.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS.hxx>

#include <App/Application.h>
#include <App/Document.h>
#include <App/FeatureTest.h>
#include <Base/Vector3D.h>
#include <Mod/Part/App/FeaturePartBox.h>
#include <Mod/Part/App/FeatureFillet.h>
#include <Mod/Part/App/FeaturePartCut.h>
#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/App/TopoShape.h>
#include <src/App/InitApplication.h>

namespace
{

/// Centre of a face or edge, in absolute coordinates. Ground truth is physical.
Base::Vector3d centreOf(const TopoDS_Shape& shape, bool linear)
{
    GProp_GProps props;
    if (linear) {
        BRepGProp::LinearProperties(shape, props);
    }
    else {
        BRepGProp::SurfaceProperties(shape, props);
    }
    const gp_Pnt point = props.CentreOfMass();
    return Base::Vector3d(point.X(), point.Y(), point.Z());
}

/// The cylindrical face a fillet creates, identified by its axis position.
/// Returns false when the shape carries no fillet.
bool filletCorner(const Part::TopoShape& shape, Base::Vector3d& corner)
{
    const auto faceCount = shape.countSubShapes(TopAbs_FACE);
    for (unsigned long i = 1; i <= faceCount; ++i) {
        const TopoDS_Shape face = shape.getSubShape(TopAbs_FACE, static_cast<int>(i));
        BRepAdaptor_Surface surface(TopoDS::Face(face));
        if (surface.GetType() == GeomAbs_Cylinder) {
            corner = centreOf(face, false);
            return true;
        }
    }
    return false;
}

/// Which XY corner of the box is this point nearest to? Returns the corner.
/// A fillet's cylindrical face centre sits inside its corner by a radius-
/// dependent offset, so comparing it to corner coordinates directly is wrong.
Base::Vector3d nearestCorner(const Base::Vector3d& point, double maxX, double maxY)
{
    Base::Vector3d best(0, 0, 0);
    double bestDistance = 1e30;
    for (const double x : {0.0, maxX}) {
        for (const double y : {0.0, maxY}) {
            const double distance =
                (point.x - x) * (point.x - x) + (point.y - y) * (point.y - y);
            if (distance < bestDistance) {
                bestDistance = distance;
                best = Base::Vector3d(x, y, 0);
            }
        }
    }
    return best;
}

bool near(double value, double target, double tolerance = 1e-6)
{
    return std::abs(value - target) < tolerance;
}

/// Which vertical edge of a box sits at this corner in XY?
int verticalEdgeAt(const Part::TopoShape& shape, double x, double y)
{
    const auto edgeCount = shape.countSubShapes(TopAbs_EDGE);
    for (unsigned long i = 1; i <= edgeCount; ++i) {
        const auto index = static_cast<int>(i);
        const TopoDS_Shape edge = shape.getSubShape(TopAbs_EDGE, index);
        const Base::Vector3d centre = centreOf(edge, true);
        if (near(centre.x, x, 1e-3) && near(centre.y, y, 1e-3)) {
            // vertical edges have their centre at mid-height
            return index;
        }
    }
    return 0;
}

}  // namespace

class DocumentChangeChallengeTest: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
    }

    void SetUp() override
    {
        _docName = App::GetApplication().getUniqueDocumentName("p0e");
        _doc = App::GetApplication().newDocument(_docName.c_str(), "testUser");
    }

    void TearDown() override
    {
        if (App::GetApplication().getDocument(_docName.c_str()) != nullptr) {
            App::GetApplication().closeDocument(_docName.c_str());
        }
    }

    App::Document* doc()
    {
        return _doc;
    }

    const std::string& docName() const
    {
        return _docName;
    }

private:
    std::string _docName;
    App::Document* _doc {};
};

// M2: a Part::Fillet stores a raw edge index. Does it stay on the same
// physical corner when the base box is lengthened?
TEST_F(DocumentChangeChallengeTest, M2_FilletStaysOnTheSameCorner)  // NOLINT
{
    auto* box = doc()->addObject<Part::Box>("Box");
    box->Length.setValue(20);
    box->Width.setValue(10);
    box->Height.setValue(10);
    doc()->recompute();

    // Fillet the vertical edge at the far corner (x=20, y=10).
    const Part::TopoShape boxShape = box->Shape.getValue();
    const int edgeIndex = verticalEdgeAt(boxShape, 20, 10);
    ASSERT_GT(edgeIndex, 0) << "no vertical edge found at the far corner";

    auto* fillet = doc()->addObject<Part::Fillet>("Fillet");
    fillet->Base.setValue(box);
    std::vector<Part::FilletElement> edges;
    edges.emplace_back(edgeIndex, 2.0, 2.0);
    fillet->Edges.setValues(edges);
    doc()->recompute();

    Base::Vector3d cornerBefore;
    ASSERT_TRUE(filletCorner(fillet->Shape.getValue(), cornerBefore))
        << "the fillet produced no cylindrical face";
    std::cout << "M2 edge_index=" << edgeIndex << " corner_before=(" << cornerBefore.x << ","
              << cornerBefore.y << ")" << std::endl;

    // The engineer lengthens the box. The filleted corner should follow to
    // (30, 10) - it is still "the far corner".
    box->Length.setValue(30);
    doc()->recompute();

    Base::Vector3d cornerAfter;
    const bool stillFilleted = filletCorner(fillet->Shape.getValue(), cornerAfter);
    ASSERT_TRUE(stillFilleted) << "the fillet vanished after the change";
    std::cout << "M2 corner_after=(" << cornerAfter.x << "," << cornerAfter.y << ")"
              << " expected=(30,10)" << std::endl;

    const Base::Vector3d cornerIdBefore = nearestCorner(cornerBefore, 20, 10);
    const Base::Vector3d cornerIdAfter = nearestCorner(cornerAfter, 30, 10);
    std::cout << "M2 nearest_corner_before=(" << cornerIdBefore.x << "," << cornerIdBefore.y
              << ") nearest_corner_after=(" << cornerIdAfter.x << "," << cornerIdAfter.y
              << ") expected_after=(30,10)" << std::endl;

    const bool followed =
        near(cornerIdAfter.x, 30, 1e-3) && near(cornerIdAfter.y, 10, 1e-3);
    if (!followed) {
        std::cout << "M2 THE FILLET MOVED to a different physical corner" << std::endl;
    }
    EXPECT_TRUE(followed) << "a raw-index reference put the fillet on a different corner";
}

// M1: a PropertyLinkSub reference to a face, through a real recompute.
TEST_F(DocumentChangeChallengeTest, M1_LinkSubReferenceSurvivesChange)  // NOLINT
{
    auto* box = doc()->addObject<Part::Box>("Box");
    box->Length.setValue(20);
    box->Width.setValue(10);
    box->Height.setValue(10);
    doc()->recompute();

    // Identify the +X face physically, then store a reference to it the way a
    // feature would: object plus subname.
    const Part::TopoShape shape = box->Shape.getValue();
    int target = 0;
    for (unsigned long i = 1; i <= shape.countSubShapes(TopAbs_FACE); ++i) {
        const auto index = static_cast<int>(i);
        if (near(centreOf(shape.getSubShape(TopAbs_FACE, index), false).x, 20)) {
            target = index;
        }
    }
    ASSERT_GT(target, 0);
    const std::string subname = "Face" + std::to_string(target);

    auto* consumer = doc()->addObject<App::FeatureTest>("Consumer");
    consumer->LinkSub.setValue(box, std::vector<std::string> {subname});
    doc()->recompute();

    std::cout << "M1 box_element_map_size=" << shape.getElementMapSize(false)
              << " has_element_map=" << shape.hasElementMap() << std::endl;

    const auto shadowBefore = consumer->LinkSub.getShadowSubs();
    std::cout << "M1 subname=" << subname << " shadow_entries=" << shadowBefore.size();
    if (!shadowBefore.empty()) {
        std::cout << " mapped='" << shadowBefore.front().newName << "'";
    }
    std::cout << std::endl;
    // D-015: say whether the mechanism under test was exercised at all.
    if (shadowBefore.empty() || shadowBefore.front().newName.empty()) {
        std::cout << "M1 NOT EXERCISED: no shadow mapped name was stored, so this measures the "
                     "index path, not the element map"
                  << std::endl;
    }

    box->Length.setValue(30);
    doc()->recompute();

    const auto subsAfter = consumer->LinkSub.getSubValues();
    ASSERT_FALSE(subsAfter.empty());
    const std::string resolvedSub = subsAfter.front();

    // Where does the stored reference physically point now?
    const Part::TopoShape after = box->Shape.getValue();
    int resolvedIndex = 0;
    if (sscanf(resolvedSub.c_str(), "Face%d", &resolvedIndex) != 1) {
        resolvedIndex = 0;
    }
    std::cout << "M1 sub_after=" << resolvedSub;
    if (resolvedIndex > 0
        && resolvedIndex <= static_cast<int>(after.countSubShapes(TopAbs_FACE))) {
        const double x = centreOf(after.getSubShape(TopAbs_FACE, resolvedIndex), false).x;
        std::cout << " centre_x=" << x << " expected=30";
        EXPECT_TRUE(near(x, 30)) << "the reference now points at a different physical face";
    }
    std::cout << std::endl;
}

// M3: a real save -> close -> reopen cycle, closing round 005's document gap.
TEST_F(DocumentChangeChallengeTest, M3_ReferencesSurviveSaveAndReopen)  // NOLINT
{
    auto* box = doc()->addObject<Part::Box>("Box");
    box->Length.setValue(20);
    box->Width.setValue(10);
    box->Height.setValue(10);
    doc()->recompute();

    const int edgeIndex = verticalEdgeAt(box->Shape.getValue(), 20, 10);
    ASSERT_GT(edgeIndex, 0);
    auto* fillet = doc()->addObject<Part::Fillet>("Fillet");
    fillet->Base.setValue(box);
    std::vector<Part::FilletElement> edges;
    edges.emplace_back(edgeIndex, 2.0, 2.0);
    fillet->Edges.setValues(edges);
    doc()->recompute();

    Base::Vector3d cornerBefore;
    ASSERT_TRUE(filletCorner(fillet->Shape.getValue(), cornerBefore));

    const std::string path = "/tmp/diofancad_p0e_roundtrip.FCStd";
    std::remove(path.c_str());
    ASSERT_TRUE(doc()->saveAs(path.c_str())) << "saveAs failed";
    const std::string name = docName();
    App::GetApplication().closeDocument(name.c_str());

    App::Document* reopened = App::GetApplication().openDocument(path.c_str());
    ASSERT_NE(reopened, nullptr) << "the document did not reopen";

    auto* reloadedFillet =
        dynamic_cast<Part::Fillet*>(reopened->getObject("Fillet"));
    ASSERT_NE(reloadedFillet, nullptr) << "the fillet did not survive the reload";

    Base::Vector3d cornerAfter;
    const bool stillFilleted = filletCorner(reloadedFillet->Shape.getValue(), cornerAfter);
    std::cout << "M3 reopened=" << (reopened != nullptr) << " fillet_present=" << stillFilleted
              << " corner_before=(" << cornerBefore.x << "," << cornerBefore.y << ")"
              << " corner_after=(" << cornerAfter.x << "," << cornerAfter.y << ")" << std::endl;

    EXPECT_TRUE(stillFilleted);
    EXPECT_TRUE(near(cornerAfter.x, cornerBefore.x, 1e-3)
                && near(cornerAfter.y, cornerBefore.y, 1e-3))
        << "the fillet moved across a save/reload";

    App::GetApplication().closeDocument(reopened->getName());
    std::remove(path.c_str());
}

// NEGATIVE CONTROL: the physical comparison must be able to report a mismatch.
TEST_F(DocumentChangeChallengeTest, N4_ComparisonDetectsAMovedCorner)  // NOLINT
{
    auto* box = doc()->addObject<Part::Box>("Box");
    box->Length.setValue(20);
    box->Width.setValue(10);
    box->Height.setValue(10);
    doc()->recompute();

    const int nearEdge = verticalEdgeAt(box->Shape.getValue(), 0, 0);
    ASSERT_GT(nearEdge, 0);
    auto* fillet = doc()->addObject<Part::Fillet>("Fillet");
    fillet->Base.setValue(box);
    std::vector<Part::FilletElement> edges;
    edges.emplace_back(nearEdge, 2.0, 2.0);
    fillet->Edges.setValues(edges);
    doc()->recompute();

    Base::Vector3d corner;
    ASSERT_TRUE(filletCorner(fillet->Shape.getValue(), corner));
    // The fillet is at the origin corner; assert it is NOT at the far corner.
    const bool atFarCorner = near(corner.x, 20, 1e-3) && near(corner.y, 10, 1e-3);
    std::cout << "N4 corner=(" << corner.x << "," << corner.y << ") at_far_corner=" << atFarCorner
              << " (must be 0)" << std::endl;
    EXPECT_FALSE(atFarCorner) << "the comparison cannot distinguish two corners, so every "
                                 "'stayed put' result in this file is meaningless";
}

// Probe: which document-level features carry an element map at all?
TEST_F(DocumentChangeChallengeTest, P1_WhichFeaturesCarryElementMaps)  // NOLINT
{
    auto* base = doc()->addObject<Part::Box>("Base");
    base->Length.setValue(20);
    base->Width.setValue(10);
    base->Height.setValue(10);
    auto* tool = doc()->addObject<Part::Box>("Tool");
    tool->Length.setValue(4);
    tool->Width.setValue(4);
    tool->Height.setValue(20);
    Base::Placement placement;
    placement.setPosition(Base::Vector3d(3, 3, -5));
    tool->Placement.setValue(placement);

    auto* cut = doc()->addObject<Part::Cut>("Cut");
    cut->Base.setValue(base);
    cut->Tool.setValue(tool);

    auto* fillet = doc()->addObject<Part::Fillet>("Fillet");
    fillet->Base.setValue(base);
    std::vector<Part::FilletElement> edges;
    doc()->recompute();
    const int edgeIndex = verticalEdgeAt(base->Shape.getValue(), 20, 10);
    edges.emplace_back(edgeIndex > 0 ? edgeIndex : 1, 2.0, 2.0);
    fillet->Edges.setValues(edges);
    doc()->recompute();

    const auto report = [](const char* label, const Part::TopoShape& shape) {
        std::cout << "P1 " << label << " has_element_map=" << shape.hasElementMap()
                  << " element_map_size=" << shape.getElementMapSize(false)
                  << " faces=" << shape.countSubShapes(TopAbs_FACE) << std::endl;
    };
    // getValue() returns a TopoDS_Shape; binding that to a TopoShape parameter
    // CONSTRUCTS a fresh TopoShape and drops the element map. getShape() returns
    // the real TopoShape. Report both, because the difference is the difference
    // between a finding and a bug in this harness.
    std::cout << "P1 --- via getValue() (constructs a new TopoShape) ---" << std::endl;
    report("Part::Box  ", base->Shape.getValue());
    report("Part::Cut  ", cut->Shape.getValue());
    report("Part::Fillet", fillet->Shape.getValue());
    std::cout << "P1 --- via getShape() (the stored TopoShape) ---" << std::endl;
    report("Part::Box  ", base->Shape.getShape());
    report("Part::Cut  ", cut->Shape.getShape());
    report("Part::Fillet", fillet->Shape.getShape());
}

// P2: round 007 measured the consumer side against a Part::Box, which
// genuinely has no element map. Re-measure against a Part::Cut, which does.
TEST_F(DocumentChangeChallengeTest, P2_ConsumerAgainstAMappedFeature)  // NOLINT
{
    auto* base = doc()->addObject<Part::Box>("Base");
    base->Length.setValue(20);
    base->Width.setValue(10);
    base->Height.setValue(10);
    auto* tool = doc()->addObject<Part::Box>("Tool");
    tool->Length.setValue(4);
    tool->Width.setValue(4);
    tool->Height.setValue(20);
    Base::Placement placement;
    placement.setPosition(Base::Vector3d(3, 3, -5));
    tool->Placement.setValue(placement);
    auto* cut = doc()->addObject<Part::Cut>("Cut");
    cut->Base.setValue(base);
    cut->Tool.setValue(tool);
    doc()->recompute();

    const Part::TopoShape& cutShape = cut->Shape.getShape();
    std::cout << "P2 cut_has_element_map=" << cutShape.hasElementMap()
              << " size=" << cutShape.getElementMapSize(false) << std::endl;

    // Reference the +X outer face of the cut result.
    int target = 0;
    for (unsigned long i = 1; i <= cutShape.countSubShapes(TopAbs_FACE); ++i) {
        const auto index = static_cast<int>(i);
        if (near(centreOf(cutShape.getSubShape(TopAbs_FACE, index), false).x, 20)) {
            target = index;
        }
    }
    ASSERT_GT(target, 0);

    auto* consumer = doc()->addObject<App::FeatureTest>("Consumer");
    consumer->LinkSub.setValue(cut, std::vector<std::string> {"Face" + std::to_string(target)});
    doc()->recompute();

    const auto shadow = consumer->LinkSub.getShadowSubs();
    std::cout << "P2 subname=Face" << target << " shadow_entries=" << shadow.size();
    if (!shadow.empty()) {
        std::cout << " mapped='" << shadow.front().newName << "'"
                  << " old='" << shadow.front().oldName << "'";
    }
    std::cout << std::endl;
    if (shadow.empty() || shadow.front().newName.empty()) {
        std::cout << "P2 the consumer stored NO mapped name even against a mapped feature"
                  << std::endl;
    }
    else {
        std::cout << "P2 the consumer DID store a mapped name" << std::endl;
    }
}
