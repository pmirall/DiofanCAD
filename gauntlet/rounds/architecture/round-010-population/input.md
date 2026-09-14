# Round 010 — Input

## The mechanism, read before measuring

`PropertyLinkBase::_updateElementReference` (`src/App/PropertyLinks.cpp:436`)
resolves a subname through `GeoFeature::resolveElement` and then:

```cpp
if (elementName.newName.empty()) {
    shadow.oldName.swap(elementName.oldName);
    return false;
}
```

A mapped name is stored only when `resolveElement` produces one, and
`GeoFeature::_getElementName` produces one only when the element map resolves
the subname — which round 007's corrected probe showed happens for derived
features and not for primitives.

That yields a prediction with no harness involved: **population follows the
target**. Recorded in `bar.md` before anything ran, so the measurement could
contradict it.

## Why no rate is reportable

If population follows the target, then the fraction of populated references in
a model depends entirely on how many of its references point at primitives.
That is a property of the model, not of FreeCAD. Round 009's gap was phrased as
a rate; this round answers the question behind it instead.

## Method

Three targets in one document — a primitive, a boolean result, and a dress-up
result — each referenced by an `App::FeatureTest::LinkSub`, with the target's
`hasElementMap()` and the reference's shadow read in the same pass.

The correlation is asserted in **both** directions: no populated reference to
an unmapped target, and no empty reference to a mapped one. A one-directional
check would pass on a harness that always reported "empty".
