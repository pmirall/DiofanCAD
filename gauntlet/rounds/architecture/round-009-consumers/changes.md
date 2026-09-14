# Round 009 — Changes

## Divergence introduced

**None.** No file under `src/`, `tests/` or `cMake/` was touched. This round is
a source survey plus one re-run of an existing probe, so
`tools/diofancad/divergence-report.sh` was not run — per `CLAUDE.md` §3 it is
required only for changes that touch those paths.

## First round under the D-019 methodology

The pre-flight in `bar.md` was filled in before any measuring: the claim in one
sentence, the second route, all three controls, and — because a "most consumers
are fragile" result would have favoured the programme — a written answer to
"what would make this wrong?".

That answer named the real risk correctly: *my enumeration may be incomplete, so
I would be counting the consumers I found rather than the ones that exist.* The
behaviour-axis check existed to attack exactly that, and the bar committed in
advance to withdrawing the counts rather than patching them if it found
anything. It found two candidates, both of which resolved to properties already
in the list.

The verification step also caught nothing this time, which is worth recording:
three independent counts (55 class-uses, 48 workbench headers, 51 behaviour-axis
files) cohere, and `FilletBase` has exactly the two derivatives the raw-index
count predicted.
