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
| cq-005 | leetcode | [2265. Count Nodes Equal to Average of Subtree](https://leetcode.com/problems/count-nodes-equal-to-average-of-subtree/) | `leetcode/2265-tree.cpp` | [`leetcode/2265-tree.md`](leetcode/2265-tree.md) | solved | postorder dfs, subtree aggregation, multi-value returns in c++, integer division semantics, recursion depth, complexity accounting | 2026-09-10 | 2026-09-21 |
| cq-006 | atcoder | [E — Quiz Competition: Qualifiers](https://atcoder.jp/contests/abc475/tasks/abc475_e) | `contests/abc-475/e3.cpp` | [`atcoder/abc475-e-qualifiers.md`](atcoder/abc475-e-qualifiers.md) | partial | binary search with the STL, prefix descent, binary trie, fenwick tree, coordinate compression, offline query processing, bit packing, complexity accounting | 2026-09-18 | 2026-09-18 |
| cq-007 | leetcode | [3524. Find X Value of Array I](https://leetcode.com/problems/find-x-value-of-array-i/) | `leetcode/3524-array.cpp` | [`leetcode/3524-array.md`](leetcode/3524-array.md) | unsolved | subarray counting, modular arithmetic, modular inverse, rolling frequency over bounded states, overflow accounting, monoid transitions | 2026-09-21 | 2026-09-21 |

Next free question id: **cq-008**

<!-- cq-007's `.cpp` is still an empty stub on purpose: the write-up is in
     teach-mode (concepts first, re-attempt checkpoint before §3), so the
     solution file is yours to fill in. -->


<!-- cq-006 is ct-01's problem E, pulled out as a standalone question per
     CONVENTIONS.md §1. Its three solution files stay in `contests/abc-475/`
     (`e.cpp` correct-but-TLE, `e2.cpp` trie, `e3.cpp` accepted) rather than being
     copied here, so there is one source of truth for the code. The write-up exists
     in both places and both copies say so. -->


## Contests
| id | contest | directory | doc | problems | solved | logged |
|----|---------|-----------|-----|---------:|-------:|--------|
| ct-01 | [AtCoder Beginner Contest 475](https://atcoder.jp/contests/abc475/tasks) | `contests/abc-475/` | [CONTEST.md](contests/abc-475/CONTEST.md) · [E](contests/abc-475/e.md) | 7 (A–G) | 4 — A, B, C, D (E partial, see cq-006; F, G unsolved — verified routes written up 2026-09-24) | 2026-09-15 · 2026-09-24 |
| ct-02 | [AtCoder Beginner Contest 476](https://atcoder.jp/contests/abc476/tasks) | `contests/abc-476/` | [CONTEST.md](contests/abc-476/CONTEST.md) | 7 (A–G) | 3 — A, B, C (D, E, F, G not attempted) | 2026-09-22 |
| ct-03 | [Codeforces Round 1122 (Div. 3)](https://codeforces.com/contest/2266) | `contests/cf-1122/` | [CONTEST.md](contests/cf-1122/CONTEST.md) | 8 (A–H) | 2 — A, B (C–H not attempted) | 2026-09-22 |
| ct-04 | [AtCoder Beginner Contest 477](https://atcoder.jp/contests/abc477/tasks) | `contests/abc-477/` | [CONTEST.md](contests/abc-477/CONTEST.md) | 7 (A–G) | 4 — A, B in contest; C, D after (E, F, G not attempted) | 2026-09-27 |

Next free contest id: **ct-05**

<!-- ct-03's directory is named `cf-1122` after the round number; the problem
     URLs use Codeforces contest id 2266. Same contest. -->

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
| `atcoder/` | AtCoder ABC/ARC problems — contest problems promoted to standalone questions (CONVENTIONS.md §1). Distinct from `atcoder-dp/`, which is only the Educational DP Contest. |
| `contests/` | Timed contests — one subdirectory per contest, one `CONTEST.md` each |

## Column notes
- **explanation** — path to the sibling `.md`, or `—` while pending.
- **status** — `solved` / `unsolved` / `partial`; see `CONVENTIONS.md` §4.
  It records how the question went for *you* and never changes afterwards.
- **concepts** — comma-separated tags, filled in when the explanation is
  written. Same strings as the rows in `CONCEPTS.md`.
- **added** — date the solution file appeared.
- **documented** — date the explanation was written.
