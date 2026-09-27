# ct-04 — AtCoder Beginner Contest 477

| | |
|---|---|
| **Link** | https://atcoder.jp/contests/abc477/tasks |
| **Date attempted** | 2026-09-26 (contest ran 21:00–22:40 JST = 17:30–19:10 IST) |
| **Problems** | 7 (A–G) |
| **Solved in contest** | 2 — A, B |
| **Solved after** | 2 — C, D |
| **Unsolved** | E, F, G — not attempted (`e.cpp` is an empty placeholder; no files for F, G) |
| **Concepts** | nearest neighbour after sorting, pattern occurrence precompute, binary search with the STL, lazy timestamps, shortest path through a hub, interval intersection as prefix difference, offline query processing, fenwick tree, euler tour, lca binary lifting, mo's algorithm, frequency of frequencies, overflow accounting |

<!-- Section order follows ct-02/ct-01: the template's §3/§4/§5 become §4/§5/§6,
     and §3 is the per-problem brute force → optimal ladder. §2 (concepts) comes
     first on purpose: read it, then try E/F/G before reading §3. -->

---

## 1. The set

| # | problem | verdict | key concept | one-line idea |
|---|---------|---------|-------------|---------------|
| A | [Traffic Light](https://atcoder.jp/contests/abc477/tasks/abc477_a) | AC in contest | — | B→Y→R→B; a three-way lookup |
| B | [Standing Outliers](https://atcoder.jp/contests/abc477/tasks/abc477_b) | AC in contest | nearest neighbour after sorting | after sorting, a person's closest other person is an adjacent one |
| C | [Range Search Query](https://atcoder.jp/contests/abc477/tasks/abc477_c) | AC after contest | pattern occurrence precompute | find every occurrence of `T` once; a query is "does the first occurrence starting at ≥ L end by R?" |
| D | [Masking Tape](https://atcoder.jp/contests/abc477/tasks/abc477_d) | AC after contest | lazy timestamps | never repaint — a square's colour is the last paint since it was last uncovered |
| E | [Wheel Distance](https://atcoder.jp/contests/abc477/tasks/abc477_e) | not attempted | shortest path through a hub | answer = `min(clockwise arc, counter-clockwise arc, hub[S] + hub[T])`, where `hub[v]` is the *true* distance to the centre, not `B_v` |
| F | [Count Cells in a Window](https://atcoder.jp/contests/abc477/tasks/abc477_f) | not attempted | interval intersection as prefix difference | a row's black count in `[C,D]` is `g(D) − g(C−1)` with `g(x) = min(R,x) − min(L−1,x)`; sum `min` offline with a Fenwick tree |
| G | [Frequency Query on Tree](https://atcoder.jp/contests/abc477/tasks/abc477_g) | not attempted | mo's algorithm (on a tree) | Euler tour turns a path into a range; Mo's moves the range; `atLeast[f]` answers `a ≤ f ≤ b` in O(1) |

**Solution files:** `a.cpp`–`d.cpp` in this directory are yours and are accepted.
`e.cpp` is a 0-byte placeholder. The E/F/G code in §3 is mine, written for this
write-up.

**How everything in this doc was checked** (not just asserted):
- Your `a.cpp`–`d.cpp` pass every official sample, and `c.cpp` and `d.cpp` were
  each run against a brute force on 500 random small inputs — no mismatches.
- My E, F, G pass every official sample and matched a brute force (Dijkstra per
  query / row-by-row loop / walk-the-path-and-count) on 500–1300 random small
  inputs each — no mismatches.
- Max-size timings on my machine: E 0.03 s, F 0.13 s, G 0.71 s (random tree) and
  0.30 s (path-shaped tree, depth 2×10⁵), against 2 s / 3 s / 3 s limits.

---

## 2. Key concepts across the set

Read this section first, then try E, F and G before opening §3. Each concept is
explained on its own terms; §3 is where they get applied.

### 2.1 After sorting, the nearest element is a neighbour — used in B

**What it is:** on a line, the closest other point to `x_i` is always one of the
two points immediately next to it in sorted order.

**How it works:** suppose the sorted order is `… ≤ p ≤ x ≤ q ≤ …`. Any point `y`
further left than `p` satisfies `y ≤ p ≤ x`, so `x − y ≥ x − p`. Same on the right.
So `min over j≠i of |x_i − x_j| = min(x − p, q − x)`, and "is every distance
`≥ D`" collapses to checking at most two numbers. Duplicates are fine — two equal
values sit next to each other and their gap is 0.

**Complexity:** `O(n log n)` for the sort, `O(n)` for the check, versus `O(n²)`
for comparing every pair.

**How it showed up here:** B asks whether *every* other person is at distance
`≥ D` — a "for all j" condition that the sort turns into "for the two
neighbours". `b.cpp` sorts `(value, index)` pairs so the original numbering
survives the sort, which is exactly the right move.

**The recognisable signal:** "closest", "every other element is at least / at
most …", or "minimum gap" on one dimension. (In 2D this is false — the nearest
point need not be adjacent in any single sorted order.)

**Sources:**
- [USACO Guide — Introduction to Sorting](https://usaco.guide/bronze/intro-sorting)

### 2.2 Precompute every pattern occurrence, then answer windows in O(1) — used in C

**What it is:** when the same short pattern `T` is asked about in many windows
`S[L..R]`, find *where `T` occurs in `S` once*, then answer each window from that
table instead of searching the window.

**How it works:** two ideas stack.

1. **Occurrences are just start positions.** Let `occ[i] = 1` if `S[i .. i+|T|−1] == T`.
   With `|T| ≤ 10`, checking every `i` directly costs `O(|S|·|T|) = 4×10⁶`. (For a
   long `T` you would use KMP or the Z-function to get `O(|S| + |T|)` — not needed
   here, and the constraint `|T| ≤ 10` is the statement telling you so.)
2. **"Contains T" is a condition on the start only.** An occurrence starting at
   `p` lies inside `[L, R]` iff `L ≤ p` and `p + |T| − 1 ≤ R`, i.e.
   `p ∈ [L, R − |T| + 1]`. The window question becomes "is there any 1 in `occ`
   on this index range?", and there are two standard `O(1)` answers:
   - **prefix count:** `P[i] = occ[1] + … + occ[i]`; answer is Yes iff
     `P[R−|T|+1] − P[L−1] > 0` (and No outright if the window is shorter than `T`).
     This is the official editorial's route.
   - **next occurrence:** `nextStart[i]` = smallest `p ≥ i` with `occ[p] = 1`,
     built right-to-left. Answer is Yes iff `nextStart[L] + |T| − 1 ≤ R`. The
     *leftmost* candidate is the only one worth checking, because it also has the
     leftmost end.

**Complexity:** `O(|S|·|T|)` precompute, `O(1)` per query.

**How it showed up here:** C, directly. `c.cpp` finds occurrences in exactly this
way and then uses two `std::partition_point` calls — which is the binary-search
version of "next occurrence" (§3 C covers what that costs).

**The recognisable signal:** many queries, a *fixed* pattern, and windows that
only restrict *where* the pattern may sit. Anything "fixed across queries" should
be precomputed once.

**Sources:**
- [AtCoder ABC 477 C — official editorial](https://atcoder.jp/contests/abc477/editorial/25883) — `occ` + prefix sums, the `[L, R−M+1]` range
- [USACO Guide — Prefix Sums](https://usaco.guide/silver/prefix-sums)
- Existing treatment of `partition_point` in this repo: [ct-01 §2.4](../abc-475/CONTEST.md#24-binary-search-with-the-stl--used-in-c)

### 2.3 Lazy evaluation with timestamps: don't apply "update everything" — used in D

**What it is:** when an operation changes *all* elements in some set (here: every
uncovered square), don't touch the elements. Record *when* the global operation
happened and *what* it did, and let each element work out its own value later from
timestamps.

**How it works:** for each element keep the time it last entered the "affected"
state; globally keep the time and value of the last broad update. The element's
value is then:

- if a broad update happened *after* it became affected → that update's value,
- else → whatever it had when it became affected.

The only subtlety is **freezing**: when an element *leaves* the affected state,
resolve its value right then (using the rule above) and store it, because later
broad updates must no longer reach it.

The reason one global "last update" is enough — rather than a list of all
updates — is that the element has been continuously affected since its timestamp.
So if *any* broad update happened since then, the *last* one did too, and the
last one wins. The official user editorial frames this as "the last type-2 query
in the time interval `[L, R)`" and answers it with binary search or a prefix
table; online, `R` is always "now", so it is just the global last update.

**Complexity:** `O(1)` per operation, `O(N + Q)` total, versus `O(N)` per broad
update (`O(NQ) = 9×10¹⁰`) for the literal simulation.

**How it showed up here:** D. "Uncovered since time `u`" is the affected state;
"paint all uncovered with `C`" is the broad update; covering a square is the
freeze. `d.cpp` implements exactly this.

**The recognisable signal:** an operation over "all elements satisfying X" where
membership in X changes one element at a time. The same trick is how you reset an
array in `O(1)` (store a version number per cell and a global version).

**Sources:**
- [AtCoder ABC 477 D — official editorial](https://atcoder.jp/contests/abc477/editorial/25849) — why the naive repaint is `Θ(NQ)`, and a reverse-order alternative
- [AtCoder ABC 477 D — user editorial (shobonvip)](https://atcoder.jp/contests/abc477/editorial/26411) — uncovered time intervals, "last type-2 query in `[L, R)`", `O(N + Q)` with a prefix table

### 2.4 Shortest paths through a hub: split on whether the path uses it — used in E

**What it is:** in a graph that is a cycle plus one centre vertex joined to every
cycle vertex (a "wheel"), every shortest path either avoids the centre entirely or
passes through it exactly once. Handle the two cases separately.

**How it works:**

- **Avoids the centre:** the path stays on the cycle, so it is one of the two arcs
  between `S` and `T`. With `pre[i]` = clockwise distance from vertex 1 to vertex
  `i` and `total` = cycle length, the arcs are `x = pre[T] − pre[S]` and
  `total − x`.
- **Uses the centre:** a shortest path never repeats a vertex, so it goes
  `S → … → centre → … → T`, and each half should itself be shortest. That is
  `hub[S] + hub[T]`, where `hub[v]` is the shortest distance from `v` to the
  centre.
- **`hub[v]` is not `B_v`.** The best way from `v` to the centre may walk along the
  cycle first and take someone else's cheap spoke. Two ways to get it:
  - Dijkstra from the centre — `O(N log N)`, and what the editorial uses.
  - Relax around the cycle: `hub[i] = min(hub[i], hub[i−1] + A_{i−1})` for
    `i = 1..N` twice (twice so the wrap-around from `N` back to `1` propagates),
    then the same in the other direction. A shortest route from `v` to the centre
    walks the cycle in *one* direction and then takes one spoke, so these sweeps
    cover every candidate. `O(N)`.

  Answer: `min(x, total − x, hub[S] + hub[T])`, or just `hub[S]` when `T = N+1`.

**Complexity:** `O(N)` (or `O(N log N)` with Dijkstra) precompute, `O(1)` per query.
A Dijkstra per query would be `O(Q·N log N) ≈ 7×10¹¹`.

**How it showed up here:** E. Sample 1, query `5 6`: `B_5 = 7` but the answer is
`5` (`5 → 4` for 2, then `B_4 = 3`). That sample is there to catch anyone who uses
`B_S + B_T`.

**The recognisable signal:** a graph with *one* special vertex that every
interesting path might route through (a hub, a root, a portal, a "teleport"). Split
on "uses it / doesn't", and precompute every vertex's distance to it once.

**Sources:**
- [AtCoder ABC 477 E — official editorial](https://atcoder.jp/contests/abc477/editorial/25884) — the `min(x, P_N − x, D_S + D_T)` formula and its two-case proof; mentions the `O(N)` computation of `D` as an exercise
- [CP-Algorithms — Dijkstra](https://cp-algorithms.com/graph/dijkstra.html) — the general tool for `hub[]` when the graph is less structured

### 2.5 Intersection length as a difference of prefix counts — used in F

**What it is:** the number of cells of `[L, R]` that fall in `[C, D]` is
`max(0, min(R, D) − max(L, C) + 1)`. The `max(0, ·)` makes that impossible to sum
over many rows with any data structure. Rewrite it as a difference of *prefix*
counts, which *can* be summed.

**How it works:** let `g(x)` = number of cells of `[L, R]` in columns `1..x`. Then
the count in `[C, D]` is `g(D) − g(C−1)`, and

```
g(x) = min(R, x) − min(L − 1, x)
```

Check the three cases: `x < L` gives `x − x = 0`; `L ≤ x ≤ R` gives
`x − (L−1)`; `x > R` gives `R − (L−1)`. No `max(0, ·)` survives — every term is a
plain `min(v, x)` with a `+` or `−` sign. So a row `[L, R]` becomes two "points":
`(+1, v = R)` and `(−1, v = L−1)`, and a query needs

```
Σ over rows i in [A, B] of  sign · min(v, x)      for x = D and x = C−1
```

**Complexity:** free — algebra. It turns one awkward query into two queries of a
standard shape (§2.6).

**How it showed up here:** F. The official editorial instead cuts the *rows*: it
answers "rows `1..X`, columns `[C, D]`" and subtracts. Both are the same
prefix-difference idea applied to a different axis (§3 F compares them).

**The recognisable signal:** a count over a rectangle, or an overlap length,
summed across many objects. Look for the axis where "up to `x`" is easy and turn
the range `[C, D]` into two prefixes.

**Sources:**
- [USACO Guide — Prefix Sums](https://usaco.guide/silver/prefix-sums) — range = difference of two prefixes
- [AtCoder ABC 477 F — official editorial](https://atcoder.jp/contests/abc477/editorial/26406) — the row-prefix form `S(B, C, D) − S(A, C, D)`

### 2.6 Offline sweep + Fenwick tree for sums of min(v, x) over an index range — used in F

**What it is:** answering many queries `Σ_{i ∈ [A, B]} wᵢ · min(vᵢ, x)` by sorting
the queries by `x` and sweeping, so each point is inserted into a Fenwick tree
exactly once.

**How it works:** split the `min` at `x`:

```
Σ wᵢ · min(vᵢ, x)  =  Σ_{vᵢ ≤ x} wᵢ·vᵢ   +   x · Σ_{vᵢ > x} wᵢ
```

Sort points by `v` and queries by `x`. Sweep `x` upward; when a point's `v`
becomes `≤ x`, add `wᵢ·vᵢ` to Fenwick #1 and `wᵢ` to Fenwick #2 **at position `i`**
(its row). Then "the `vᵢ ≤ x` part restricted to rows `[A, B]`" is a range sum on
each tree, and the `vᵢ > x` part is (total weight in `[A, B]`) − (Fenwick #2's
range sum). In F every row carries weights `+1` and `−1`, so the total weight of
any row range is 0 and that last term is just `−(Fenwick #2's range sum)`.

This is the same *offline* pattern as ABC 475 E: the queries are known up front,
so reorder them to make one dimension monotone and let a Fenwick tree handle the
other.

**Complexity:** `O((N + Q) log N)` time, `O(N + Q)` memory.

**How it showed up here:** F, after §2.5's rewrite. Each query is two evaluations
(`x = D` with `+`, `x = C−1` with `−`).

**The recognisable signal:** a query with two independent ranges ("rows `A..B`"
and "value up to `x`") and no updates. Two ranges + offline = sweep one, Fenwick
the other.

**Sources:**
- [CP-Algorithms — Fenwick Tree](https://cp-algorithms.com/data_structures/fenwick.html)
- Existing treatments in this repo: [cq-006 §2.5 Fenwick tree](../abc-475/e.md#25-fenwick-tree-binary-indexed-tree), [cq-006 §2.7 offline query processing](../abc-475/e.md#27-offline-query-processing)

### 2.7 Euler tour + LCA: a tree path as an array range — used in G

**What it is:** write down each vertex twice during a DFS — once on entry
(`tin[v]`) and once on exit (`tout[v]`) — giving an array `order` of length `2N`.
A path between two vertices then corresponds to a contiguous range of `order`, as
long as you count only vertices that appear **exactly once** in that range.

**How it works:** order the endpoints so `tin[s] ≤ tin[t]`, and let `w = lca(s, t)`.

- **`w == s`** (s is an ancestor of t): take `order[tin[s] .. tin[t]]`. Every vertex
  on the `s → t` path has been entered but not yet exited — it appears once. Every
  other vertex in the range is a finished side branch: entered *and* exited, so it
  appears twice.
- **`w ≠ s`**: take `order[tout[s] .. tin[t]]`. Walking from leaving `s` to entering
  `t`, the vertices that appear once are exactly the path minus `w` (we are still
  inside `w`'s subtree the whole time, so `w` itself never shows). Add `w` by hand.

So maintain a per-vertex "in the current set?" flag and **toggle** it every time
the range passes that vertex: the second visit cancels the first.

(The official editorial uses the equivalent form "`order[tin[s]+1 .. tin[t]]`, plus
the LCA always", justified via the symmetric difference of the two root paths.)

The LCA itself comes from **binary lifting**: `up[k][v]` is `v`'s `2ᵏ`-th ancestor;
lift the deeper vertex to equal depth, then lift both while their ancestors differ.
`O(N log N)` build, `O(log N)` per query.

Implementation note: the DFS must be iterative (or the stack raised). A path-shaped
tree with `N = 2×10⁵` means recursion depth `2×10⁵`, which is enough to crash on a
default stack.

**Complexity:** `O(N log N)` preprocessing; each query becomes `O(1)` range
endpoints plus `O(log N)` for the LCA.

**How it showed up here:** G — it is the adapter that lets an array technique
(Mo's, §2.8) run on tree paths.

**The recognisable signal:** "on the path from `s` to `t`" together with a
per-query aggregate that is *not* invertible (counts of distinct values,
frequencies). Invertible aggregates like sums don't need this — prefix sums from
the root plus LCA do it. Non-invertible ones need the Euler range.

**Sources:**
- [USACO Guide — Euler Tour Technique](https://usaco.guide/gold/tree-euler)
- [CP-Algorithms — LCA with binary lifting](https://cp-algorithms.com/graph/lca_binary_lifting.html)
- [AtCoder ABC 477 G — official editorial](https://atcoder.jp/contests/abc477/editorial/25886) — the path = odd-occurrence set proof

### 2.8 Mo's algorithm — used in G

**What it is:** an offline way to answer many range queries on a static array when
you can cheaply **add or remove one element** from a maintained "current window",
but have no way to combine two windows.

**How it works:** keep a current window `[curL, curR]`. To answer a query `[l, r]`,
move the two ends one step at a time, calling `add`/`remove` (in G: `toggle`) per
step, then read the answer from the maintained state. The whole trick is the
**order** in which queries are answered:

- cut the index space into blocks of size `K`;
- sort queries by the block of `l`, and inside a block by `r` (alternating `r`
  ascending/descending between blocks cuts the right pointer's back-and-forth).

Within a block `l` moves at most `K` per query; `r` moves at most `2N'` per block
(`N'` = array length). Total moves ≈ `Q·K + (N'/K)·N'`, minimised at
`K ≈ N'/√Q`, giving `O(N'·√Q)` moves.

**Complexity:** `O(N'·√Q · cost(add/remove) + Q · cost(answer))`. For G,
`N' = 2N = 4×10⁵`, `Q = 2×10⁵`: about `3.6×10⁸` toggles in the worst case, each a
couple of array writes. Measured 0.71 s here.

**Where it breaks down:** it needs every query up front (no online queries), a
static array (plain Mo's has no point updates), and an `add`/`remove` that is genuinely `O(1)` — an `O(log N)`
structure inside it multiplies `3.6×10⁸` by 18.

**How it showed up here:** G, on the Euler array from §2.7.

**The recognisable signal:** `N, Q ≈ 2×10⁵`, no updates, and a query about the
*distribution* of values in a range (distinct count, mode, "how many values occur
between `a` and `b` times"). If you can maintain the answer while sliding the
window one element at a time, but not while merging, it's Mo's.

**Sources:**
- [CP-Algorithms — Sqrt Decomposition, section "Mo's algorithm"](https://cp-algorithms.com/data_structures/sqrt_decomposition.html)
- [USACO Guide — Square Root Decomposition](https://usaco.guide/plat/sqrt)

### 2.9 Frequency of frequencies — used in G

**What it is:** alongside `cnt[y]` (how often value `y` appears in the window),
keep `atLeast[f]` = how many distinct values have `cnt ≥ f`. Then "how many values
appear between `a` and `b` times" is `atLeast[a] − atLeast[b+1]`.

**How it works:** Mo's only ever changes a count by ±1. When `cnt[y]` goes
`k → k+1`, the only threshold whose status changes for `y` is `f = k+1`, so exactly
one entry changes: `atLeast[k+1]++`. When it goes `k → k−1`, `atLeast[k]--`.
Both `O(1)`, and so is the query. (A value with count 0 is never counted, which is
fine because `a ≥ 1`.)

The more obvious structure — `exactly[f]` = number of values with `cnt == f` —
also updates in `O(1)`, but then the query is a range sum over `[a, b]`, which
needs a Fenwick tree (`O(log N)` per update — too slow inside Mo's, per §2.8) or
sqrt buckets (`O(√N)` per query). Storing the *cumulative* form instead removes the
query cost entirely, because each ±1 step touches only one cumulative entry.

**Complexity:** `O(1)` update, `O(1)` query, `O(N)` memory.

**How it showed up here:** G's per-query condition `a ≤ f ≤ b`.

**The recognisable signal:** inside a sliding window or Mo's, a question about how
many values have a count in some range. If the counts move by ±1, a cumulative
"at least" array is always `O(1)`.

**Sources:**
- [AtCoder ABC 477 G — official editorial](https://atcoder.jp/contests/abc477/editorial/25886) — `g_i` = number of `f_j ≥ i`, answer `g_a − g_{b+1}`

---

## 3. From brute force to optimal, problem by problem

### A — Traffic Light

One rung: map `B→Y`, `Y→R`, `R→B`. `a.cpp` does it with an `if`/`else if`
chain and is correct. (A lookup like `"BYR"[("BYR".find(c) + 1) % 3]` is the form
that survives a longer cycle, but at three colours the `if` chain is clearer.)

### B — Standing Outliers

- **Rung 1 — all pairs, `O(n²)`.** For each `i`, check `|X_i − X_j| ≥ D` for all
  `j ≠ i`. With `N ≤ 100` that is 10⁴ comparisons, and it is the right thing to
  write in a contest when `N` is this small — nothing to get wrong.
- **Rung 2 — sort, check neighbours, `O(n log n)`** (§2.1). What `b.cpp` does.

**Review of `b.cpp`:** correct. The endpoints get one-sided checks, the interior
gets two, and `N ≥ 2` guarantees `x[1]` and `x[n−2]` exist. It prints the empty
second line when `K = 0`, which the statement requires. `int` is safe:
coordinates are in `[1, 10⁹]`, so a difference is below `10⁹`. One habit note:
`std::vector<std::vector<int>>` for `(value, index)` does one heap allocation per
element; `std::vector<std::pair<int, int>>` is the idiomatic form, sorts the same
way (lexicographically), and is contiguous. Irrelevant at `N = 100`, visible at
`N = 10⁶`.

### C — Range Search Query

- **Rung 1 — search each window.** For each query, look for `T` inside
  `S[L..R]`: `O(|S|·|T|)` per query, `2×10⁵ × 4×10⁶` worst case. Dead.
- **Rung 2 — precompute occurrences, binary-search per query,
  `O(|S||T| + Q log |S|)`.** This is `c.cpp`.
- **Rung 3 — `O(1)` per query** with a prefix count or a `nextStart` array
  (§2.2).

**Review of `c.cpp`:** correct — samples pass and it matched a brute force on
500 random inputs. It runs in 0.30 s on the worst case (`S = a⁴⁰⁰⁰⁰⁰`,
`T = a¹⁰`, so ~4×10⁵ occurrences). Three things worth knowing:

1. **The loop after the two searches never runs more than once.** `left` is the
   first occurrence starting at `≥ l`, `right` is the first whose end is `> r`.
   Occurrences are sorted by start, and since they all have length `|T|` they are
   sorted by end too. So *every* `j` in `[leftId, rightId)` has start `≥ l` and
   end `≤ r` — the very first iteration prints `Yes`. The whole loop is
   equivalent to `leftId < rightId`. Even simpler: only `pres[leftId]` matters,
   because the leftmost qualifying start also has the leftmost end — that's the
   `nextStart` idea.
2. **The lambdas take `std::vector<int> a` by value.** `partition_point` calls the
   predicate `~log₂(4×10⁵) ≈ 19` times per search, two searches per query, so
   that is ~7.6×10⁶ heap-allocating vector copies over the run. Take
   `const std::vector<int>& a`. Better, store only the start positions in a
   `std::vector<int>` — the end is always `start + |T| − 1`, so storing it is
   redundant.
3. The `if (s[i] == t[0])` pre-check is redundant (the inner loop's first
   iteration does the same test) — harmless.

**Rung 3, written out** (verified on samples and 600 random inputs):

```cpp
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int q;
    std::string s, t;
    std::cin >> q >> s >> t;
    int n = s.size(), m = t.size();

    // nextStart[i] = smallest p >= i where T occurs in S starting at p (0-based), or n if none.
    std::vector<int> nextStart(n + 1, n);
    for(int i = n - 1 ; i >= 0 ; i--)
    {
        nextStart[i] = nextStart[i + 1];
        if(i + m <= n && s.compare(i, m, t) == 0)
            nextStart[i] = i;
    }

    while(q--)
    {
        int l, r;
        std::cin >> l >> r;
        l--; r--;
        int p = nextStart[l];
        std::cout << (p + m - 1 <= r ? "Yes\n" : "No\n");
    }
    return 0;
}
```

### D — Masking Tape

- **Rung 1 — simulate.** Keep a colour and a tile flag per square; a type-2 query
  repaints every uncovered square. `O(N)` per type-2 query, `O(NQ) = 9×10¹⁰`
  worst case.
- **Rung 2 — replay only the relevant paint.** For each square, list the time
  intervals during which it was uncovered, and for each interval find the last
  type-2 query inside it (binary search over the sorted type-2 times).
  `O((N + Q) log Q)` — this is the user editorial's first variant.
- **Rung 3 — timestamps, `O(N + Q)`** (§2.3). Because a square is *currently*
  uncovered since its timestamp, "the last paint inside the interval" is simply
  "the last paint so far, if it came after the timestamp". This is `d.cpp`.

**Review of `d.cpp`:** correct and already optimal — the same idea as the user
editorial's `O(N + Q)` solution, done online. Samples pass and it matched a brute
force on 500 random inputs. The logic is right in the subtle place: on cover it
freezes `colors[j]` to either `currColor` (if a paint happened since the square
was uncovered) or the colour it already had; on uncover it records the time. The
initial state (`uncover_time = 0`, `lastPaintTime = 0`, colour `a`) correctly
means "uncovered since the start, no paint yet".

Readability notes only:
- `parent` is really two things: the sentinel `101` means "uncovered", any other
  value is "covered" (and duplicates `colors[j]`). A `std::vector<char> covered`
  plus `colors` says what it means; `parent[j] = colors[j]` in the cover branch
  then disappears.
- The "colour of an uncovered square right now" rule appears twice (at cover time
  and in the final loop). Pulling it into one lambda means it cannot drift.

Same algorithm with those two changes (verified on samples and 600 random inputs):

```cpp
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;

    std::vector<char> covered(n + 1, 0);
    std::string color(n + 1, 'a');          // valid for covered squares, and for uncovered ones as of exposedAt
    std::vector<int> exposedAt(n + 1, 0);    // time the square was last uncovered (0 = start)
    int lastPaintTime = -1;
    char lastPaintColor = 'a';

    // Colour of an *uncovered* square right now.
    auto current = [&](int x) {
        return lastPaintTime > exposedAt[x] ? lastPaintColor : color[x];
    };

    for(int time = 1 ; time <= q ; time++)
    {
        int type;
        std::cin >> type;
        if(type == 1)
        {
            int x;
            std::cin >> x;
            if(!covered[x]) color[x] = current(x);   // freeze its colour under the tile
            else            exposedAt[x] = time;     // exposed from now on
            covered[x] ^= 1;
        }
        else
        {
            std::cin >> lastPaintColor;
            lastPaintTime = time;
        }
    }

    for(int x = 1 ; x <= n ; x++)
        std::cout << (covered[x] ? color[x] : current(x));
    std::cout << '\n';
    return 0;
}
```

### E — Wheel Distance

- **Rung 1 — Dijkstra per query.** `O(Q · N log N) ≈ 7×10¹¹`. Correct, and it is
  the brute force I tested against.
- **Rung 2 — notice the graph is fixed.** Only `S` and `T` change between
  queries. So ask: what does *any* shortest `S–T` path look like in a wheel? It
  either stays on the rim (two choices: the two arcs) or goes through the centre
  once (§2.4).
- **Rung 3 — the tempting wrong answer.** `min(arc1, arc2, B_S + B_T)`. Fails on
  sample 1's second query (`5 → 6`: `B_5 = 7`, answer 5). The spoke you take
  doesn't have to be your own.
- **Rung 4 — precompute `hub[v]`** (true distance to the centre) once, `O(N)` with
  the two-direction cycle sweeps or `O(N log N)` with one Dijkstra from the centre.
  Then every query is `O(1)`:
  `T = N+1 → hub[S]`, else `min(x, total − x, hub[S] + hub[T])`.

**Overflow:** `total` can reach `2×10⁵ × 10⁹ = 2×10¹⁴` — `long long` everywhere.

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n + 1), b(n + 1);
    for(int i = 1 ; i <= n ; i++) std::cin >> a[i];
    for(int i = 1 ; i <= n ; i++) std::cin >> b[i];

    // pre[i] = clockwise distance 1 -> i along the cycle.
    std::vector<long long> pre(n + 1, 0);
    for(int i = 2 ; i <= n ; i++) pre[i] = pre[i - 1] + a[i - 1];
    long long total = pre[n] + a[n];

    // hub[i] = true shortest distance from cycle vertex i to vertex N+1.
    // Relax around the cycle twice in each direction.
    std::vector<long long> hub(b);
    for(int pass = 0 ; pass < 2 ; pass++)
        for(int i = 1 ; i <= n ; i++)
        {
            int prev = (i == 1) ? n : i - 1;
            hub[i] = std::min(hub[i], hub[prev] + a[prev]);
        }
    for(int pass = 0 ; pass < 2 ; pass++)
        for(int i = n ; i >= 1 ; i--)
        {
            int next = (i == n) ? 1 : i + 1;
            hub[i] = std::min(hub[i], hub[next] + a[i]);
        }

    while(q--)
    {
        int s, t;
        std::cin >> s >> t;
        if(t == n + 1)
        {
            std::cout << hub[s] << '\n';
            continue;
        }
        long long cw = pre[t] - pre[s];
        std::cout << std::min({cw, total - cw, hub[s] + hub[t]}) << '\n';
    }
    return 0;
}
```

### F — Count Cells in a Window

- **Rung 1 — loop over rows.** For each query, add
  `max(0, min(R_i, D) − max(L_i, C) + 1)` for `i = A..B`. `O(NQ) = 4×10¹⁰`. The
  brute force.
- **Rung 2 — 2D prefix sums over the grid.** `O(1)` per query, but the grid is
  `N × M = 4×10¹⁰` cells. Dead on memory. It *is* the right idea, though — the
  answer is a 2D range sum. The job is to get it without materialising the grid.
- **Rung 3 — remove the `max(0, ·)`** (§2.5). Each row becomes `+min(R_i, x)` and
  `−min(L_i − 1, x)`, and a query becomes two evaluations of
  `Σ_{rows A..B} sign · min(v, x)`.
- **Rung 4 — offline sweep over `x` with two Fenwick trees indexed by row** (§2.6).
  `O((N + Q) log N)`.

**The editorial's route** is the same prefix idea on the other axis: sweep the
rows `1, 2, …`, and for row `X` do "+1 on every column in `[L_X, R_X]`" in a lazy
segment tree (range add, range sum). A query becomes
`sum over columns [C, D] after row B − same after row A−1`. Also
`O((N + Q) log M)`. Mine avoids the lazy segment tree at the price of the §2.5
algebra — use whichever structure you trust more under time pressure.

**Overflow:** an answer can reach `2×10⁵ × 2×10⁵ = 4×10¹⁰` — `long long`.

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

// Fenwick tree over row indices, 1-based.
struct Fenwick
{
    std::vector<long long> t;
    explicit Fenwick(int n) : t(n + 1, 0) {}
    void add(int i, long long v) { for(; i < (int)t.size() ; i += i & -i) t[i] += v; }
    long long sum(int i) const { long long s = 0; for(; i > 0 ; i -= i & -i) s += t[i]; return s; }
    long long sum(int l, int r) const { return sum(r) - sum(l - 1); }
};

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m, q;
    std::cin >> n >> m >> q;

    // Row i contributes min(R_i, x) - min(L_i - 1, x) black cells in columns 1..x.
    // Each row therefore gives two "points" (row, v, sign): +R_i and -(L_i - 1).
    struct Point { int v, row, sign; };
    std::vector<Point> pts;
    pts.reserve(2 * n);
    for(int i = 1 ; i <= n ; i++)
    {
        int l, r;
        std::cin >> l >> r;
        pts.push_back({r, i, +1});
        pts.push_back({l - 1, i, -1});
    }

    // Each query becomes two evaluations of G(A, B, x) = sum over rows of prefix count up to x:
    // answer = G(A, B, D) - G(A, B, C - 1).
    struct Eval { int x, a, b, id, sign; };
    std::vector<Eval> evals;
    evals.reserve(2 * q);
    for(int k = 0 ; k < q ; k++)
    {
        int a, b, c, d;
        std::cin >> a >> b >> c >> d;
        evals.push_back({d, a, b, k, +1});
        evals.push_back({c - 1, a, b, k, -1});
    }

    std::sort(pts.begin(), pts.end(), [](const Point& p, const Point& o) { return p.v < o.v; });
    std::sort(evals.begin(), evals.end(), [](const Eval& p, const Eval& o) { return p.x < o.x; });

    // sum of sign * min(v, x) = (sum of sign * v over v <= x) + x * (sum of sign over v > x).
    // Every row has one +1 and one -1 point, so (sum of sign over v > x) = -(sum of sign over v <= x).
    Fenwick valueSum(n), signLow(n);

    std::vector<long long> ans(q, 0);
    size_t ptr = 0;
    for(const auto& e : evals)
    {
        while(ptr < pts.size() && pts[ptr].v <= e.x)
        {
            valueSum.add(pts[ptr].row, 1LL * pts[ptr].sign * pts[ptr].v);
            signLow.add(pts[ptr].row, pts[ptr].sign);
            ptr++;
        }
        long long low = valueSum.sum(e.a, e.b);
        long long high = -signLow.sum(e.a, e.b);
        ans[e.id] += e.sign * (low + 1LL * e.x * high);
    }

    for(long long v : ans) std::cout << v << '\n';
    return 0;
}
```

### G — Frequency Query on Tree

- **Rung 1 — walk the path per query.** Find the path (DFS or parent pointers +
  LCA), count values in a map, count values with `a ≤ f ≤ b`. `O(N)` per query,
  `O(NQ) = 4×10¹⁰`. The brute force.
- **Rung 2 — why nothing cheap works.** Sums along a path are easy (root prefix
  sums + LCA), but "how many distinct values have frequency in `[a, b]`" can't be
  combined from two halves, and there are no updates to exploit. No updates +
  offline + non-mergeable → Mo's (§2.8).
- **Rung 3 — flatten the tree.** Euler tour with entry and exit (§2.7): each path
  becomes a range of `order` (plus the LCA when it isn't an endpoint), with
  "toggle on every visit" semantics.
- **Rung 4 — Mo's + a frequency structure.** Counting `exactly[f]` and answering
  with sqrt buckets works (`O(√N)` per query — my first version did this and ran
  in 1.25 s). The editorial's `atLeast[f]` (§2.9) makes both the update and the
  query `O(1)`, and dropped my worst case to 0.71 s.

Total: `O(N log N + (2N)·√Q + Q log N)`.

```cpp
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;
    std::vector<int> x(n + 1);
    for(int i = 1 ; i <= n ; i++) std::cin >> x[i];

    std::vector<std::vector<int>> adj(n + 1);
    for(int i = 0 ; i < n - 1 ; i++)
    {
        int u, v;
        std::cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // Euler tour where every vertex appears twice: at tin (enter) and tout (leave).
    // Iterative DFS so a path-shaped tree of 2e5 vertices doesn't overflow the stack.
    const int LOG = 18;
    std::vector<int> tin(n + 1), tout(n + 1), depth(n + 1, 0), order(2 * n);
    std::vector<std::vector<int>> up(LOG, std::vector<int>(n + 1, 1));
    {
        int timer = 0;
        std::vector<std::pair<int, size_t>> st;
        st.push_back({1, 0});
        tin[1] = timer; order[timer++] = 1;
        while(!st.empty())
        {
            auto& [u, idx] = st.back();
            if(idx < adj[u].size())
            {
                int v = adj[u][idx++];
                if(v == up[0][u] && u != 1) continue;
                up[0][v] = u;
                depth[v] = depth[u] + 1;
                tin[v] = timer; order[timer++] = v;
                st.push_back({v, 0});
            }
            else
            {
                tout[u] = timer; order[timer++] = u;
                st.pop_back();
            }
        }
    }
    for(int k = 1 ; k < LOG ; k++)
        for(int v = 1 ; v <= n ; v++)
            up[k][v] = up[k - 1][up[k - 1][v]];

    auto lca = [&](int u, int v) {
        if(depth[u] < depth[v]) std::swap(u, v);
        for(int k = LOG - 1 ; k >= 0 ; k--)
            if(depth[u] - (1 << k) >= depth[v]) u = up[k][u];
        if(u == v) return u;
        for(int k = LOG - 1 ; k >= 0 ; k--)
            if(up[k][u] != up[k][v]) { u = up[k][u]; v = up[k][v]; }
        return up[0][u];
    };

    struct Query { int l, r, extra, a, b, id; };
    std::vector<Query> qs(q);
    for(int k = 0 ; k < q ; k++)
    {
        int s, t, a, b;
        std::cin >> s >> t >> a >> b;
        if(tin[s] > tin[t]) std::swap(s, t);
        int w = lca(s, t);
        if(w == s) qs[k] = {tin[s], tin[t], 0, a, b, k};
        else       qs[k] = {tout[s], tin[t], w, a, b, k};
    }

    // Mo's ordering over the 2n-long Euler array.
    int block = std::max(1, (int)(2 * n / std::sqrt((double)q)));
    std::sort(qs.begin(), qs.end(), [&](const Query& p, const Query& o) {
        if(p.l / block != o.l / block) return p.l < o.l;
        return ((p.l / block) & 1) ? p.r > o.r : p.r < o.r;
    });

    // cnt[y]    = occurrences of value y on the current path
    // atLeast[f] = how many values currently occur at least f times
    // A +1 on cnt[y] (k -> k+1) changes exactly one entry: atLeast[k+1]++.
    std::vector<int> cnt(n + 1, 0), atLeast(n + 2, 0);
    std::vector<char> inPath(n + 1, 0);

    auto toggle = [&](int v) {
        int y = x[v];
        if(inPath[v]) atLeast[cnt[y]--]--;
        else          atLeast[++cnt[y]]++;
        inPath[v] ^= 1;
    };

    std::vector<int> ans(q);
    int curL = 0, curR = -1;
    for(const auto& qu : qs)
    {
        while(curL > qu.l) toggle(order[--curL]);
        while(curR < qu.r) toggle(order[++curR]);
        while(curL < qu.l) toggle(order[curL++]);
        while(curR > qu.r) toggle(order[curR--]);
        if(qu.extra) toggle(qu.extra);
        ans[qu.id] = atLeast[qu.a] - atLeast[qu.b + 1];
        if(qu.extra) toggle(qu.extra);
    }

    for(int v : ans) std::cout << v << '\n';
    return 0;
}
```

---

## 4. Key ideas that would have helped

- **C: you already had the answer after the searches.** Occurrences sorted by
  start are also sorted by end (fixed length), so "first start `≥ L`" is the only
  candidate worth checking — if it doesn't end by `R`, nothing does. The
  `for (j = leftId; j < rightId; …)` loop is a sign the monotonicity wasn't fully
  trusted. When a search lands on the extremal candidate, ask whether *that one*
  decides the query.
- **D: "change all elements with property X" is a timestamp, not a loop.** You
  found this after the contest. Worth naming so it's instant next time: global
  `(lastTime, lastValue)` + a per-element "since when" + freeze on exit.
- **E was the reachable one, and it's a two-case argument.** 450 points, no data
  structure. Everything hinges on one sentence: *the path uses the centre or it
  doesn't*. The only trap — `B_v` isn't the distance to the centre — is caught by
  sample 1 query 2, so checking the samples by hand would have exposed it. If time
  after D was spent anywhere, E was the right place.
- **F: kill the `max(0, …)` before looking for a data structure.** An overlap
  length summed over many rows looks like it needs something exotic. Writing it as
  `g(D) − g(C−1)` with `g(x) = min(R, x) − min(L−1, x)` turns it into sums of
  `min(v, x)` — the same "offline + Fenwick" shape as ABC 475 E.
- **G: `N, Q = 2×10⁵`, static tree, "distinct values with frequency in a range",
  3 s limit.** That combination is Mo's on a tree. The 3-second limit (not 2) is
  the setter leaving room for `N√Q`.

## 5. Reading the constraints

| # | limits | what that was signalling |
|---|--------|--------------------------|
| A | `c ∈ {B, Y, R}` | no algorithm — a lookup |
| B | `N ≤ 100`, `X_i, D ≤ 10⁹` | `O(N²)` is fine; differences `< 10⁹` fit `int` |
| C | `\|S\| ≤ 4×10⁵`, `\|T\| ≤ 10`, `Q ≤ 2×10⁵` | `\|T\| ≤ 10` is the whole hint: naive matching at every start (`4×10⁶`) is allowed, no KMP needed. `Q` large → `O(1)` or `O(log)` per query from a precomputed table |
| D | `N, Q ≤ 3×10⁵` | `O(NQ) = 9×10¹⁰` is out, so a type-2 query must be `O(1)` — you can't touch the squares. That forces lazy evaluation |
| E | `N, Q ≤ 2×10⁵`, `A_i, B_i ≤ 10⁹` | fixed graph, many queries → precompute once, `O(1)` per query. Path lengths reach `2×10¹⁴` → `long long` |
| F | `N, M, Q ≤ 2×10⁵`, 3 s | `N·M = 4×10¹⁰` rules out any grid; `N·Q` rules out per-row work. `O((N+Q) log)` → offline sweep + BIT / segment tree. Answers up to `4×10¹⁰` → `long long` |
| G | `N, Q ≤ 2×10⁵`, `x_i ≤ N`, 3 s | no updates, offline allowed, non-mergeable query, and an extended time limit → `O(N√Q)` Mo's. `x_i ≤ N` means `cnt[]` is a plain array — no map or coordinate compression |

## 6. Post-mortem

**Timeline (from file timestamps and your confirmation):** A accepted at ~18:07
IST and B at ~18:19, both in contest. C (19:40) and D (21:05) were finished after
the contest ended at 19:10. E was never started (0-byte file), F and G have no
files.

**Where the time went:** not recorded.

**What to drill:**
1. **E end to end, as a timed exercise.** It needs no new technique — prefix sums
   on a cycle + one Dijkstra or two sweeps — and is the highest-value unattempted
   problem here. Aim for under 25 minutes.
2. **The offline sweep + Fenwick pattern** (§2.6) — second contest in a row
   (ABC 475 E, now F) where it was the key. Write F from §2.5's identity without
   looking at §3.
3. **Mo's algorithm on an array first** (distinct values in a range is the
   standard warm-up), then G. Build the Euler-tour adapter (§2.7) as a separate,
   reusable piece.

**Open:** E, F, G unsolved in code. §3 gives a verified route and full code for
each; E first.
