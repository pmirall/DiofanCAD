# Round 007 — CORRECTION

**Issued 2026-09-14, after round 008 began. Round 007's headline was wrong, and
the error was in this project's harness, not in FreeCAD.**

## What round 007 claimed

> In this configuration, no document-level Part feature carried an element map.
> `Part::Box`, `Part::Cut` and `Part::Fillet` all report `hasElementMap()`
> false... `PropertyLinkSub` stored a shadow entry whose mapped name was empty.

Both claims are false.

## What is actually true

`PropertyPartShape` has two accessors:

```cpp
const TopoDS_Shape& getValue() const;   // the raw OCCT shape
const TopoShape&    getShape() const;   // the stored TopoShape, with its element map
```

The probe passed `Shape.getValue()` to a `const Part::TopoShape&` parameter.
That **constructs a new TopoShape from a TopoDS_Shape**, and the new one has no
element map. Every zero it reported was produced by the measurement.

Measured correctly, via `getShape()`:

| Feature | `hasElementMap()` | size |
|---|---:|---:|
| `Part::Box` | **0** | 0 |
| `Part::Cut` | **1** | 50 |
| `Part::Fillet` | **1** | 32 |

And the consumer, re-measured against a mapped feature rather than a primitive:

```
P2 subname=Face6 shadow_entries=1 mapped=';Face2;:H346,F.Face6' old='Face6'
P2 the consumer DID store a mapped name
```

## The corrected picture

- **Primitives carry no element map.** `Part::Box::execute` assigns a raw `TopoDS_Shape`. This part of round 007 was right, and it is why the original probe's `Part::Box` row happened to be correct by accident.
- **Derived features do.** Booleans and fillets carry substantial maps.
- **`PropertyLinkSub` does store mapped names** when the referenced feature has a map. Round 007 measured it against a `Part::Box`, which has none, and read the resulting empty shadow as a property of the consumer rather than of the target.

So the identity layer **is** active in Part documents and **does** reach
consumers. Rounds 002–005 describe something real, and the "unknown relationship
to what a user experiences" that round 007 introduced was manufactured by this
error.

## What survives from round 007

| Claim | Status |
|---|---|
| M2: a `Part::Fillet` followed its corner when the box was lengthened | **stands** |
| M3: references survived a real `.FCStd` save/reload | **stands** |
| M1: the reference resolved to the correct physical face | **stands** |
| `Part::Fillet` stores `FilletElement { int edgeid; }` — a raw index, no name | **stands** — read from the source, independent of the broken probe |
| "No document feature carries an element map" | **RETRACTED** |
| "Both references survived by stable indexing, not by identity" | **RETRACTED** for the `PropertyLinkSub` case; still true of `Part::Fillet`, which uses indices by design |

The one genuine consumer-side finding is narrower and stands on source
reading: `Part::Fillet` does not use mapped names, so it cannot benefit from
the identity layer however good that layer is.

## How this happened, and what it says

This is the **fourth** measurement error in eight rounds, and the fourth biased
towards a more dramatic conclusion:

| Round | Error | Would have claimed |
|---|---|---|
| 003 | ground truth required a face plane to match exactly | a silent-corruption defect in FreeCAD |
| 005 | the string-table case was never exercised | a clean pass on an untested question |
| 007 M2 | compared a cylindrical face centre to corner coordinates | a fillet jumping corners |
| 007 P1 | `getValue()` dropped the element map | element maps absent from all documents |

D-017 named this bias one round before it happened again. Naming it was not
enough.

What was missing here is specific and fixable: the probe reported a mechanism
as absent without ever checking that it had looked in the right place. D-015
requires a measurement to say whether it was *exercised*; this one said it was,
because it genuinely ran — it just ran against a value that could not possibly
carry the thing being looked for.

**D-018** adds the missing rule: when a probe reports a mechanism as absent,
it must first demonstrate the probe can detect that mechanism when it *is*
present. A positive control, not only a negative one.

## Process failure, recorded

This correction comes after the wrong claim was committed, pushed, and reported
to the user as the round's headline. The check that caught it was prompted by
writing the *next* round's harness, not by any discipline in this one.
