# Conventions — `coding-questions/`

The rulebook for this directory. An agent picking up a session reads this
file plus `INDEX.md` and knows how to act. Driven by `/coding-questions`
(full instructions in `agents/coding-questions.md`).

This directory holds **solutions you wrote**, each paired with a written
explanation. It is separate from `docs/`, which holds the write-ups from
the guided `daily-dsa` sessions. Nothing here is generated until you ask
for it — you solve first, the explanation comes after.

---

## 1. Layout

```
coding-questions/
├── INDEX.md              # registry — every question, one row
├── CONCEPTS.md           # concept -> questions where it showed up
├── CONVENTIONS.md        # this file
├── _TEMPLATE-solved.md   # explanation shape for case-1 (you solved it)
├── _TEMPLATE-unsolved.md # explanation shape for case-2 (you didn't)
├── _TEMPLATE-contest.md  # shape for a contest's CONTEST.md
│
├── <platform>/           # atcoder-dp, leetcode, cses, codeforces, ...
│   ├── <slug>.cpp        # your solution
│   └── <slug>.md         # its explanation — same stem, sibling file
│
└── contests/
    └── <contest-slug>/
        ├── <letter>.cpp  # a.cpp, b.cpp, ... one per problem
        └── CONTEST.md    # ONE doc for the whole set — no per-problem .md
```

- **Platform directories** are flat. Create a new one (`codeforces/`,
  `codechef/`, ...) the first time you solve something there, and add it
  to the platform list in `INDEX.md` in the same session.
- **An explanation is a sibling file with the same stem as its solution.**
  `leetcode/2265-tree.cpp` -> `leetcode/2265-tree.md`. That is the whole
  naming rule; there is no date prefix and no id prefix in the filename.
- **Contests are the exception**: a contest directory gets exactly one
  `CONTEST.md` covering every problem in the set. Individual problems in a
  contest do **not** get their own `.md`. If one contest problem deserves
  a full standalone write-up, copy the solution out into its platform
  directory as a normal question and link it from `CONTEST.md`.

## 2. Solution files

Every `.cpp` starts with a header comment carrying the problem link:

```cpp
/*
    Problem link: <url>

    Solution idea:
    1. ...
*/
```

The link is what the agent uses to fetch the statement, so it matters more
than the rest of the header. A `.cpp` with no link still gets documented —
the agent will ask for the URL instead of guessing at the problem.

Empty (0-byte) `.cpp` files are **placeholders**, not unsolved questions.
They are skipped by every mode and never get an INDEX row.

## 3. Addressing

| thing | id form | example |
|---|---|---|
| Question | `cq-NNN` | `cq-007` |
| Contest | `ct-NN` | `ct-01` |

Ids are assigned in `INDEX.md`, zero-padded, **never reused and never
renumbered**. They exist so `context/progress.md` and `CONCEPTS.md` can
point at a question without depending on its path.

## 4. Statuses

| status | meaning |
|---|---|
| `solved` | You solved it yourself and it was accepted on the platform |
| `unsolved` | You could not solve it — the explanation is in teach-mode |
| `partial` | You got a working but wrong-complexity / TLE-ing solution |

Status describes **how the question went for you**, permanently. It is not
a progress tracker and does not change when the explanation gets written —
a question you failed stays `unsolved` even after you understand it. That
history is the point: `unsolved` rows are your revision queue.

Whether an explanation exists is tracked separately, by the `explanation`
column (a path, or `—` when it is still pending).

## 5. What an explanation must contain

Both templates are non-negotiable in **section order**, because the two
cases are read for different reasons:

- **`solved`** — premise -> review of *your* solution -> optimal solution
  -> concept deep-dive. You already have an answer; the value is in what
  was wrong with it and what the question was really testing.
- **`unsolved`** — **concepts first**, then the solution. The concepts
  section has to stand alone well enough that you can close the file and
  re-attempt the question before reading section 2.

Every concept explained anywhere in this directory carries at least one
**real, linkable source** (cppreference, USACO Guide, CP-Algorithms, a
paper, an editorial). No source means the claim does not go in.

## 6. Cross-linking

- Every concept named in an explanation gets a row in `CONCEPTS.md`, and
  the row accumulates every question that used it.
- When a concept already has a deeper treatment somewhere in the repo
  (`concepts/`, `system-design/`, a previous explanation here), link to it
  instead of re-explaining it, and say in one line what today's question
  added.
