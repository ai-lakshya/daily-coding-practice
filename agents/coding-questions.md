# Coding Questions Instructions

The user solves questions on their own, on their own schedule, and drops
the `.cpp` files into `coding-questions/`. This track turns those files
into explanations that compound.

It is **not** the `daily-dsa` track. There, the agent serves a problem and
guides the solve. Here the solving already happened somewhere else and you
are writing the record afterwards. Never serve a problem from this agent;
never log to `problem-bank.md` from it.

Everything lives under `coding-questions/`. `coding-questions/INDEX.md` is
its registry, `coding-questions/CONVENTIONS.md` is its rulebook, and
`coding-questions/CONCEPTS.md` is the cross-question concept map.

---

## 0. Load context first
Read, in this order:
1. `context/state.md` and `context/profile.md` (repo-wide rule — always).
2. `coding-questions/CONVENTIONS.md` — the layout and naming rules.
3. `coding-questions/INDEX.md` — what already exists and what's pending.
4. `coding-questions/CONCEPTS.md` — so you link to an existing concept
   treatment instead of writing a fourth explanation of prefix sums.

## 0.1 Route the request
| the user says | mode |
|---|---|
| "analyse" / "analyse the new questions" / no argument | **A — Analyse** (§1) |
| "I couldn't solve X" / "explain X" + names a file | **B — Unsolved** (§2) |
| "contest <slug>" / names a contest directory | **C — Contest** (§3) |
| "status" / "what's pending" | **D — Status** (§4) |

If the request is ambiguous, look at the file they named: a non-empty
`.cpp` with a real solution in it is mode A, a stub with only a link in it
is mode B. Say which reading you took in one line, then proceed — don't
stall a session on a question you can answer from the file itself.

---

## 1. Mode A — Analyse (case 1: solved, accepted)

### 1.1 Find the pending work
Scan the filesystem — it is the source of truth for "needs a write-up",
and `INDEX.md` is the map that goes with it:

```bash
cd coding-questions
# every solution file that has no sibling explanation
find . -name '*.cpp' -size +0 -not -path './contests/*' \
  | while read -r f; do [ -f "${f%.cpp}.md" ] || echo "$f"; done
```

Rules for this scan:
- Skip **0-byte `.cpp` files** — they're placeholders, not work.
- Skip anything under `contests/` — that's mode C.
- Skip `_TEMPLATE-*.md` and any `_`-prefixed path.
- A `.cpp` newer than its existing `.md` is **not** automatically pending.
  Mention it and ask; the user may have just reformatted.

Cross-check the result against `INDEX.md`. A file on disk with no INDEX row
is new — it gets a row. An INDEX row whose file has vanished gets flagged
to the user, not silently deleted.

### 1.2 Report, then confirm scope
Tell the user what you found before writing anything: the list of pending
files, and a one-line guess at each one's topic. Then:
- **1–2 files** — just do them.
- **3+ files** — these are long documents. Say so, and ask whether to do
  all of them or start with a subset. Do not silently produce eight
  thousand-word write-ups.
- **nothing pending** — say so in one line and stop. Don't invent work.

### 1.3 For each question, gather before writing
1. Read the `.cpp` in full. The header comment carries the problem link
   and usually the user's own stated solution idea — both matter.
2. **No link in the header?** Ask for the URL. Do not guess the problem
   from the code; a write-up premised on the wrong problem is worse than
   no write-up.
3. Fetch the statement with WebFetch. You need the **actual constraints** —
   they drive the entire "what complexity is required" discussion, and you
   cannot recover them from a solution file.
4. Check `CONCEPTS.md` for concepts this question will touch, and
   `concepts/` + `docs/` for deeper existing treatments to link.

### 1.4 Write the explanation
Copy `coding-questions/_TEMPLATE-solved.md` to `<same-stem>.md` beside the
solution and fill every section. The section order is fixed by
`CONVENTIONS.md` §5 — premise, **their** solution, optimal solution,
concept deep-dive.

The things that make this worth reading in three months:

- **Section 2 is a real review.** The user's `profile.md` teaching style is
  `SOLVE_FIRST`; they already have an accepted solution, so the analog here
  is critiquing what they actually wrote before presenting anything better.
  Quote their code. Name the input that breaks it. Point at the `int` that
  should be `long long`, the `size_t`/`int` comparison, the vector sized
  `w` when it needs `w+1`. If the solution is genuinely clean, say so in
  two lines and move on — manufactured criticism trains nothing.
- **Accepted is not the same as correct.** AtCoder and LeetCode both pass
  solutions that are wrong on inputs the test set doesn't contain. If you
  find one, that finding *is* the write-up's value — lead section 2 with it.
- **Section 4 is the point of the directory.** Give each concept the room
  the templates ask for, including the "where it breaks down" part. A
  concept explained only in the shape this problem used it doesn't transfer.
- **Sources are mandatory** (`CONVENTIONS.md` §5). cppreference for
  language semantics, CP-Algorithms / USACO Guide for algorithms, the
  official editorial where one exists. Never cite a URL you haven't
  fetched, and never invent a complexity bound or a "standard" name for a
  technique — if you're unsure a name is real, describe the technique
  instead.

Present the explanation in chat as well as writing it. The file is the
record; the conversation is where the user actually reads it.

### 1.5 Write back
For each question documented:
- `INDEX.md` — fill in `explanation`, `concepts`, `documented`; add the row
  if it was missing; bump "Next free question id".
- `CONCEPTS.md` — a row per concept, or append this question's id to an
  existing row. Reuse existing tag strings rather than coining near-
  duplicates.
- `context/progress.md` — one row, `type: coding-question`, id `cq-NNN`.
  The `key_takeaway` must be what the *analysis* established (including
  anything wrong with their solution), not a restatement of the problem.
- `context/state.md` — `last_session_date` -> today; `streak_days` += 1 if
  yesterday was the prior session, else reset to 1.

---

## 2. Mode B — Unsolved (case 2: couldn't solve it)

The user names a question and its `.cpp` file. That file is typically a
stub containing only the problem link.

### 2.1 Gather
Same as §1.3 — read the file, get the link, **fetch the statement**. Here
the fetch is not optional: there is no solution to infer the problem from.
If the file has no link and the user didn't give one, ask. That is the one
question worth blocking on in this mode.

Also ask — or take from their message — **what they tried and where it
broke**. Their dead end is the most useful thing that goes in the doc, and
it changes what section 2 needs to cover. If they don't volunteer it,
record "not recorded" rather than inventing a struggle they didn't have.

### 2.2 Write it, concepts first
Copy `coding-questions/_TEMPLATE-unsolved.md` to `<same-stem>.md`. **The
section order is inverted relative to the solved template and that is
deliberate:**

1. **Section 2 — concepts.** Everything needed to solve the problem,
   explained from scratch, *with no reference to the actual solution*. The
   test to apply to every subsection: could the user read only this, close
   the file, and have a fair shot at the problem? If a paragraph leaks the
   answer, it belongs in section 3. Worked micro-examples here must use
   some *other* instance, not this problem's.
2. **Section 3 — the solution.** Full detail, assuming section 2 has been
   read. Lead with the derivation — what you'd notice first, why the
   obvious approach fails, what that suggests — before the final algorithm.
   The reasoning path transfers; the answer doesn't.

### 2.3 In chat, respect the re-attempt
Present **section 2 in full in chat first**. Then tell them section 3 is
already in the file, and that re-attempting before reading it is worth more
than the solution is. Do not paste the solution into chat in the same
message as the concepts — that removes the choice the template's structure
exists to give them.

Then let them cross-question. Answer fully, follow it wherever it goes,
push back when they're wrong. When they ask for the solution, walk it.

### 2.4 Write back
As §1.5, with `status: unsolved` in `INDEX.md`. That status is permanent —
it does not become `solved` when they understand the explanation. The
`unsolved` rows are the revision queue; rewriting history erases it.

If the problem is also a row in `context/problem-bank.md`, set that row's
`next_review` for a genuine re-attempt (7 days out), and note in
`progress.md` that the solve was assisted.

---

## 3. Mode C — Contest

A contest directory gets **one `CONTEST.md`** covering the whole set. The
individual problems do not get their own `.md` files.

### 3.1 Gather
- List every `.cpp` in the directory. **Empty files are problems not
  attempted** — they belong in the set table with an honest verdict, not
  omitted and not invented.
- Fetch the contest's task list from its URL to get real problem names and
  statements. Every problem in the set gets a row, including the ones with
  no solution file.
- Ask the user, in one message, for: which they solved **during** the
  contest vs. after, and where the time went. You cannot get either from
  the files, and section 5 is worthless guessed at. If they'd rather not
  say, write "not recorded" and carry on — don't block the doc on it.

### 3.2 Write
Copy `coding-questions/_TEMPLATE-contest.md` to
`contests/<slug>/CONTEST.md` (replacing any stub) and fill it in.

Where the value is concentrated:
- **§2 treats each concept once for the whole set**, naming every problem
  that used it and what varied between them. A per-problem walkthrough
  repeated six times is the failure mode to avoid.
- **§3 "key ideas that would have helped" must be contest-specific.**
  "D's `n <= 18` only ever means bitmask DP or meet-in-the-middle, and you
  spent twenty minutes on greedy" is the shape. "Read constraints
  carefully" is not — cut anything that would apply to any contest.
- **§4 constraint table** is the most reusable artefact here. Fill it for
  every problem, attempted or not.

A contest problem that deserves a full standalone write-up gets **copied**
into its platform directory as a normal question (new `cq-NNN`, its own
`.md`) and linked from `CONTEST.md`. Don't grow per-problem docs inside the
contest directory.

### 3.3 Write back
- `INDEX.md` — the contest's row in the Contests table: problem count,
  solved count, `logged` date. Plus a question row for anything promoted
  out into a platform directory.
- `CONCEPTS.md` — rows for the set's concepts, tagged with the `ct-NN` id.
- `context/progress.md` — one row, `type: contest`, id `ct-NN`.
- `context/state.md` — as §1.5.

---

## 4. Mode D — Status

Read-only. Print, and change nothing:
- Pending write-ups (the §1.1 scan), grouped by platform.
- Contest directories with a stub or missing `CONTEST.md`.
- `unsolved` rows in `INDEX.md` — the revision queue, oldest first.
- Concepts in `CONCEPTS.md` with 3+ questions attached and a confidence
  of 2 or lower, or blank.

End with the single thing you'd do next, and why.

---

## 5. Standing rules

- **Never edit the user's `.cpp` files.** Not to fix a bug, not to fix
  formatting. Their solution is the historical record of what they wrote.
  Corrected code goes in the `.md`, in a fenced block, clearly labelled.
- **Never invent a problem statement or a constraint.** Fetch it, or ask.
  Everything downstream — the complexity target, the "what the constraints
  were signalling" analysis — is wrong if the constraints are wrong.
- **Never fabricate a source.** Cite what you fetched. If a claim has no
  source you can point at, either cut it or mark it explicitly as your own
  reasoning.
- **Don't renumber ids**, ever. `CONVENTIONS.md` §3.
- **Explanations are written on request only.** The user solves on their
  own schedule; this agent runs when they say "analyse", not proactively.
- Match the depth to `profile.md`: intermediate C++ engineer, ~45-60
  min/day. Don't explain what a `std::vector` is. Do explain why the
  recurrence has that base case, and what the same idea looks like when the
  next problem changes one constraint.
