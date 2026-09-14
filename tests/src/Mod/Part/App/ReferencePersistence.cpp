// SPDX-License-Identifier: LGPL-2.1-or-later
//
// DiofanCAD round 005 — P0-D: does identity survive the persistence boundary?
//
// Rounds 002-004 measured reference survival on shapes that were never saved.
// A real model is always saved and reopened, so this round puts the element
// map through the actual save/restore path and repeats the measurements.
//
// The deep test is M3: applying the same change to a RELOADED shape must
// produce the same mapped names as applying it to a never-saved one. If a
// reload silently changes downstream naming, every reference captured before a
// save is at risk after one.

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <sstream>
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

#include <App/StringHasher.h>
#include <Base/Vector3D.h>
#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/App/TopoShapeOpCode.h>
#include <src/App/InitApplication.h>

using Part::TopoShape;

namespace
{

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

TopoShape cut(const TopoShape& base, const std::vector<TopoShape>& tools)
{
    TopoShape result {base};
    std::vector<TopoShape> operands {base};
    operands.insert(operands.end(), tools.begin(), tools.end());
    result.makeElementBoolean(Part::OpCodes::Cut, operands);
    return result;
}

/// Every Face index mapped to its stable name, as strings for comparison.
std::map<int, std::string> nameByFace(const TopoShape& shape)
{
    std::map<int, std::string> names;
    const auto faceCount = shape.countSubShapes(TopAbs_FACE);
    for (unsigned long i = 1; i <= faceCount; ++i) {
        const auto index = static_cast<int>(i);
        const Data::MappedName mapped =
            shape.getMappedName(Data::IndexedName("Face", index), false);
        if (mapped) {
            names[index] = mapped.toString();
        }
    }
    return names;
}

double faceCentreX(const TopoShape& shape, int index)
{
    GProp_GProps props;
    BRepGProp::SurfaceProperties(shape.getSubShape(TopAbs_FACE, index), props);
    return props.CentreOfMass().X();
}

/// Round-trip a shape's element map through the real save/restore path.
/// When `freshHasher` is true the map is restored with a hasher that never saw
/// the save, which is what happens if the string table is lost.
TopoShape roundTrip(const TopoShape& shape,
                    bool freshHasher,
                    bool* saved = nullptr,
                    std::size_t* hashedEntries = nullptr)
{
    // resetElementMap() is the public accessor: it detaches the map and hands
    // it back. Done on a copy so the caller's shape is untouched.
    TopoShape detachable {shape};
    const Data::ElementMapPtr original = detachable.resetElementMap();
    if (!original) {
        if (saved != nullptr) {
            *saved = false;
        }
        return shape;
    }

    App::StringHasherRef hasher(new App::StringHasher);
    original->beforeSave(hasher);
    std::ostringstream out;
    original->save(out);
    if (saved != nullptr) {
        *saved = true;
    }
    // If the hasher holds nothing, the save never used it, and a test that
    // restores with a fresh hasher has not exercised anything.
    if (hashedEntries != nullptr) {
        *hashedEntries = hasher->size();
    }

    App::StringHasherRef restoreHasher = freshHasher ? App::StringHasherRef(new App::StringHasher)
                                                     : hasher;
    std::istringstream in(out.str());
    auto fresh = std::make_shared<Data::ElementMap>();
    const Data::ElementMapPtr restored = fresh->restore(restoreHasher, in);

    TopoShape reloaded {shape};
    reloaded.resetElementMap(restored);
    return reloaded;
}

bool near(double value, double target, double tolerance = 1e-6)
{
    return std::abs(value - target) < tolerance;
}

}  // namespace

class ReferencePersistenceTest: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
    }

    static TopoShape pocketedBlock()
    {
        return cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                   {box(4, 4, 20, Base::Vector3d(3, 3, -5), 2L)});
    }
};

// M1: every mapped name must survive save -> restore unchanged.
TEST_F(ReferencePersistenceTest, M1_MapSurvivesRoundTrip)  // NOLINT
{
    const TopoShape before = pocketedBlock();
    const auto namesBefore = nameByFace(before);
    ASSERT_FALSE(namesBefore.empty()) << "no mapped names to begin with";

    bool saved = false;
    const TopoShape reloaded = roundTrip(before, false, &saved);
    ASSERT_TRUE(saved) << "the shape had no element map to save";
    const auto namesAfter = nameByFace(reloaded);

    std::cout << "M1 faces_before=" << namesBefore.size() << " faces_after=" << namesAfter.size()
              << std::endl;
    int identical = 0;
    int differing = 0;
    int missing = 0;
    for (const auto& [index, name] : namesBefore) {
        const auto found = namesAfter.find(index);
        if (found == namesAfter.end()) {
            ++missing;
            std::cout << "  M1 Face" << index << " lost its name" << std::endl;
        }
        else if (found->second == name) {
            ++identical;
        }
        else {
            ++differing;
            std::cout << "  M1 Face" << index << " changed name" << std::endl;
        }
    }
    std::cout << "M1 identical=" << identical << " differing=" << differing
              << " missing=" << missing << std::endl;

    EXPECT_EQ(differing, 0);
    EXPECT_EQ(missing, 0);
    EXPECT_EQ(identical, static_cast<int>(namesBefore.size()));
}

// M2: a reference stored before the save still resolves to the same face after.
TEST_F(ReferencePersistenceTest, M2_ReferenceResolvesAfterReload)  // NOLINT
{
    const TopoShape before = pocketedBlock();
    // The pocket's far wall, identified by absolute position.
    int target = 0;
    for (unsigned long i = 1; i <= before.countSubShapes(TopAbs_FACE); ++i) {
        if (near(faceCentreX(before, static_cast<int>(i)), 7)) {
            target = static_cast<int>(i);
        }
    }
    ASSERT_GT(target, 0);
    const Data::MappedName stored =
        before.getMappedName(Data::IndexedName("Face", target), false);
    ASSERT_TRUE(stored);

    const TopoShape reloaded = roundTrip(before, false);
    const Data::IndexedName resolved = reloaded.getIndexedName(stored);

    ASSERT_TRUE(resolved) << "the stored reference no longer resolves after a reload";
    std::cout << "M2 stored=Face" << target << " resolved=Face" << resolved.getIndex()
              << " centre_x=" << faceCentreX(reloaded, resolved.getIndex()) << std::endl;
    EXPECT_EQ(resolved.getIndex(), target);
    EXPECT_TRUE(near(faceCentreX(reloaded, resolved.getIndex()), 7));
}

// M3: the deep one. The same change applied to a reloaded shape must produce
// the same mapped names as applied to a never-saved one.
TEST_F(ReferencePersistenceTest, M3_DownstreamNamingIsDeterministicAfterReload)  // NOLINT
{
    const TopoShape neverSaved = pocketedBlock();
    const TopoShape reloaded = roundTrip(pocketedBlock(), false);

    const TopoShape changedFromFresh =
        cut(neverSaved, {box(3, 3, 20, Base::Vector3d(14, 4, -5), 6L)});
    const TopoShape changedFromReloaded =
        cut(reloaded, {box(3, 3, 20, Base::Vector3d(14, 4, -5), 6L)});

    const auto fresh = nameByFace(changedFromFresh);
    const auto viaReload = nameByFace(changedFromReloaded);

    std::cout << "M3 faces_fresh=" << fresh.size() << " faces_via_reload=" << viaReload.size()
              << std::endl;

    int identical = 0;
    int differing = 0;
    for (const auto& [index, name] : fresh) {
        const auto found = viaReload.find(index);
        if (found != viaReload.end() && found->second == name) {
            ++identical;
        }
        else {
            ++differing;
            std::cout << "  M3 Face" << index << " differs after a reload" << std::endl;
        }
    }
    std::cout << "M3 identical=" << identical << " differing=" << differing << std::endl;

    EXPECT_EQ(fresh.size(), viaReload.size());
    EXPECT_EQ(differing, 0) << "a reload changed downstream naming, so references captured "
                               "before a save are at risk after one";
}

// M4: restore with a hasher that never saw the save. The question is not
// whether information is lost - it is whether the loss is visible or silent.
TEST_F(ReferencePersistenceTest, M4_RestoreWithFreshHasher)  // NOLINT
{
    const TopoShape before = pocketedBlock();
    const auto namesBefore = nameByFace(before);

    std::vector<std::pair<Data::MappedName, double>> stored;
    for (const auto& [index, name] : namesBefore) {
        stored.emplace_back(before.getMappedName(Data::IndexedName("Face", index), false),
                            faceCentreX(before, index));
    }

    std::size_t hashedEntries = 0;
    const TopoShape reloaded = roundTrip(before, true, nullptr, &hashedEntries);
    const auto namesAfter = nameByFace(reloaded);
    std::cout << "M4 hasher_entries_at_save=" << hashedEntries << std::endl;

    int resolvedSame = 0;
    int resolvedElsewhere = 0;  //< silent: the dangerous outcome
    int didNotResolve = 0;      //< visible: the safe outcome
    for (const auto& [name, centreX] : stored) {
        const Data::IndexedName resolved = reloaded.getIndexedName(name);
        if (!resolved) {
            ++didNotResolve;
            continue;
        }
        if (near(faceCentreX(reloaded, resolved.getIndex()), centreX)) {
            ++resolvedSame;
        }
        else {
            ++resolvedElsewhere;
            std::cout << "  M4 a reference resolved to a DIFFERENT face after a partial restore"
                      << std::endl;
        }
    }

    std::cout << "M4 names_before=" << namesBefore.size() << " names_after=" << namesAfter.size()
              << " resolved_same=" << resolvedSame << " resolved_ELSEWHERE=" << resolvedElsewhere
              << " did_not_resolve=" << didNotResolve << std::endl;

    // If nothing was hashed, this scenario did not exercise the hasher at all,
    // and its clean result says nothing about a real string-table loss.
    if (hashedEntries == 0) {
        std::cout << "M4 NOT EXERCISED: the save stored no string IDs, so restoring with a "
                     "fresh hasher is indistinguishable from restoring with the real one"
                  << std::endl;
    }

    // The classification, not the loss, is what matters.
    EXPECT_EQ(resolvedElsewhere, 0)
        << "UNSAFE CHANGE: a partial restore makes references resolve to the wrong face silently";
}

// NEGATIVE CONTROL: the comparison must be able to report a difference.
TEST_F(ReferencePersistenceTest, N3_ComparisonDetectsDifference)  // NOLINT
{
    const TopoShape blockA = pocketedBlock();
    const TopoShape blockB = cut(box(20, 10, 10, Base::Vector3d(0, 0, 0), 1L),
                                 {box(4, 4, 20, Base::Vector3d(3, 3, -5), 99L)});

    const auto namesA = nameByFace(blockA);
    const auto namesB = nameByFace(blockB);

    int differing = 0;
    for (const auto& [index, name] : namesA) {
        const auto found = namesB.find(index);
        if (found == namesB.end() || found->second != name) {
            ++differing;
        }
    }
    std::cout << "N3 differing=" << differing << " (must be > 0)" << std::endl;
    EXPECT_GT(differing, 0) << "the name comparison cannot detect a difference, so every "
                               "'identical' result in this file is meaningless";
}

// M5: M4 again, but with the string table actually in use.
//
// M4 came back clean and meant nothing: the shapes carried no StringHasher, so
// the save stored no string IDs and a "fresh hasher" was indistinguishable from
// the real one. Here the shapes share a hasher and a chain of booleans grows
// the names until hashing kicks in, so the loss of a string table can be
// measured rather than assumed.
TEST_F(ReferencePersistenceTest, M5_RestoreWithFreshHasherWhenStringsAreHashed)  // NOLINT
{
    App::StringHasherRef hasher(new App::StringHasher);
    const auto hashedBox = [&hasher](double dx, double dy, double dz,
                                     const Base::Vector3d& at, long tag) {
        TopoDS_Shape shape = BRepPrimAPI_MakeBox(dx, dy, dz).Shape();
        if (at.Length() > 0.0) {
            gp_Trsf transform;
            transform.SetTranslation(gp_Vec(at.x, at.y, at.z));
            shape.Move(TopLoc_Location(transform));
        }
        return TopoShape {shape, tag, hasher};
    };

    // A chain of cuts, each feeding the next, so mapped names accumulate history.
    TopoShape current = hashedBox(30, 10, 10, Base::Vector3d(0, 0, 0), 1L);
    for (int step = 0; step < 5; ++step) {
        TopoShape tool =
            hashedBox(2, 2, 20, Base::Vector3d(3.0 + 4.0 * step, 4, -5), 10L + step);
        TopoShape next {current.getShape(), 1L, hasher};
        next.makeElementBoolean(Part::OpCodes::Cut, {current, tool});
        current = next;
    }

    const auto namesBefore = nameByFace(current);
    ASSERT_FALSE(namesBefore.empty());

    std::vector<std::pair<Data::MappedName, double>> stored;
    for (const auto& [index, name] : namesBefore) {
        stored.emplace_back(current.getMappedName(Data::IndexedName("Face", index), false),
                            faceCentreX(current, index));
    }

    std::size_t hashedEntries = 0;
    const TopoShape reloaded = roundTrip(current, true, nullptr, &hashedEntries);
    std::cout << "M5 chain_faces=" << namesBefore.size()
              << " hasher_entries_at_save=" << hashedEntries << std::endl;

    int resolvedSame = 0;
    int resolvedElsewhere = 0;
    int didNotResolve = 0;
    for (const auto& [name, centreX] : stored) {
        const Data::IndexedName resolved = reloaded.getIndexedName(name);
        if (!resolved) {
            ++didNotResolve;
            continue;
        }
        if (near(faceCentreX(reloaded, resolved.getIndex()), centreX)) {
            ++resolvedSame;
        }
        else {
            ++resolvedElsewhere;
        }
    }
    std::cout << "M5 resolved_same=" << resolvedSame
              << " resolved_ELSEWHERE=" << resolvedElsewhere
              << " did_not_resolve=" << didNotResolve << std::endl;

    if (hashedEntries == 0) {
        std::cout << "M5 STILL NOT EXERCISED: even a five-deep boolean chain stored no string IDs"
                  << std::endl;
    }

    // Losing the string table may lose names. It must not silently redirect them.
    EXPECT_EQ(resolvedElsewhere, 0)
        << "UNSAFE CHANGE: losing the string table makes references resolve to the wrong face";
}
