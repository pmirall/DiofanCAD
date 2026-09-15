#!/bin/sh
# Check that a Gauntlet round meets the rules in CLAUDE.md section 1.
#
#   tools/diofancad/check-round.sh gauntlet/rounds/architecture/round-0NN-name
#
# Exists because those rules were written for a reader with judgement, and
# judgement is exactly what fails at 2am. Eight rounds produced four wrong
# results while a human-readable rulebook sat in the repo. This turns the
# checkable parts into something that returns a non-zero exit code.
#
# It cannot tell you whether your second route is any good. It can tell you
# that you did not write one down.

set -eu

ROUND=${1:-}
if [ -z "$ROUND" ] || [ ! -d "$ROUND" ]; then
    echo "usage: $0 <round-directory>" >&2
    exit 2
fi

FAIL=0
ok()   { printf '  ok    %s\n' "$1"; }
bad()  { printf '  FAIL  %s\n' "$1"; FAIL=1; }
warn() { printf '  warn  %s\n' "$1"; }

need_file() {
    if [ -f "$ROUND/$1" ]; then ok "$1 present"; else bad "$1 missing"; fi
}

need_phrase() {
    # need_phrase <file> <grep-pattern> <description>
    if [ ! -f "$ROUND/$1" ]; then
        bad "$3 (no $1)"
        return
    fi
    if grep -qiE "$2" "$ROUND/$1"; then ok "$3"; else bad "$3"; fi
}

echo "Checking $ROUND"
echo
echo "Files:"
need_file bar.md
need_file verdict.md
need_file largest-gap.md
need_file changes.md
need_file benchmark.json

if [ -f "$ROUND/benchmark.json" ]; then
    if python3 -m json.tool "$ROUND/benchmark.json" >/dev/null 2>&1; then
        ok "benchmark.json parses"
    else
        bad "benchmark.json is not valid JSON"
    fi
fi

if [ -d "$ROUND/evidence" ] && [ -n "$(ls -A "$ROUND/evidence" 2>/dev/null)" ]; then
    ok "evidence/ is not empty"
else
    bad "evidence/ is missing or empty"
fi

echo
echo "Pre-flight, which belongs in bar.md and must be written BEFORE measuring:"
need_phrase bar.md "route 2|second route"            "route 2 named (CLAUDE.md 1.1)"
need_phrase bar.md "negative control"                 "negative control declared (1.2)"
need_phrase bar.md "positive control"                 "positive control declared (1.2)"
need_phrase bar.md "exercised"                        "exercised check declared (1.2, D-015)"
need_phrase bar.md "what would make this wrong"       "'what would make this wrong?' answered (1.3)"
need_phrase bar.md "predict"                          "a prediction recorded before measuring"

echo
echo "Verdict:"
need_phrase verdict.md "PASS|FAIL|UNKNOWN|RETRACT"    "a verdict is stated"
need_phrase verdict.md "scope|caveat|does NOT|not establish" \
                                                      "scope limits stated (1.5)"

# An absence claim without a positive control is the round 007 failure mode.
if [ -f "$ROUND/verdict.md" ]; then
    if grep -qiE "\b(no |none|zero|absent|never|empty)\b" "$ROUND/verdict.md"; then
        if grep -qi "positive control" "$ROUND/bar.md" 2>/dev/null; then
            ok "absence-shaped claim, and a positive control was declared"
        else
            bad "verdict makes an absence claim with no positive control declared (D-018)"
        fi
    fi
fi

# Dramatic results get the higher bar (1.3).
if [ -f "$ROUND/verdict.md" ]; then
    if grep -qiE "defect|bug in|corruption|broken|fails to|wrong" "$ROUND/verdict.md"; then
        warn "verdict claims something is defective - CLAUDE.md 1.3 says re-check the"
        warn "instrument before believing it. Four of four past errors looked like this."
    fi
fi

echo
if [ "$FAIL" -eq 0 ]; then
    echo "PASS - the checkable parts are in place."
    echo "Not checked, and not checkable here: whether the second route is"
    echo "independent, and whether the numbers are physically possible."
else
    echo "FAIL - see above."
fi
exit "$FAIL"
