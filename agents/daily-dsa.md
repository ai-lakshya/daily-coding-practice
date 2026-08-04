# Daily DSA Instructions

## 1. Determine what to serve
Read context/state.md and context/problem-bank.md.
- If pending_review_ids has an entry due (next_review <= today), serve that
  problem first — spaced repetition beats novelty.
- Else (selection_mode: random-combined): pick uniformly at random from every
  `status: todo` row across all three sources combined (leetcode-150 +
  atcoder-dp + cses) — no per-source rotation, no cursor to advance. This
  naturally randomizes difficulty and topic session to session, per the
  user's explicit request during setup.
- If a source's todo rows are fully exhausted (e.g. all 24 seeded CSES
  problems are solved), it simply stops contributing candidates to the
  random pool — do not auto-seed more. If the user wants more from that
  source, point them at the "Adding a new CSES section" flow in
  agents/log-progress.md (or the equivalent manual ask for the other
  sources).

## 2. Present the problem
Title, link, constraints, examples. Do NOT reveal or hint at the approach.
Explicitly ask the user to attempt it first per profile.md's teaching_style
(default SOLVE_FIRST is not optional unless the user overrides it for this
one session by saying so explicitly).

## 3. Wait for the user's attempt
Do not analyze until the user has actually attempted (pasted code,
pseudocode, or a described approach), unless they explicitly say "just
show me" — treat that as a one-off override, don't change the stored
preference in profile.md because of it.

## 4. Analyze — flaws first, then optimal
a. Point out correctness issues (wrong outputs, missed edge cases,
   off-by-ones) and complexity issues in what the user actually wrote —
   be specific, cite the exact line/step, not generic advice.
b. Explain the optimal approach and explicitly contrast it with theirs:
   what changed and why that change helps.
c. Name the underlying pattern/category so it generalizes to other problems.
d. Give the C++ idiom for the optimal approach, and a JS/TS equivalent.

## 5. Log it
Copy docs/_TEMPLATE.md to docs/<source>/<id>-<slug>.md and fill every
section (see Phase 6 template). Add one row to docs/INDEX.md.

## 6. Update state
- problem-bank.md: status -> solved, last_attempted -> today,
  next_review -> +7d if self_rating <=3 else +14d, doc_file -> the path
  from step 5.
- progress.md: append one row.
- state.md: last_session_date -> today; streak_days += 1 if yesterday was
  the prior session, else reset streak_days to 1.
