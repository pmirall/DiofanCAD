// SPDX-License-Identifier: LGPL-2.1-or-later
//
// DiofanCAD round 003 — can a descriptor-based repair be trusted on geometry
// where features resemble each other?
//
// Round 002 measured that broken references have exactly one matching
// candidate, but its matcher and its ground truth shared a predicate, and the
// geometry was axis-aligned boxes. This file separates the two and uses
// geometry designed to be ambiguous.
//
//   GROUND TRUTH  absolute coordinates, derived by hand per scenario.
//                 Never given to the matcher.
//   MATCHER       generic and scenario-blind: surface type, axis, area,
//                 bounding-box-relative position, radius. It ranks every face
//                 of the changed shape and reports a top-1/top-2 margin. It
//                 cannot know which answer is wanted.
//
// The metric that matters is the MIS-REPAIR rate. A repair that confidently
// points at the wrong face is worse than the honest breakage the baseline
// gives today.

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <BRepAdaptor_Surface.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <GProp_GProps.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <gp_Ax2.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
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

// ---------------------------------------------------------------------------
// Geometry extraction, shared by both sides. What each side is allowed to USE
// is what keeps them independent: ground truth reads absolute coordinates,
// the matcher reads only the normalised descriptor built from this.
// ---------------------------------------------------------------------------

struct FaceGeometry
{
    GeomAbs_SurfaceType surface {GeomAbs_OtherSurface};
    Base::Vector3d center;   //< absolute
    Base::Vector3d axis;     //< plane normal, or cylinder axis
    double area {0.0};
    double radius {0.0};     //< cylinders only
};

FaceGeometry geometryOf(const TopoDS_Shape& shape)
{
    FaceGeometry geom;
    GProp_GProps props;
    BRepGProp::SurfaceProperties(shape, props);
    geom.area = props.Mass();
    const gp_Pnt centre = props.CentreOfMass();
    geom.center = Base::Vector3d(centre.X(), centre.Y(), centre.Z());

    const TopoDS_Face face = TopoDS::Face(shape);
    BRepAdaptor_Surface surface(face);
    geom.surface = surface.GetType();
    if (geom.surface == GeomAbs_Plane) {
        gp_Dir direction = surface.Plane().Axis().Direction();
        if (face.Orientation() == TopAbs_REVERSED) {
            direction.Reverse();
        }
        geom.axis = Base::Vector3d(direction.X(), direction.Y(), direction.Z());
    }
    else if (geom.surface == GeomAbs_Cylinder) {
        const gp_Cylinder cylinder = surface.Cylinder();
        const gp_Dir direction = cylinder.Axis().Direction();
        geom.axis = Base::Vector3d(direction.X(), direction.Y(), direction.Z());
        geom.radius = cylinder.Radius();
    }
    return geom;
}

// ---------------------------------------------------------------------------
// The matcher. Scenario-blind by construction: its only inputs are a stored
// descriptor and a shape.
// ---------------------------------------------------------------------------

struct Descriptor
{
    GeomAbs_SurfaceType surface {GeomAbs_OtherSurface};
    Base::Vector3d axis;
    double area {0.0};
    double radius {0.0};
    Base::Vector3d relativeCenter;  //< position within the bounding box, 0..1
};

Base::Vector3d relativePosition(const Base::Vector3d& point, const Base::BoundBox3d& box)
{
    const auto safe = [](double value, double low, double high) {
        const double span = high - low;
        return span > 1e-12 ? (value - low) / span : 0.5;
    };
    return Base::Vector3d(safe(point.x, box.MinX, box.MaxX),
                          safe(point.y, box.MinY, box.MaxY),
                          safe(point.z, box.MinZ, box.MaxZ));
}

Descriptor describe(const FaceGeometry& geom, const Base::BoundBox3d& box)
{
    Descriptor descriptor;
    descriptor.surface = geom.surface;
    descriptor.axis = geom.axis;
    descriptor.area = geom.area;
    descriptor.radius = geom.radius;
    descriptor.relativeCenter = relativePosition(geom.center, box);
    return descriptor;
}

double ratioScore(double lhs, double rhs)
{
    const double high = std::max(std::abs(lhs), std::abs(rhs));
    if (high < 1e-12) {
        return 1.0;
    }
    return std::min(std::abs(lhs), std::abs(rhs)) / high;
}

/// 0 = no match, 1 = perfect. Rejects outright on surface type or axis
/// direction, then scores position, area and radius.
double similarity(const Descriptor& stored, const Descriptor& candidate)
{
    if (stored.surface != candidate.surface) {
        return 0.0;
    }
    const double axisAlignment = stored.axis * candidate.axis;
    if (axisAlignment < 0.99) {
        return 0.0;
    }
    constexpr double maxDistance = 1.7320508;  // sqrt(3), the unit cube diagonal
    const double distance = (stored.relativeCenter - candidate.relativeCenter).Length();
    const double positionScore = 1.0 - std::min(distance / maxDistance, 1.0);
    const double areaScore = ratioScore(stored.area, candidate.area);
    const double radiusScore =
        stored.surface == GeomAbs_Cylinder ? ratioScore(stored.radius, candidate.radius) : 1.0;

    return 0.55 * positionScore + 0.25 * areaScore + 0.20 * radiusScore;
}

struct Proposal
{
    int faceIndex {0};
    double score {0.0};
    double margin {0.0};  //< gap to the runner-up; thin means ambiguous
    bool any {false};
};

/// Rank every face of `shape` against `stored`. Knows nothing else.
Proposal propose(const TopoShape& shape, const Descriptor& stored)
{
    const Base::BoundBox3d box = shape.getBoundBox();
    const auto faceCount = shape.countSubShapes(TopAbs_FACE);

    double bestScore = 0.0;
    double secondScore = 0.0;
    int bestIndex = 0;
    for (unsigned long i = 1; i <= faceCount; ++i) {
        const auto index = static_cast<int>(i);
        const double score =
            similarity(stored, describe(geometryOf(shape.getSubShape(TopAbs_FACE, index)), box));
        if (score > bestScore) {
            secondScore = bestScore;
            bestScore = score;
            bestIndex = index;
        }
        else if (score > secondScore) {
            secondScore = score;
        }
    }

    Proposal proposal;
    proposal.any = bestScore > 0.0;
    proposal.faceIndex = bestIndex;
    proposal.score = bestScore;
    proposal.margin = bestScore - secondScore;
    return proposal;
}

/// A repair is only offered when the winner is clearly ahead. Abstaining is the
/// safe outcome; proposing under ambiguity is the failure this round hunts for.
constexpr double minimumScore = 0.50;
constexpr double minimumMargin = 0.05;

bool confidentEnough(const Proposal& proposal)
{
    return proposal.any && proposal.score >= minimumScore && proposal.margin >= minimumMargin;
}

// ---------------------------------------------------------------------------
// References and outcomes
// ---------------------------------------------------------------------------

/// Ground truth: an absolute-coordinate predicate written by hand for each
/// scenario. The matcher never receives it.
using GroundTruth = std::function<bool(const FaceGeometry&)>;

struct TrackedReference
{
    std::string label;
    Data::MappedName mapped;
    Descriptor descriptor;
    GroundTruth intendedAfterChange;
};

struct Tally
{
    std::string scenario;
    int tracked {0};
    int resolvedCorrect {0};
    int resolvedWrong {0};  //< baseline silent corruption
    int repairCorrect {0};
    int repairWrong {0};    //< THE metric: confident and wrong
    int abstained {0};

    void report() const
    {
        std::cout << "scenario=" << scenario << " tracked=" << tracked
                  << " resolved_correct=" << resolvedCorrect
                  << " resolved_WRONG=" << resolvedWrong << " repair_correct=" << repairCorrect
                  << " repair_WRONG=" << repairWrong << " abstained=" << abstained << std::endl;
    }
};

/// Find the single face satisfying an absolute predicate. Returns 0 if the
/// predicate matches no face, or more than one (which would make the scenario's
/// own ground truth ambiguous and must be fixed in the scenario, not here).
int groundTruthFace(const TopoShape& shape, const GroundTruth& predicate)
{
    const auto faceCount = shape.countSubShapes(TopAbs_FACE);
    int found = 0;
    int matches = 0;
    for (unsigned long i = 1; i <= faceCount; ++i) {
        const auto index = static_cast<int>(i);
        if (predicate(geometryOf(shape.getSubShape(TopAbs_FACE, index)))) {
            ++matches;
            found = index;
        }
    }
    return matches == 1 ? found : 0;
}

void evaluate(Tally& tally, const TopoShape& after, const TrackedReference& ref)
{
    ++tally.tracked;
    const int intended = groundTruthFace(after, ref.intendedAfterChange);
    if (intended == 0) {
        std::cout << "  " << tally.scenario << ": " << ref.label
                  << " -> SCENARIO ERROR, ground truth is not unique" << std::endl;
        return;
    }

    const Data::IndexedName resolved = after.getIndexedName(ref.mapped);
    if (resolved) {
        if (resolved.getIndex() == intended) {
            ++tally.resolvedCorrect;
        }
        else {
            ++tally.resolvedWrong;
            std::cout << "  " << tally.scenario << ": " << ref.label
                      << " -> BASELINE RESOLVED WRONG (got Face" << resolved.getIndex()
                      << ", intended Face" << intended << ")" << std::endl;
        }
        return;
    }

    const Proposal proposal = propose(after, ref.descriptor);
    if (!confidentEnough(proposal)) {
        ++tally.abstained;
        std::cout << "  " << tally.scenario << ": " << ref.label << " -> broken, ABSTAINED"
                  << " (score=" << proposal.score << " margin=" << proposal.margin << ")"
                  << std::endl;
        return;
    }
    if (proposal.faceIndex == intended) {
        ++tally.repairCorrect;
        std::cout << "  " << tally.scenario << ": " << ref.label << " -> broken, repaired to Face"
                  << proposal.faceIndex << " (score=" << proposal.score
                  << " margin=" << proposal.margin << ")" << std::endl;
    }
    else {
        ++tally.repairWrong;
        std::cout << "  " << tally.scenario << ": " << ref.label
                  << " -> broken, REPAIRED WRONG to Face" << proposal.faceIndex << ", intended Face"
                  << intended << " (score=" << proposal.score << " margin=" << proposal.margin
                  << ")" << std::endl;
    }
}

// ---------------------------------------------------------------------------
// Shape construction
// ---------------------------------------------------------------------------

TopoShape box(double dx, double dy, double dz, const Base::Vector3d& at, long tag)
{
    TopoDS_Shape shape = BRepPrimAPI_MakeBox(dx, dy, dz).Shape();
    if (at.Length() > 0.0) {
        gp_Trsf transform;
        transform.SetTranslation(gp_Vec(at.x, at.y, at.z));
        shape.Move(TopLoc_Location(transform));
    }
    return TopoShape {shape, tag};
}

TopoShape cylinder(double radius, double height, const Base::Vector3d& at, long tag)
{
    const gp_Ax2 axis(gp_Pnt(at.x, at.y, at.z), gp_Dir(0, 0, 1));
    return TopoShape {BRepPrimAPI_MakeCylinder(axis, radius, height).Shape(), tag};
}

TopoShape cut(const TopoShape& base, const std::vector<TopoShape>& tools)
{
    TopoShape result {base};
    std::vector<TopoShape> operands {base};
    operands.insert(operands.end(), tools.begin(), tools.end());
    result.makeElementBoolean(Part::OpCodes::Cut, operands);
    return result;
}

/// Capture what both the baseline and a repair layer would store about one face.
TrackedReference track(const TopoShape& shape,
                       int faceIndex,
                       std::string label,
                       GroundTruth intendedAfterChange)
{
    TrackedReference ref;
    ref.label = std::move(label);
    ref.mapped = shape.getMappedName(Data::IndexedName("Face", faceIndex), false);
    ref.descriptor =
        describe(geometryOf(shape.getSubShape(TopAbs_FACE, faceIndex)), shape.getBoundBox());
    ref.intendedAfterChange = std::move(intendedAfterChange);
    return ref;
}

bool near(double value, double target, double tolerance = 1e-6)
{
    return std::abs(value - target) < tolerance;
}

}  // namespace

class ReferenceRepairTest: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
    }
};

// H0 (control): the same construction twice. Nothing may break or move.
TEST_F(ReferenceRepairTest, H0_Control)  // NOLINT
{
    const auto build = []() {
        return cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                   {box(4, 4, 20, Base::Vector3d(3, 3, -5), 2L)});
    };
    const TopoShape before = build();
    const auto wall = [](const FaceGeometry& geom) {
        return geom.surface == GeomAbs_Plane && std::abs(geom.axis.x) > 0.9
            && near(geom.center.x, 7);
    };
    const TrackedReference ref = track(before, groundTruthFace(before, wall), "pocket +X wall", wall);
    ASSERT_TRUE(ref.mapped) << "no stable name captured";

    Tally tally;
    tally.scenario = "H0-control";
    evaluate(tally, build(), ref);
    tally.report();

    EXPECT_EQ(tally.resolvedCorrect, 1);
    EXPECT_EQ(tally.resolvedWrong, 0);
    EXPECT_EQ(tally.repairWrong, 0);
}

// H1: two similar pockets. One is widened and the block is lengthened, so every
// relative position shifts while pocket A itself is untouched.
TEST_F(ReferenceRepairTest, H1_TwoSimilarPockets)  // NOLINT
{
    const TopoShape before = cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                 {box(4, 4, 20, Base::Vector3d(3, 3, -5), 2L),
                                  box(4, 4, 20, Base::Vector3d(13, 3, -5), 3L)});

    // Pocket A is untouched by the change, so its ground truth is the same
    // predicate before and after.
    const auto pocketAFarWall = [](const FaceGeometry& geom) {
        return geom.surface == GeomAbs_Plane && std::abs(geom.axis.x) > 0.9
            && near(geom.center.x, 7);
    };
    const auto pocketANearWall = [](const FaceGeometry& geom) {
        return geom.surface == GeomAbs_Plane && std::abs(geom.axis.x) > 0.9
            && near(geom.center.x, 3);
    };
    // Pocket B's far wall moves from x=17 to x=18 when the pocket is widened.
    const auto pocketBFarWallBefore = [](const FaceGeometry& geom) {
        return geom.surface == GeomAbs_Plane && std::abs(geom.axis.x) > 0.9
            && near(geom.center.x, 17);
    };
    const auto pocketBFarWallAfter = [](const FaceGeometry& geom) {
        return geom.surface == GeomAbs_Plane && std::abs(geom.axis.x) > 0.9
            && near(geom.center.x, 18);
    };

    std::vector<TrackedReference> refs;
    refs.push_back(track(before, groundTruthFace(before, pocketAFarWall), "pocketA far wall",
                         pocketAFarWall));
    refs.push_back(track(before, groundTruthFace(before, pocketANearWall), "pocketA near wall",
                         pocketANearWall));
    refs.push_back(track(before, groundTruthFace(before, pocketBFarWallBefore), "pocketB far wall",
                         pocketBFarWallAfter));
    for (const auto& ref : refs) {
        ASSERT_TRUE(ref.mapped) << ref.label << ": no stable name captured";
    }

    // Widen pocket B and lengthen the block. Pocket A does not move.
    const TopoShape after = cut(box(24, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                {box(4, 4, 20, Base::Vector3d(3, 3, -5), 2L),
                                 box(6, 6, 20, Base::Vector3d(12, 2, -5), 3L)});

    Tally tally;
    tally.scenario = "H1-two-similar-pockets";
    for (const auto& ref : refs) {
        evaluate(tally, after, ref);
    }
    tally.report();

    EXPECT_EQ(tally.resolvedWrong, 0) << "baseline silently resolved to the wrong face";
}

// H2: four identical holes. The block is lengthened and one hole is deleted.
//
// This scenario is built to be a trap. The stored reference is the hole at
// x=8 in a 20-long block, whose relative position is 0.40. After the change the
// block is 30 long, so the hole at x=8 sits at 0.27 while the hole at x=12
// sits at exactly 0.40. A position-normalised matcher should be pulled towards
// the wrong hole, and the two are otherwise identical in radius and area.
TEST_F(ReferenceRepairTest, H2_IdenticalHolePattern)  // NOLINT
{
    const TopoShape before = cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                 {cylinder(1.5, 20, Base::Vector3d(4, 5, -5), 2L),
                                  cylinder(1.5, 20, Base::Vector3d(8, 5, -5), 3L),
                                  cylinder(1.5, 20, Base::Vector3d(12, 5, -5), 4L),
                                  cylinder(1.5, 20, Base::Vector3d(16, 5, -5), 5L)});

    const auto holeAt = [](double x) {
        return [x](const FaceGeometry& geom) {
            return geom.surface == GeomAbs_Cylinder && near(geom.center.x, x, 1e-3);
        };
    };

    std::vector<TrackedReference> refs;
    refs.push_back(track(before, groundTruthFace(before, holeAt(8)), "hole@x=8", holeAt(8)));
    refs.push_back(track(before, groundTruthFace(before, holeAt(12)), "hole@x=12", holeAt(12)));
    for (const auto& ref : refs) {
        ASSERT_TRUE(ref.mapped) << ref.label << ": no stable name captured";
    }

    // Lengthen the block and delete the first hole.
    const TopoShape after = cut(box(30, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                {cylinder(1.5, 20, Base::Vector3d(8, 5, -5), 3L),
                                 cylinder(1.5, 20, Base::Vector3d(12, 5, -5), 4L),
                                 cylinder(1.5, 20, Base::Vector3d(16, 5, -5), 5L)});

    Tally tally;
    tally.scenario = "H2-identical-hole-pattern";
    for (const auto& ref : refs) {
        evaluate(tally, after, ref);
    }
    tally.report();

    EXPECT_EQ(tally.resolvedWrong, 0) << "baseline silently resolved to the wrong face";
}

// H3: a fillet, so the tracked face is cylindrical and has no normal, only an
// axis. The radius changes, then the base is resized.
TEST_F(ReferenceRepairTest, H3_FilletRadiusChanged)  // NOLINT
{
    const auto filleted = [](double dx, double radius) {
        const TopoShape base = box(dx, 10, 10, Base::Vector3d(0, 0, 0), 1L);
        const TopoShape edge = base.getSubTopoShape(TopAbs_EDGE, 1);
        return base.makeElementFillet({edge}, radius, radius);
    };

    const TopoShape before = filleted(10, 2.0);
    const auto onlyCylinder = [](const FaceGeometry& geom) {
        return geom.surface == GeomAbs_Cylinder;
    };
    const TrackedReference ref =
        track(before, groundTruthFace(before, onlyCylinder), "fillet face", onlyCylinder);
    ASSERT_TRUE(ref.mapped) << "no stable name captured for the fillet face";

    Tally tally;
    tally.scenario = "H3-fillet-radius-changed";
    evaluate(tally, filleted(10, 3.0), ref);
    tally.report();

    Tally resized;
    resized.scenario = "H3b-fillet-base-resized";
    evaluate(resized, filleted(16, 2.0), ref);
    resized.report();

    EXPECT_EQ(tally.resolvedWrong, 0) << "baseline silently resolved to the wrong face";
    EXPECT_EQ(resized.resolvedWrong, 0) << "baseline silently resolved to the wrong face";
}

// NEGATIVE CONTROL: proves the harness can detect a wrong resolution at all.
// A measurement that never fires is indistinguishable from a broken one, so
// this deliberately points the ground truth at the wrong face and requires the
// harness to say so.
TEST_F(ReferenceRepairTest, N1_HarnessDetectsWrongResolution)  // NOLINT
{
    const auto build = []() {
        return cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                   {box(4, 4, 20, Base::Vector3d(3, 3, -5), 2L)});
    };
    const TopoShape before = build();
    const auto farWall = [](const FaceGeometry& geom) {
        return geom.surface == GeomAbs_Plane && std::abs(geom.axis.x) > 0.9
            && near(geom.center.x, 7);
    };
    const auto nearWall = [](const FaceGeometry& geom) {
        return geom.surface == GeomAbs_Plane && std::abs(geom.axis.x) > 0.9
            && near(geom.center.x, 3);
    };
    // Track the far wall but declare the near wall to be the intended target.
    TrackedReference ref =
        track(before, groundTruthFace(before, farWall), "far wall, mislabelled", nearWall);

    Tally tally;
    tally.scenario = "N1-negative-control";
    evaluate(tally, build(), ref);
    tally.report();

    EXPECT_EQ(tally.resolvedWrong, 1) << "the harness cannot detect a wrong resolution, so every "
                                         "'0 wrong' result in this file is meaningless";
    EXPECT_EQ(tally.resolvedCorrect, 0);
}

// H4: the trap that requires the reference to actually break.
//
// A user deletes a hole and re-creates it in the same place: same geometry,
// new generator. The stored reference loses its anchor, so repair is the only
// route left. Meanwhile the block has been lengthened, which moves the hole at
// x=12 into exactly the relative position the stored reference remembers.
//
// The correct repair is the hole at x=8. The seductive one is the hole at x=12.
TEST_F(ReferenceRepairTest, H4_HoleRecreatedAfterResize)  // NOLINT
{
    const TopoShape before = cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                 {cylinder(1.5, 20, Base::Vector3d(4, 5, -5), 2L),
                                  cylinder(1.5, 20, Base::Vector3d(8, 5, -5), 3L),
                                  cylinder(1.5, 20, Base::Vector3d(12, 5, -5), 4L),
                                  cylinder(1.5, 20, Base::Vector3d(16, 5, -5), 5L)});

    const auto holeAt = [](double x) {
        return [x](const FaceGeometry& geom) {
            return geom.surface == GeomAbs_Cylinder && near(geom.center.x, x, 1e-3);
        };
    };
    const TrackedReference ref =
        track(before, groundTruthFace(before, holeAt(8)), "hole@x=8 re-created", holeAt(8));
    ASSERT_TRUE(ref.mapped);

    // Same four holes in the same places, but the one at x=8 carries a new tag
    // (it was deleted and re-created), and the block is longer.
    const TopoShape after = cut(box(30, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                {cylinder(1.5, 20, Base::Vector3d(4, 5, -5), 2L),
                                 cylinder(1.5, 20, Base::Vector3d(8, 5, -5), 9L),
                                 cylinder(1.5, 20, Base::Vector3d(12, 5, -5), 4L),
                                 cylinder(1.5, 20, Base::Vector3d(16, 5, -5), 5L)});

    // Report what the matcher sees, whatever the verdict.
    const Proposal proposal = propose(after, ref.descriptor);
    const int intendedFace = groundTruthFace(after, holeAt(8));
    const auto centreOf = [&after](int index) {
        return geometryOf(after.getSubShape(TopAbs_FACE, index)).center.x;
    };
    std::cout << "  H4 matcher: top face=Face" << proposal.faceIndex << " (x="
              << centreOf(proposal.faceIndex) << ") score=" << proposal.score
              << " margin=" << proposal.margin << " confident=" << confidentEnough(proposal)
              << " | intended=Face" << intendedFace << " (x=" << centreOf(intendedFace) << ")"
              << std::endl;

    Tally tally;
    tally.scenario = "H4-hole-recreated-after-resize";
    evaluate(tally, after, ref);
    tally.report();

    EXPECT_EQ(tally.resolvedWrong, 0) << "baseline silently resolved to the wrong face";
}
