// SPDX-License-Identifier: LGPL-2.1-or-later
//
// DiofanCAD round 004 — does provenance disambiguate what geometry cannot?
//
// Round 003 showed a geometric matcher scoring 1.0 on the WRONG feature after
// a resize. This round compares three strategies on the same breaks:
//
//   G    geometry only (round 003's matcher, unchanged)
//   G+E  geometry, minus candidates already claimed by a surviving reference
//   G+P  geometry, filtered by the element's originating tag
//
// Ground truth remains absolute-coordinate predicates written per scenario,
// independent of every strategy.

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <set>
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

struct FaceGeometry
{
    GeomAbs_SurfaceType surface {GeomAbs_OtherSurface};
    Base::Vector3d center;
    Base::Vector3d axis;
    double area {0.0};
    double radius {0.0};
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

struct Descriptor
{
    GeomAbs_SurfaceType surface {GeomAbs_OtherSurface};
    Base::Vector3d axis;
    double area {0.0};
    double radius {0.0};
    Base::Vector3d relativeCenter;
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
    return high < 1e-12 ? 1.0 : std::min(std::abs(lhs), std::abs(rhs)) / high;
}

double similarity(const Descriptor& stored, const Descriptor& candidate)
{
    if (stored.surface != candidate.surface) {
        return 0.0;
    }
    if (stored.axis * candidate.axis < 0.99) {
        return 0.0;
    }
    constexpr double maxDistance = 1.7320508;
    const double distance = (stored.relativeCenter - candidate.relativeCenter).Length();
    const double positionScore = 1.0 - std::min(distance / maxDistance, 1.0);
    const double areaScore = ratioScore(stored.area, candidate.area);
    const double radiusScore =
        stored.surface == GeomAbs_Cylinder ? ratioScore(stored.radius, candidate.radius) : 1.0;
    return 0.55 * positionScore + 0.25 * areaScore + 0.20 * radiusScore;
}

/// The originating tag of a face, as the baseline already records it.
long provenanceTagOf(const TopoShape& shape, int faceIndex)
{
    const Data::MappedName mapped =
        shape.getMappedName(Data::IndexedName("Face", faceIndex), false);
    if (!mapped) {
        return 0;
    }
    return shape.getElementHistory(mapped, nullptr, nullptr);
}

struct Proposal
{
    int faceIndex {0};
    double score {0.0};
    double margin {0.0};
    bool any {false};
};

constexpr double minimumScore = 0.50;
constexpr double minimumMargin = 0.05;

bool confidentEnough(const Proposal& proposal)
{
    return proposal.any && proposal.score >= minimumScore && proposal.margin >= minimumMargin;
}

/// Rank faces of `shape` against `stored`, skipping any index in `excluded` and
/// — when `requiredTag` is non-zero — any face with a different originating tag.
Proposal propose(const TopoShape& shape,
                 const Descriptor& stored,
                 const std::set<int>& excluded,
                 long requiredTag)
{
    const Base::BoundBox3d box = shape.getBoundBox();
    const auto faceCount = shape.countSubShapes(TopAbs_FACE);

    double bestScore = 0.0;
    double secondScore = 0.0;
    int bestIndex = 0;
    for (unsigned long i = 1; i <= faceCount; ++i) {
        const auto index = static_cast<int>(i);
        if (excluded.count(index) != 0) {
            continue;
        }
        if (requiredTag != 0 && provenanceTagOf(shape, index) != requiredTag) {
            continue;
        }
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

using GroundTruth = std::function<bool(const FaceGeometry&)>;

struct TrackedReference
{
    std::string label;
    Data::MappedName mapped;
    Descriptor descriptor;
    long originTag {0};
    GroundTruth intendedAfterChange;
};

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
    ref.originTag = provenanceTagOf(shape, faceIndex);
    ref.intendedAfterChange = std::move(intendedAfterChange);
    return ref;
}

enum class Strategy
{
    Geometry,
    GeometryPlusExclusion,
    GeometryPlusProvenance
};

const char* strategyName(Strategy strategy)
{
    switch (strategy) {
        case Strategy::Geometry:
            return "G   ";
        case Strategy::GeometryPlusExclusion:
            return "G+E ";
        case Strategy::GeometryPlusProvenance:
        default:
            return "G+P ";
    }
}

struct StrategyTally
{
    int repairCorrect {0};
    int repairWrong {0};
    int abstained {0};
};

struct ScenarioResult
{
    std::string scenario;
    int tracked {0};
    int resolvedCorrect {0};
    int resolvedWrong {0};
    int broken {0};
    StrategyTally geometry;
    StrategyTally exclusion;
    StrategyTally provenance;

    StrategyTally& tallyFor(Strategy strategy)
    {
        switch (strategy) {
            case Strategy::Geometry:
                return geometry;
            case Strategy::GeometryPlusExclusion:
                return exclusion;
            case Strategy::GeometryPlusProvenance:
            default:
                return provenance;
        }
    }

    void report() const
    {
        std::cout << "scenario=" << scenario << " tracked=" << tracked
                  << " resolved_correct=" << resolvedCorrect << " resolved_WRONG=" << resolvedWrong
                  << " broken=" << broken << std::endl;
        const auto line = [this](const char* name, const StrategyTally& tally) {
            std::cout << "    " << name << " repair_correct=" << tally.repairCorrect
                      << " repair_WRONG=" << tally.repairWrong << " abstained=" << tally.abstained
                      << std::endl;
        };
        line("G  ", geometry);
        line("G+E", exclusion);
        line("G+P", provenance);
    }
};

/// Faces already claimed by a reference that still resolves. This is
/// information the document itself holds: no privileged knowledge.
std::set<int> claimedFaces(const TopoShape& after,
                           const std::vector<TrackedReference>& all,
                           const TrackedReference& current)
{
    std::set<int> claimed;
    for (const auto& ref : all) {
        if (&ref == &current) {
            continue;
        }
        const Data::IndexedName resolved = after.getIndexedName(ref.mapped);
        if (resolved) {
            claimed.insert(resolved.getIndex());
        }
    }
    return claimed;
}

void evaluateScenario(ScenarioResult& result,
                      const TopoShape& after,
                      const std::vector<TrackedReference>& refs)
{
    for (const auto& ref : refs) {
        ++result.tracked;
        const int intended = groundTruthFace(after, ref.intendedAfterChange);
        if (intended == 0) {
            std::cout << "  " << result.scenario << ": " << ref.label
                      << " -> SCENARIO ERROR, ground truth is not unique" << std::endl;
            continue;
        }

        const Data::IndexedName resolved = after.getIndexedName(ref.mapped);
        if (resolved) {
            if (resolved.getIndex() == intended) {
                ++result.resolvedCorrect;
            }
            else {
                ++result.resolvedWrong;
                std::cout << "  " << result.scenario << ": " << ref.label
                          << " -> BASELINE RESOLVED WRONG" << std::endl;
            }
            continue;
        }

        ++result.broken;
        const std::set<int> claimed = claimedFaces(after, refs, ref);
        for (const Strategy strategy : {Strategy::Geometry,
                                        Strategy::GeometryPlusExclusion,
                                        Strategy::GeometryPlusProvenance}) {
            const std::set<int> excluded =
                strategy == Strategy::GeometryPlusExclusion ? claimed : std::set<int> {};
            const long requiredTag =
                strategy == Strategy::GeometryPlusProvenance ? ref.originTag : 0;
            const Proposal proposal = propose(after, ref.descriptor, excluded, requiredTag);

            StrategyTally& tally = result.tallyFor(strategy);
            if (!confidentEnough(proposal)) {
                ++tally.abstained;
            }
            else if (proposal.faceIndex == intended) {
                ++tally.repairCorrect;
            }
            else {
                ++tally.repairWrong;
            }
            std::cout << "  " << result.scenario << " " << strategyName(strategy) << ref.label
                      << " -> top=Face" << proposal.faceIndex << " score=" << proposal.score
                      << " margin=" << proposal.margin
                      << (confidentEnough(proposal)
                              ? (proposal.faceIndex == intended ? " CORRECT" : " WRONG")
                              : " abstain")
                      << " (intended Face" << intended << ")" << std::endl;
        }
    }
}

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

bool near(double value, double target, double tolerance = 1e-6)
{
    return std::abs(value - target) < tolerance;
}

auto holeAt(double x)
{
    return [x](const FaceGeometry& geom) {
        return geom.surface == GeomAbs_Cylinder && near(geom.center.x, x, 1e-3);
    };
}

}  // namespace

class ReferenceProvenanceTest: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
    }
};

// R1: one hole deleted and re-created, block lengthened. Round 003's H4, now
// with the other holes' references present so exclusion has something to work
// with.
TEST_F(ReferenceProvenanceTest, R1_OneHoleRecreated)  // NOLINT
{
    const TopoShape before = cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                 {cylinder(1.5, 20, Base::Vector3d(4, 5, -5), 2L),
                                  cylinder(1.5, 20, Base::Vector3d(8, 5, -5), 3L),
                                  cylinder(1.5, 20, Base::Vector3d(12, 5, -5), 4L),
                                  cylinder(1.5, 20, Base::Vector3d(16, 5, -5), 5L)});

    std::vector<TrackedReference> refs;
    for (const double x : {4.0, 8.0, 12.0, 16.0}) {
        refs.push_back(track(before, groundTruthFace(before, holeAt(x)),
                             "hole@x=" + std::to_string(static_cast<int>(x)), holeAt(x)));
        ASSERT_TRUE(refs.back().mapped);
    }

    // The hole at x=8 is deleted and re-created under a new tag; block lengthened.
    const TopoShape after = cut(box(30, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                {cylinder(1.5, 20, Base::Vector3d(4, 5, -5), 2L),
                                 cylinder(1.5, 20, Base::Vector3d(8, 5, -5), 9L),
                                 cylinder(1.5, 20, Base::Vector3d(12, 5, -5), 4L),
                                 cylinder(1.5, 20, Base::Vector3d(16, 5, -5), 5L)});

    ScenarioResult result;
    result.scenario = "R1-one-hole-recreated";
    evaluateScenario(result, after, refs);
    result.report();

    EXPECT_EQ(result.resolvedWrong, 0);
    EXPECT_EQ(result.geometry.repairWrong + result.exclusion.repairWrong
                  + result.provenance.repairWrong,
              0)
        << "a strategy repaired confidently to the wrong face";
}

// R2: the honest limit. TWO holes are re-created, so exclusion has fewer
// surviving claimants to work with.
TEST_F(ReferenceProvenanceTest, R2_TwoHolesRecreated)  // NOLINT
{
    const TopoShape before = cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                 {cylinder(1.5, 20, Base::Vector3d(4, 5, -5), 2L),
                                  cylinder(1.5, 20, Base::Vector3d(8, 5, -5), 3L),
                                  cylinder(1.5, 20, Base::Vector3d(12, 5, -5), 4L),
                                  cylinder(1.5, 20, Base::Vector3d(16, 5, -5), 5L)});

    std::vector<TrackedReference> refs;
    for (const double x : {4.0, 8.0, 12.0, 16.0}) {
        refs.push_back(track(before, groundTruthFace(before, holeAt(x)),
                             "hole@x=" + std::to_string(static_cast<int>(x)), holeAt(x)));
        ASSERT_TRUE(refs.back().mapped);
    }

    const TopoShape after = cut(box(30, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                {cylinder(1.5, 20, Base::Vector3d(4, 5, -5), 2L),
                                 cylinder(1.5, 20, Base::Vector3d(8, 5, -5), 9L),
                                 cylinder(1.5, 20, Base::Vector3d(12, 5, -5), 10L),
                                 cylinder(1.5, 20, Base::Vector3d(16, 5, -5), 5L)});

    ScenarioResult result;
    result.scenario = "R2-two-holes-recreated";
    evaluateScenario(result, after, refs);
    result.report();

    EXPECT_EQ(result.resolvedWrong, 0);
    EXPECT_EQ(result.geometry.repairWrong + result.exclusion.repairWrong
                  + result.provenance.repairWrong,
              0)
        << "a strategy repaired confidently to the wrong face";
}

// R3: a feature inserted upstream modifies outer faces whose generator is
// otherwise unchanged.
TEST_F(ReferenceProvenanceTest, R3_UpstreamFeatureInserted)  // NOLINT
{
    const TopoShape before = cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                 {box(4, 4, 20, Base::Vector3d(3, 3, -5), 2L)});

    const auto outerFaceAt = [](double x) {
        return [x](const FaceGeometry& geom) {
            return geom.surface == GeomAbs_Plane && std::abs(geom.axis.x) > 0.9
                && near(geom.center.x, x);
        };
    };
    std::vector<TrackedReference> refs;
    refs.push_back(track(before, groundTruthFace(before, outerFaceAt(0)), "outer -X face",
                         outerFaceAt(0)));
    refs.push_back(track(before, groundTruthFace(before, outerFaceAt(20)), "outer +X face",
                         outerFaceAt(20)));
    for (const auto& ref : refs) {
        ASSERT_TRUE(ref.mapped);
    }

    // A notch is now cut at the origin corner, modifying the -X face.
    const TopoShape notched =
        cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L), {box(2, 2, 2, Base::Vector3d(0, 0, 0), 7L)});
    const TopoShape after = cut(notched, {box(4, 4, 20, Base::Vector3d(3, 3, -5), 2L)});

    ScenarioResult result;
    result.scenario = "R3-upstream-feature-inserted";
    evaluateScenario(result, after, refs);
    result.report();

    EXPECT_EQ(result.resolvedWrong, 0);
    EXPECT_EQ(result.geometry.repairWrong + result.exclusion.repairWrong
                  + result.provenance.repairWrong,
              0)
        << "a strategy repaired confidently to the wrong face";
}

// R4: the cutting tool is replaced by a different solid, so every face it
// generated loses its origin.
TEST_F(ReferenceProvenanceTest, R4_ToolReplaced)  // NOLINT
{
    const TopoShape before = cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                 {box(4, 4, 20, Base::Vector3d(3, 3, -5), 2L)});

    const auto pocketWallAt = [](double x) {
        return [x](const FaceGeometry& geom) {
            return geom.surface == GeomAbs_Plane && std::abs(geom.axis.x) > 0.9
                && near(geom.center.x, x);
        };
    };
    std::vector<TrackedReference> refs;
    refs.push_back(track(before, groundTruthFace(before, pocketWallAt(3)), "pocket near wall",
                         pocketWallAt(2)));
    refs.push_back(track(before, groundTruthFace(before, pocketWallAt(7)), "pocket far wall",
                         pocketWallAt(8)));
    for (const auto& ref : refs) {
        ASSERT_TRUE(ref.mapped);
    }

    // Replaced by a wider pocket carrying a different tag.
    const TopoShape after = cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                {box(6, 6, 20, Base::Vector3d(2, 2, -5), 8L)});

    ScenarioResult result;
    result.scenario = "R4-tool-replaced";
    evaluateScenario(result, after, refs);
    result.report();

    EXPECT_EQ(result.resolvedWrong, 0);
    EXPECT_EQ(result.geometry.repairWrong + result.exclusion.repairWrong
                  + result.provenance.repairWrong,
              0)
        << "a strategy repaired confidently to the wrong face";
}

// R5: the lone-reference limit. Exactly the change from R1, but the document
// holds only ONE reference, so exclusion has nothing to exclude. This measures
// how much of G+E's advantage depends on the document being richly referenced.
TEST_F(ReferenceProvenanceTest, R5_LoneReference)  // NOLINT
{
    const TopoShape before = cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                 {cylinder(1.5, 20, Base::Vector3d(4, 5, -5), 2L),
                                  cylinder(1.5, 20, Base::Vector3d(8, 5, -5), 3L),
                                  cylinder(1.5, 20, Base::Vector3d(12, 5, -5), 4L),
                                  cylinder(1.5, 20, Base::Vector3d(16, 5, -5), 5L)});

    std::vector<TrackedReference> refs;
    refs.push_back(track(before, groundTruthFace(before, holeAt(8)), "hole@x=8 (alone)", holeAt(8)));
    ASSERT_TRUE(refs.back().mapped);

    const TopoShape after = cut(box(30, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                {cylinder(1.5, 20, Base::Vector3d(4, 5, -5), 2L),
                                 cylinder(1.5, 20, Base::Vector3d(8, 5, -5), 9L),
                                 cylinder(1.5, 20, Base::Vector3d(12, 5, -5), 4L),
                                 cylinder(1.5, 20, Base::Vector3d(16, 5, -5), 5L)});

    ScenarioResult result;
    result.scenario = "R5-lone-reference";
    evaluateScenario(result, after, refs);
    result.report();

    EXPECT_EQ(result.geometry.repairWrong + result.exclusion.repairWrong
                  + result.provenance.repairWrong,
              0)
        << "a strategy repaired confidently to the wrong face";
}

// NEGATIVE CONTROL, carried forward from round 003: a measurement that never
// fires is indistinguishable from a broken one.
TEST_F(ReferenceProvenanceTest, N2_HarnessDetectsWrongResolution)  // NOLINT
{
    const auto build = []() {
        return cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                   {box(4, 4, 20, Base::Vector3d(3, 3, -5), 2L)});
    };
    const auto wallAt = [](double x) {
        return [x](const FaceGeometry& geom) {
            return geom.surface == GeomAbs_Plane && std::abs(geom.axis.x) > 0.9
                && near(geom.center.x, x);
        };
    };
    const TopoShape before = build();
    std::vector<TrackedReference> refs;
    // Track the far wall, but declare the near wall to be the intended target.
    refs.push_back(
        track(before, groundTruthFace(before, wallAt(7)), "far wall, mislabelled", wallAt(3)));

    ScenarioResult result;
    result.scenario = "N2-negative-control";
    evaluateScenario(result, build(), refs);
    result.report();

    EXPECT_EQ(result.resolvedWrong, 1) << "the harness cannot detect a wrong resolution, so every "
                                          "zero in this round is meaningless";
}
