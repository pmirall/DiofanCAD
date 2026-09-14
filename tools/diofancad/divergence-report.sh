#!/bin/sh
# DiofanCAD fork divergence report (V7 §11, §61).
#
# Measures how far the fork has drifted from upstream so that divergence can be
# budgeted like any other engineering resource. Read-only: touches no refs.
#
# Usage:
#   tools/diofancad/divergence-report.sh [upstream-ref] [fork-ref]
#
# Defaults: upstream/main HEAD
#
# Requires the upstream remote:
#   git remote add upstream https://github.com/FreeCAD/FreeCAD.git
#   git fetch upstream main

set -eu

UPSTREAM_REF=${1:-upstream/main}
FORK_REF=${2:-HEAD}

# Paths that are DiofanCAD-native by design. Changes here are program overhead,
# not divergence from upstream code, and are reported separately.
NATIVE='docs/ gauntlet/ tools/diofancad/ DIOFANCAD.md CLAUDE.md'

if ! git rev-parse --verify --quiet "$UPSTREAM_REF" >/dev/null; then
    echo "error: cannot resolve '$UPSTREAM_REF'." >&2
    echo "       add the upstream remote and fetch it first:" >&2
    echo "         git remote add upstream https://github.com/FreeCAD/FreeCAD.git" >&2
    echo "         git fetch upstream main" >&2
    exit 1
fi

if ! git rev-parse --verify --quiet "$FORK_REF" >/dev/null; then
    echo "error: cannot resolve '$FORK_REF'." >&2
    exit 1
fi

BASE=$(git merge-base "$UPSTREAM_REF" "$FORK_REF")
BEHIND=$(git rev-list --count "$FORK_REF..$UPSTREAM_REF")
AHEAD=$(git rev-list --count "$UPSTREAM_REF..$FORK_REF")

UPSTREAM_TIP_DATE=$(git log -1 --format=%cI "$UPSTREAM_REF")
BASE_DATE=$(git log -1 --format=%cI "$BASE")

echo "DiofanCAD divergence report"
echo "==========================="
echo
echo "generated:      $(date -u +%Y-%m-%dT%H:%M:%SZ)"
echo "upstream ref:   $UPSTREAM_REF ($(git rev-parse --short "$UPSTREAM_REF"), tip $UPSTREAM_TIP_DATE)"
echo "fork ref:       $FORK_REF ($(git rev-parse --short "$FORK_REF"))"
echo "merge base:     $(git rev-parse --short "$BASE") ($BASE_DATE)"
echo
echo "upstream age:   $BEHIND commits behind"
echo "fork delta:     $AHEAD commits ahead"
echo

CHANGED_ALL=$(git diff --name-only "$BASE" "$FORK_REF" | wc -l | tr -d ' ')

# Split the diff into DiofanCAD-native paths and modified upstream code.
NATIVE_FILTER=$(printf '%s\n' $NATIVE | sed 's/[.[\*^$]/\\&/g' | paste -sd'|' -)
CHANGED_NATIVE=$(git diff --name-only "$BASE" "$FORK_REF" | grep -Ec "^($NATIVE_FILTER)" || true)
CHANGED_UPSTREAM=$((CHANGED_ALL - CHANGED_NATIVE))

echo "files changed:  $CHANGED_ALL total"
echo "                $CHANGED_NATIVE DiofanCAD-native (docs/gauntlet/tools/charter/agreement)"
echo "                $CHANGED_UPSTREAM in upstream code  <-- this is the divergence that costs"
echo

echo "upstream subsystems touched:"
if [ "$CHANGED_UPSTREAM" -eq 0 ]; then
    echo "  (none)"
else
    git diff --name-only "$BASE" "$FORK_REF" \
        | grep -Ev "^($NATIVE_FILTER)" \
        | awk -F/ '{
            if ($1=="src" && $2=="Mod" && NF>3) print $1"/"$2"/"$3;
            else if ($1=="src" && NF>2) print $1"/"$2;
            else if ($1=="tests" && NF>3) print $1"/"$2"/"$3;
            else if (NF>1) print $1"/"$2;
            else print $1 }' \
        | sort | uniq -c | sort -rn | sed 's/^/  /'
fi
echo

echo "line delta in upstream code:"
if [ "$CHANGED_UPSTREAM" -eq 0 ]; then
    echo "  (none)"
else
    git diff --numstat "$BASE" "$FORK_REF" \
        | grep -Ev "	($NATIVE_FILTER)" \
        | awk '{ add+=$1; del+=$2 } END { printf "  +%d / -%d\n", add, del }'
fi
echo

echo "merge cost against $UPSTREAM_REF:"
if [ "$BEHIND" -eq 0 ]; then
    echo "  up to date, no merge required"
else
    CONFLICTS=$(git merge-tree --write-tree --name-only "$UPSTREAM_REF" "$FORK_REF" 2>/dev/null \
        | sed -n '/^$/,$p' | tail -n +2 | wc -l | tr -d ' ' || echo "UNKNOWN")
    if [ "$CONFLICTS" = "UNKNOWN" ]; then
        echo "  UNKNOWN (git merge-tree --write-tree unavailable; needs git >= 2.38)"
    elif [ "$CONFLICTS" = "0" ]; then
        echo "  clean merge, 0 conflicting files"
    else
        echo "  $CONFLICTS conflicting file(s)"
    fi
fi
echo
echo "Record this output in docs/upstream-sync.md. Do not summarize it from memory."
