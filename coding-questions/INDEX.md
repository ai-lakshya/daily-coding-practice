# Coding Questions Index

The registry for `coding-questions/` — every question you have a solution
file for gets exactly one row here. Read this before acting on anything in
this directory; `CONVENTIONS.md` is the rulebook that goes with it.

A row with `—` in the **explanation** column is *pending work*: the
solution exists, the write-up does not. `/coding-questions analyse` picks
those up.

## Questions
| id | platform | problem | solution | explanation | status | concepts | added | documented |
|----|----------|---------|----------|-------------|--------|----------|-------|------------|
| cq-001 | atcoder-dp | [Frog 1](https://atcoder.jp/contests/dp/tasks/dp_a) | `atcoder-dp/a-frog1.cpp` | — | solved | | 2026-09-07 | |
| cq-002 | atcoder-dp | [Frog 2](https://atcoder.jp/contests/dp/tasks/dp_b) | `atcoder-dp/b-frog2.cpp` | — | solved | | 2026-09-07 | |
| cq-003 | atcoder-dp | [Vacation](https://atcoder.jp/contests/dp/tasks/dp_c) | `atcoder-dp/c-vacation.cpp` | — | solved | | 2026-09-08 | |
| cq-004 | atcoder-dp | [Knapsack 1](https://atcoder.jp/contests/dp/tasks/dp_d) | `atcoder-dp/d-knapsack1.cpp` | — | solved | | 2026-09-10 | |
| cq-005 | leetcode | [2265. Count Nodes Equal to Average of Subtree](https://leetcode.com/problems/count-nodes-equal-to-average-of-subtree/) | `leetcode/2265-tree.cpp` | — | solved | | 2026-09-10 | |

Next free question id: **cq-006**

## Contests
| id | contest | directory | doc | problems | solved | logged |
|----|---------|-----------|-----|---------:|-------:|--------|
| ct-01 | [AtCoder Beginner Contest 475](https://atcoder.jp/contests/abc475/tasks) | `contests/abc-475/` | [CONTEST.md](contests/abc-475/CONTEST.md) | 7 (A–G) | 4 — A, B, C, D (E, F, G unsolved) | 2026-09-15 |

Next free contest id: **ct-02**

<!-- contests/lcw-519/ exists but is empty — no row until it has a solution
     file with content in it. -->

## Platforms
Directories that exist, and what belongs in them. Add a row when you
create a new platform directory.

| directory | source |
|-----------|--------|
| `atcoder-dp/` | AtCoder Educational DP Contest (A–Z) |
| `leetcode/` | LeetCode — filename is `<number>-<short-slug>.cpp` |
| `cses/` | CSES Problem Set |
| `contests/` | Timed contests — one subdirectory per contest, one `CONTEST.md` each |

## Column notes
- **explanation** — path to the sibling `.md`, or `—` while pending.
- **status** — `solved` / `unsolved` / `partial`; see `CONVENTIONS.md` §4.
  It records how the question went for *you* and never changes afterwards.
- **concepts** — comma-separated tags, filled in when the explanation is
  written. Same strings as the rows in `CONCEPTS.md`.
- **added** — date the solution file appeared.
- **documented** — date the explanation was written.
