# Round 009 — Input

## Why this round, and why now

Round 008 (element-map coverage in PartDesign) was prepared and blocked:
PartDesign does not run in this configuration, 0 of 20 tests passing. Rather
than debug an environment problem of unknown size, this round takes the
programme's largest gap — nobody had surveyed how many consumers store raw
indices like `Part::Fillet` — which needs **no build at all**.

Source reading is a legitimate route under D-019, and it is immune to the
environment problems that have obstructed the last three rounds.

## What round 007 left

Its surviving finding, established by reading the source rather than by the
probe that was later retracted:

```cpp
struct PartExport FilletElement
{
    int edgeid;
    double radius1, radius2;
};
```

A consumer storing a raw index cannot benefit from the identity layer however
good that layer is. The open question was how many consumers are built that
way.

## Method

Two search axes, deliberately different in kind:

| Axis | Asks |
|---|---|
| **Class** | which property types can hold a sub-element reference, and which features declare them |
| **Behaviour** | which translation units actually manipulate subnames, regardless of the property type behind them |

If the class axis were incomplete, the behaviour axis would show more files
than the class uses can account for. It does not.

Classification then comes from two places per class: the member type, and what
`Save()`/`SaveDocFile()` actually persists. A class whose persistence writes an
integer is not carrying a name, whatever its members suggest.
