// SPDX-License-Identifier: LGPL-2.1-or-later
//
// DiofanCAD P0-A — reference resilience under topology-changing edits.
//
// A measurement harness, not a feature. It asks the question the baseline's
// own tests do not:
//
//     after a realistic change, does a stored reference still point at the
//     face the engineer meant?
//
// The distinction that matters is between a reference that BREAKS (visible,
// recoverable) and one that silently RESOLVES TO THE WRONG FACE (invisible,
// and far more dangerous). Name-level tests cannot separate them: both
// produce a name that looks valid.
//
// Ground truth is geometric. For the axis-aligned box scenarios used here, a
// face's engineering identity is its outward normal plus which side of the
// solid it sits on. Scenarios are restricted to cases where that identity is
// unambiguous.
//
// This works at TopoShape level, like upstream's own toponaming tests. A
// modelling operation is re-run with a changed operand under the same tags,
// which is what a document recompute does to the element map after a
// parameter change.

#include <gtest/gtest.h>

#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <BRepAdaptor_Surface.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <gp_Pln.hxx>
#include <gp_Trsf.hxx>

#include <Base/BoundBox.h>
#include <Base/Vector3D.h>
#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/App/TopoShapeOpCode.h>
#include <src/App/InitApplication.h>

using Part::TopoShape;

namespace
{

/// What a face looks like to an engineer, independent of its index.
struct FaceFingerprint
{
    Base::Vector3d center;
    Base::Vector3d normal;
    double area {0.0};
    bool planar {false};
    /// True when the face lies on the solid's bounding box, i.e. it is an
    /// outer wall rather than the wall of a pocket. This is what separates
    /// the block's +X face from the pocket wall that also faces +X.
    bool outerBoundary {false};
};

enum class ReferenceOutcome
{
    ResolvedCorrect,  //< still points at the intended face
    ResolvedWrong,    //< resolves, but to a different face: silent corruption
    Broken,           //< no longer resolves, though the target still exists
    TargetGone        //< the intended face genuinely no longer exists
};

/// Does this face sit on the solid's bounding box?
bool liesOnBoundingBox(const Base::Vector3d& center,
                       const Base::Vector3d& normal,
                       const Base::BoundBox3d& box)
{
    constexpr double tolerance = 1e-6;
    constexpr double axisThreshold = 0.5;
    if (normal.x > axisThreshold) {
        return std::abs(center.x - box.MaxX) < tolerance;
    }
    if (normal.x < -axisThreshold) {
        return std::abs(center.x - box.MinX) < tolerance;
    }
    if (normal.y > axisThreshold) {
        return std::abs(center.y - box.MaxY) < tolerance;
    }
    if (normal.y < -axisThreshold) {
        return std::abs(center.y - box.MinY) < tolerance;
    }
    if (normal.z > axisThreshold) {
        return std::abs(center.z - box.MaxZ) < tolerance;
    }
    if (normal.z < -axisThreshold) {
        return std::abs(center.z - box.MinZ) < tolerance;
    }
    return false;
}

FaceFingerprint fingerprintOf(const TopoDS_Shape& shape, const Base::BoundBox3d& solidBox)
{
    FaceFingerprint print;
    GProp_GProps props;
    BRepGProp::SurfaceProperties(shape, props);
    print.area = props.Mass();
    const gp_Pnt centre = props.CentreOfMass();
    print.center = Base::Vector3d(centre.X(), centre.Y(), centre.Z());

    const TopoDS_Face face = TopoDS::Face(shape);
    BRepAdaptor_Surface surface(face);
    if (surface.GetType() == GeomAbs_Plane) {
        gp_Dir direction = surface.Plane().Axis().Direction();
        if (face.Orientation() == TopAbs_REVERSED) {
            direction.Reverse();
        }
        print.normal = Base::Vector3d(direction.X(), direction.Y(), direction.Z());
        print.planar = true;
        print.outerBoundary = liesOnBoundingBox(print.center, print.normal, solidBox);
    }
    return print;
}

/// The same face in engineering terms.
///
/// Identity is the outward normal plus whether the face is an outer wall or a
/// pocket wall. Deliberately NOT position: a parameter change is supposed to
/// move faces, and an identity that breaks when a face moves would report
/// correct behaviour as corruption. In these scenarios the pair (normal,
/// outer/pocket) is unique across all ten faces, so it is a complete identity
/// without being a positional one.
bool sameEngineeringFace(const FaceFingerprint& before, const FaceFingerprint& after)
{
    if (!before.planar || !after.planar) {
        return false;
    }
    constexpr double angularTolerance = 1e-6;
    if ((before.normal - after.normal).Length() > angularTolerance) {
        return false;
    }
    return before.outerBoundary == after.outerBoundary;
}

struct StoredReference
{
    Data::MappedName mapped;
    std::string indexAtCapture;
    FaceFingerprint print;
};

std::vector<StoredReference> captureFaceReferences(const TopoShape& shape)
{
    std::vector<StoredReference> refs;
    const auto faceCount = shape.countSubShapes(TopAbs_FACE);
    for (unsigned long i = 1; i <= faceCount; ++i) {
        const Data::IndexedName index("Face", static_cast<int>(i));
        // allowUnmapped stays false on purpose: an unmapped name is not a
        // persistent reference, and counting one would inflate the result.
        Data::MappedName mapped = shape.getMappedName(index, false);
        if (!mapped) {
            continue;
        }
        StoredReference ref;
        ref.mapped = mapped;
        std::string buffer;
        index.appendToStringBuffer(buffer);
        ref.indexAtCapture = buffer;
        ref.print =
            fingerprintOf(shape.getSubShape(TopAbs_FACE, static_cast<int>(i)), shape.getBoundBox());
        refs.push_back(std::move(ref));
    }
    return refs;
}

/// Phase 2 probe: how many faces of the changed shape match the stored
/// descriptor? Exactly one means a repair could be proposed unambiguously.
/// More than one is the "unacceptable ambiguity" the proof must rule out.
int countCandidates(const TopoShape& shape, const FaceFingerprint& print)
{
    const Base::BoundBox3d box = shape.getBoundBox();
    const auto faceCount = shape.countSubShapes(TopAbs_FACE);
    int candidates = 0;
    for (unsigned long i = 1; i <= faceCount; ++i) {
        if (sameEngineeringFace(
                print,
                fingerprintOf(shape.getSubShape(TopAbs_FACE, static_cast<int>(i)), box))) {
            ++candidates;
        }
    }
    return candidates;
}

bool targetStillExists(const TopoShape& shape, const FaceFingerprint& print)
{
    const Base::BoundBox3d box = shape.getBoundBox();
    const auto faceCount = shape.countSubShapes(TopAbs_FACE);
    for (unsigned long i = 1; i <= faceCount; ++i) {
        if (sameEngineeringFace(
                print,
                fingerprintOf(shape.getSubShape(TopAbs_FACE, static_cast<int>(i)), box))) {
            return true;
        }
    }
    return false;
}

ReferenceOutcome followReference(const TopoShape& after, const StoredReference& ref)
{
    const Data::IndexedName resolved = after.getIndexedName(ref.mapped);
    if (!resolved) {
        return targetStillExists(after, ref.print) ? ReferenceOutcome::Broken
                                                   : ReferenceOutcome::TargetGone;
    }
    const TopoDS_Shape face = after.getSubShape(TopAbs_FACE, resolved.getIndex(), true);
    if (face.IsNull()) {
        return ReferenceOutcome::Broken;
    }
    return sameEngineeringFace(ref.print, fingerprintOf(face, after.getBoundBox()))
        ? ReferenceOutcome::ResolvedCorrect
        : ReferenceOutcome::ResolvedWrong;
}

struct ChallengeResult
{
    std::string scenario;
    int captured {0};
    int correct {0};
    int wrong {0};
    int broken {0};
    int gone {0};
    int recoverableUnambiguously {0};
    int recoverableAmbiguously {0};

    void record(ReferenceOutcome outcome, const StoredReference& ref, int candidates = -1)
    {
        if (outcome == ReferenceOutcome::Broken) {
            if (candidates == 1) {
                ++recoverableUnambiguously;
            }
            else if (candidates > 1) {
                ++recoverableAmbiguously;
            }
        }
        if (outcome != ReferenceOutcome::ResolvedCorrect) {
            std::cout << "  " << scenario << ": " << ref.indexAtCapture << " ("
                      << (ref.print.outerBoundary ? "outer" : "pocket") << " wall, normal "
                      << ref.print.normal.x << "," << ref.print.normal.y << ","
                      << ref.print.normal.z << ") -> "
                      << (outcome == ReferenceOutcome::ResolvedWrong ? "RESOLVED-WRONG"
                          : outcome == ReferenceOutcome::Broken      ? "broken"
                                                                     : "target-gone");
            if (candidates >= 0) {
                std::cout << ", candidates=" << candidates;
            }
            std::cout << std::endl;
        }
        switch (outcome) {
            case ReferenceOutcome::ResolvedCorrect:
                ++correct;
                break;
            case ReferenceOutcome::ResolvedWrong:
                ++wrong;
                break;
            case ReferenceOutcome::Broken:
                ++broken;
                break;
            case ReferenceOutcome::TargetGone:
                ++gone;
                break;
        }
    }

    void report() const
    {
        std::cout << "scenario=" << scenario << " captured=" << captured << " correct=" << correct
                  << " wrong=" << wrong << " broken=" << broken << " target_gone=" << gone
                  << " recoverable_unambiguous=" << recoverableUnambiguously
                  << " recoverable_ambiguous=" << recoverableAmbiguously << std::endl;
    }
};

TopoShape makeBox(double dx, double dy, double dz, const Base::Vector3d& at, long tag)
{
    TopoDS_Shape box = BRepPrimAPI_MakeBox(dx, dy, dz).Shape();
    if (at.Length() > 0.0) {
        gp_Trsf transform;
        transform.SetTranslation(gp_Vec(at.x, at.y, at.z));
        box.Move(TopLoc_Location(transform));
    }
    return TopoShape {box, tag};
}

/// Re-run the same cut with the same tags, which is what a document recompute
/// does after a parameter change on an operand.
TopoShape cutBoxes(const TopoShape& base, const TopoShape& tool)
{
    TopoShape result {base};
    result.makeElementBoolean(Part::OpCodes::Cut, {base, tool});
    return result;
}

}  // namespace

class ReferenceResilienceTest: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
    }
};

// S1 (control): the operation is re-run with an identical operand. If
// references do not survive this, nothing else in the suite is interpretable.
TEST_F(ReferenceResilienceTest, S1_RecomputeWithoutChange)  // NOLINT
{
    const TopoShape base = makeBox(10, 10, 10, Base::Vector3d(0, 0, 0), 1L);
    const TopoShape tool = makeBox(4, 4, 20, Base::Vector3d(3, 3, -5), 2L);
    const TopoShape before = cutBoxes(base, tool);

    const auto refs = captureFaceReferences(before);
    ChallengeResult result;
    result.scenario = "S1-recompute-no-change";
    result.captured = static_cast<int>(refs.size());
    ASSERT_GT(result.captured, 0) << "no stable face names captured at all";

    const TopoShape base2 = makeBox(10, 10, 10, Base::Vector3d(0, 0, 0), 1L);
    const TopoShape tool2 = makeBox(4, 4, 20, Base::Vector3d(3, 3, -5), 2L);
    const TopoShape after = cutBoxes(base2, tool2);

    for (const auto& ref : refs) {
        const ReferenceOutcome outcome = followReference(after, ref);
        const int candidates = outcome == ReferenceOutcome::Broken
            ? countCandidates(after, ref.print)
            : -1;
        result.record(outcome, ref, candidates);
    }
    result.report();

    EXPECT_EQ(result.wrong, 0) << "a reference silently resolved to the wrong face";
    EXPECT_EQ(result.broken, 0) << "a reference broke on an unchanged recompute";
}

// S2: the pocket is widened. Faces change size; the through-cut stays a
// through-cut, so every captured face still has an engineering counterpart
// except the pocket walls, which move.
TEST_F(ReferenceResilienceTest, S2_PocketWidened)  // NOLINT
{
    const TopoShape base = makeBox(10, 10, 10, Base::Vector3d(0, 0, 0), 1L);
    const TopoShape tool = makeBox(4, 4, 20, Base::Vector3d(3, 3, -5), 2L);
    const TopoShape before = cutBoxes(base, tool);

    const auto refs = captureFaceReferences(before);
    ChallengeResult result;
    result.scenario = "S2-pocket-widened";
    result.captured = static_cast<int>(refs.size());
    ASSERT_GT(result.captured, 0);

    const TopoShape base2 = makeBox(10, 10, 10, Base::Vector3d(0, 0, 0), 1L);
    const TopoShape tool2 = makeBox(6, 6, 20, Base::Vector3d(2, 2, -5), 2L);
    const TopoShape after = cutBoxes(base2, tool2);

    for (const auto& ref : refs) {
        const ReferenceOutcome outcome = followReference(after, ref);
        const int candidates = outcome == ReferenceOutcome::Broken
            ? countCandidates(after, ref.print)
            : -1;
        result.record(outcome, ref, candidates);
    }
    result.report();

    EXPECT_EQ(result.wrong, 0) << "a reference silently resolved to the wrong face";
}

// S3: the hard case. The cutter stops short, turning a through pocket into a
// blind one and creating a floor face that did not exist before.
TEST_F(ReferenceResilienceTest, S3_ThroughPocketBecomesBlind)  // NOLINT
{
    const TopoShape base = makeBox(10, 10, 10, Base::Vector3d(0, 0, 0), 1L);
    const TopoShape tool = makeBox(4, 4, 20, Base::Vector3d(3, 3, -5), 2L);
    const TopoShape before = cutBoxes(base, tool);

    const auto refs = captureFaceReferences(before);
    ChallengeResult result;
    result.scenario = "S3-through-to-blind";
    result.captured = static_cast<int>(refs.size());
    ASSERT_GT(result.captured, 0);

    const TopoShape base2 = makeBox(10, 10, 10, Base::Vector3d(0, 0, 0), 1L);
    const TopoShape tool2 = makeBox(4, 4, 6, Base::Vector3d(3, 3, 4), 2L);
    const TopoShape after = cutBoxes(base2, tool2);

    for (const auto& ref : refs) {
        const ReferenceOutcome outcome = followReference(after, ref);
        const int candidates = outcome == ReferenceOutcome::Broken
            ? countCandidates(after, ref.print)
            : -1;
        result.record(outcome, ref, candidates);
    }
    result.report();

    // No assertion on broken: a genuine topology change may legitimately
    // invalidate a reference. Silent corruption is the failure that matters.
    EXPECT_EQ(result.wrong, 0) << "a reference silently resolved to the wrong face";
}

// S4: the tool is replaced by a differently shaped solid carrying a different
// tag, the "component replacement" attack from the red-team plan.
TEST_F(ReferenceResilienceTest, S4_ToolReplaced)  // NOLINT
{
    const TopoShape base = makeBox(10, 10, 10, Base::Vector3d(0, 0, 0), 1L);
    const TopoShape tool = makeBox(4, 4, 20, Base::Vector3d(3, 3, -5), 2L);
    const TopoShape before = cutBoxes(base, tool);

    const auto refs = captureFaceReferences(before);
    ChallengeResult result;
    result.scenario = "S4-tool-replaced";
    result.captured = static_cast<int>(refs.size());
    ASSERT_GT(result.captured, 0);

    const TopoShape base2 = makeBox(10, 10, 10, Base::Vector3d(0, 0, 0), 1L);
    const TopoShape replacement = makeBox(5, 5, 20, Base::Vector3d(2, 2, -5), 3L);
    const TopoShape after = cutBoxes(base2, replacement);

    for (const auto& ref : refs) {
        const ReferenceOutcome outcome = followReference(after, ref);
        const int candidates = outcome == ReferenceOutcome::Broken
            ? countCandidates(after, ref.print)
            : -1;
        result.record(outcome, ref, candidates);
    }
    result.report();

    EXPECT_EQ(result.wrong, 0) << "a reference silently resolved to the wrong face";
}

// S5: an extra feature is inserted upstream. This is the classic toponaming
// trigger: a second pocket is cut first, shifting the face numbering that the
// original reference was captured against.
TEST_F(ReferenceResilienceTest, S5_UpstreamFeatureInserted)  // NOLINT
{
    const TopoShape base = makeBox(10, 10, 10, Base::Vector3d(0, 0, 0), 1L);
    const TopoShape tool = makeBox(4, 4, 20, Base::Vector3d(3, 3, -5), 2L);
    const TopoShape before = cutBoxes(base, tool);

    const auto refs = captureFaceReferences(before);
    ChallengeResult result;
    result.scenario = "S5-upstream-feature-inserted";
    result.captured = static_cast<int>(refs.size());
    ASSERT_GT(result.captured, 0);

    // Same final pocket, but an unrelated notch is now cut first.
    const TopoShape base2 = makeBox(10, 10, 10, Base::Vector3d(0, 0, 0), 1L);
    const TopoShape notch = makeBox(2, 2, 2, Base::Vector3d(0, 0, 0), 4L);
    const TopoShape notched = cutBoxes(base2, notch);
    const TopoShape tool2 = makeBox(4, 4, 20, Base::Vector3d(3, 3, -5), 2L);
    const TopoShape after = cutBoxes(notched, tool2);

    for (const auto& ref : refs) {
        const ReferenceOutcome outcome = followReference(after, ref);
        const int candidates = outcome == ReferenceOutcome::Broken
            ? countCandidates(after, ref.print)
            : -1;
        result.record(outcome, ref, candidates);
    }
    result.report();

    EXPECT_EQ(result.wrong, 0) << "a reference silently resolved to the wrong face";
}

// S6: the base itself is resized. Every outer face moves; the pocket stays put.
TEST_F(ReferenceResilienceTest, S6_BaseResized)  // NOLINT
{
    const TopoShape base = makeBox(10, 10, 10, Base::Vector3d(0, 0, 0), 1L);
    const TopoShape tool = makeBox(4, 4, 20, Base::Vector3d(3, 3, -5), 2L);
    const TopoShape before = cutBoxes(base, tool);

    const auto refs = captureFaceReferences(before);
    ChallengeResult result;
    result.scenario = "S6-base-resized";
    result.captured = static_cast<int>(refs.size());
    ASSERT_GT(result.captured, 0);

    const TopoShape base2 = makeBox(16, 12, 10, Base::Vector3d(0, 0, 0), 1L);
    const TopoShape tool2 = makeBox(4, 4, 20, Base::Vector3d(3, 3, -5), 2L);
    const TopoShape after = cutBoxes(base2, tool2);

    for (const auto& ref : refs) {
        const ReferenceOutcome outcome = followReference(after, ref);
        const int candidates = outcome == ReferenceOutcome::Broken
            ? countCandidates(after, ref.print)
            : -1;
        result.record(outcome, ref, candidates);
    }
    result.report();

    EXPECT_EQ(result.wrong, 0) << "a reference silently resolved to the wrong face";
}
