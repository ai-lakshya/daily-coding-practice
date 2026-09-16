# ct-01 — AtCoder Beginner Contest 475

| | |
|---|---|
| **Link** | https://atcoder.jp/contests/abc475/tasks |
| **Date attempted** | 2026-09-12 (live, A–B); C and D upsolved afterward — exact upsolve date not recorded |
| **Problems** | 7 (A–G) |
| **Solved in contest** | 2 — A, B |
| **Solved after** | 2 — C, D |
| **Unsolved** | E, F, G |
| **Concepts** | segmented sieve, canonical coin systems, range covering on a line, binary search with the STL |

---

## 1. The set

| # | problem | verdict | key concept | one-line idea |
|---|---------|---------|-------------|---------------|
| A | [mnclr](https://atcoder.jp/contests/abc475/tasks/abc475_a) | AC in contest | string building | insert `o` between every adjacent pair of characters in a length ≤10 string |
| B | [Change](https://atcoder.jp/contests/abc475/tasks/abc475_b) | AC in contest | canonical coin systems | change per purchase is `(1e6 - A_i) % 1000`, decomposed digit-wise into 1/10/100-yen coins and summed across purchases |
| C | [Walk the Line](https://atcoder.jp/contests/abc475/tasks/abc475_c) | AC after contest | range covering on a line | for every "go-left-first" boundary (and, symmetrically, every "go-right-first" boundary), binary-search the farthest reachable point in the other direction using prefix/suffix sums |
| D | [Alphametic Prime](https://atcoder.jp/contests/abc475/tasks/abc475_d) | AC after contest | segmented sieve | segmented-sieve every `\|S\|`-digit number, then test candidates smallest-first for a character↔digit bijection with `S` |
| E | [Quiz Competition: Qualifiers](https://atcoder.jp/contests/abc475/tasks/abc475_e) | unsolved | — | not attempted — link recorded, "solution idea" left blank, no code |
| F | [Rectangle Filling](https://atcoder.jp/contests/abc475/tasks/abc475_f) | unsolved | — | not attempted — empty placeholder |
| G | [Has Many Divisors](https://atcoder.jp/contests/abc475/tasks/abc475_g) | unsolved | — | not attempted — empty placeholder |

**Solution files:** `a.cpp`, `b.cpp`, `c.cpp`, `c2.cpp`, `d.cpp`, `e.cpp`, `f.cpp`, `g.cpp` in this directory.

A naming note worth recording: **C's actual, working solution lives in `c2.cpp`**, not `c.cpp`. `c.cpp` holds only a header comment enumerating three planned approaches (O(n²) brute force, O(n log n) binary search, O(n) two-pointer) and is reserved for the O(n) version later; `c2.cpp` is the O(n log n) approach that was implemented and is treated as C's solved entry here. `e.cpp`, `f.cpp`, `g.cpp` have no working code and contributed no verdict.

---

## 2. Key concepts across the set

### 2.1 Range covering on a line (prefix sums + binary search) — used in C

**What it is:** given `n` points on a line connected by weighted edges, a start point `S`, and a distance budget `L`, find the maximum number of distinct points reachable, where "reachable" means some walk from `S` (revisiting edges is allowed and costs distance each time) stays within total distance `L`.

**How it works:** the walk's optimal shape is always one of two forms — go to the left extreme `l` first, then to the right extreme `r` (retracing through `S`), or the mirror image. For a fixed interval `[l, r]` containing `S`:
- cost(left-first) = `2·dist(S, l) + dist(S, r)`
- cost(right-first) = `dist(S, l) + 2·dist(S, r)`

and the interval is reachable iff `min(cost(left-first), cost(right-first)) ≤ L`. Zig-zagging more than once never helps, since every extra direction change re-pays a segment already paid for without visiting anything new. Precomputing prefix sums `pre[i] = dist(town 0, town i)` and suffix sums `post[i] = dist(town i, town n-1)` turns `dist(i, j)` into an O(1) subtraction, and because all edge weights are positive, `pre` (and `post`) are strictly monotonic — which is exactly what makes binary-searching the farthest affordable endpoint valid.

**Complexity:** the implemented approach is O(n log n) — for every one of the `n` possible "first extreme" choices, binary search the farthest point reachable in the other direction. The O(n) refinement (reserved for `c.cpp`) replaces the binary search with a two-pointer sweep: as the first-extreme index moves monotonically outward, the farthest-reachable-in-the-other-direction index also moves monotonically, so it never needs to be re-searched from scratch.

**How it showed up here:** `leftCountFirst` fixes the left extreme `i` (looping `S-1` down to `0`, breaking as soon as `dist(S, i) > L`) and binary-searches the largest `curr` with `pre[curr] - pre[i] ≤ L - dist(S, i)`; `rightCountFirst` is the mirror image over `post`. The answer is `max` of the two functions' best results.

**The recognisable signal:** "single starting point, movement cost is the sum of edge weights traversed (revisits cost again), maximize the count of distinct nodes reached within a budget" is the shape that always decomposes into these two path orders. It shows up any time the graph is a line (or a tree, in a harder variant).

**Implementation notes (from reviewing `c2.cpp`):** the solution is correct, including a subtle point that's easy to get backwards — after moving to the first extreme, the code widens the result with `std::max(startPos, <binary-search result>)` rather than trusting the binary search alone. That guard matters: if the leftover budget after reaching `i` is too small to even re-pay the trip back to `S`, the binary search can return an index short of `S`, but `S` was already visited for free on the way out, so the true right boundary is never less than `S`. Two things worth tightening if this gets reused: the `if (left == minLeft) maxRight = startPos; else maxRight = std::max(startPos, left);` branch is redundant — `std::max(startPos, left)` alone produces the same value in both cases, since `minLeft < startPos` always holds inside `leftCountFirst` (mirrored in `rightCountFirst`); and the code mixes `size_t` (`pre.size()`), `ll`, and the sentinel `n` assigned into an `ll` — harmless at N ≤ 8000, but the signed/unsigned mix (`CONVENTIONS.md`'s "the `size_t`/`int` comparison" flag) is worth cleaning up before reusing this as a template for a larger-N problem. `std::midpoint` (used for the binary search's mid-point) is a C++20 addition specifically to avoid the classic `(low+high)/2` overflow bug.

**Sources:**
- [USACO Guide — Two Pointers](https://usaco.guide/silver/two-pointers) (the general technique the O(n) refinement targets)
- [cppreference — std::midpoint](https://en.cppreference.com/w/cpp/numeric/midpoint) (overflow-safe midpoint, used in the binary search)

**See also §2.4** — the hand-rolled binary search described above rewritten with `std::upper_bound` / `std::lower_bound` / `std::partition_point`, which also shows that `post` is entirely redundant with `pre` and that the `std::max(startPos, ...)` guard discussed here is an artefact of the search window rather than a real edge case.

### 2.2 Segmented sieve — used in D

**What it is:** a way to enumerate primes in an arbitrary range `[L, R]` without allocating a sieve array of size `R` from `0`. It only needs the numbers up to `√R` (composite or not) to mark multiples inside the window `[L, R]`.

**How it works:** allocate a boolean array of size `R - L + 1`, representing candidacy for each number in `[L, R]`. For each `i` from `2` to `√R`, mark every multiple of `i` inside `[L, R]` as composite, starting from `max(i·i, first multiple of i that is ≥ L)`. Whatever survives is prime. Sieving multiples of every `i` (not just prime `i`) is slightly wasteful but still correct, since a composite `i`'s multiples are already covered by its prime factors — with `√R ≤ ~3163` here, the waste is immaterial.

**Complexity:** O((R − L + 1) log log R + √R) — dominated by the marking pass over the window.

**How it showed up here:** `S` has length 1–7, so the candidate primes are restricted to exactly `|S|`-digit numbers: `L = 10^(|S|-1)`, `R = 10^|S| - 1`. Worst case (`|S| = 7`) that's a window of ~9×10⁶ numbers — small enough that a segmented sieve over it, followed by an O(|S|) bijection check per candidate prime (smallest-first), comfortably fits the time limit.

**The recognisable signal:** whenever a problem needs "all primes in `[L, R]`" and `R` is too large to sieve from `0` but `R − L` (or `√R`) is small, that's a segmented sieve, not a full sieve.

**Sources:**
- [CP-Algorithms — Sieve of Eratosthenes (§ Segmented Sieve / Find primes in range)](https://cp-algorithms.com/algebra/sieve-of-eratosthenes.html)

### 2.3 Canonical coin systems / digit decomposition — used in B

**What it is:** a coin system is *canonical* if the greedy algorithm (always take the largest denomination that fits) is provably optimal for every amount — not all systems are (e.g. some real-world denominations aren't).

**How it works:** a base-10 positional system with denominations `1, 10, 100, 1000, ...` is canonical, because ten of any denomination always equals exactly one of the next, so an optimal representation never uses ten or more of a smaller coin — which forces the optimal count of each denomination to be exactly that digit of the amount (own reasoning from the positional-notation definition, not stated explicitly on the source below, which only defines canonicity in general).

**Complexity:** O(1) per amount (or per digit, if you want the count broken out by denomination).

**How it showed up here:** each purchase's change is `(1000 - A_i % 1000) % 1000` — computed in the code as `(1_000_000 - A_i) % 1000` to dodge C++'s negative-modulo behavior, since `1e6` is a multiple of `1000`. Each purchase's change is independent of every other purchase's (Takahashi only ever pays with 1000-yen bills, never his accumulated coins), so the final coin counts are just the digit-wise sum, across all `N` purchases, of each purchase's change.

**The recognisable signal:** any "minimum coins for change" problem where the denominations are powers of a fixed base reduces to digit arithmetic — no DP needed, unlike change-making with arbitrary denominations.

**Sources:**
- [Wikipedia — Change-making problem (§ Canonical coin systems)](https://en.wikipedia.org/wiki/Change-making_problem)

### 2.4 Binary search with the STL — used in C

**What it is:** `<algorithm>`'s three binary-search primitives — `std::lower_bound`, `std::upper_bound`, `std::partition_point` — used in place of the hand-rolled loop in `c2.cpp`. All three answer the same question ("where does a monotone property flip?") and differ only in how the property is spelled: the first two compare each element against a fixed value, the third takes an arbitrary predicate.

**How it works:**

*Step 1 — isolate the element.* This is the enabling move, and it is algebra, not API knowledge. `lower_bound`/`upper_bound` never see the index, only the element, so everything that does not depend on the search variable must move to the right-hand side. `c2.cpp`'s predicate

```
pre[curr] - pre[minLeft] <= totalCostLocal      ->      pre[curr] <= pre[minLeft] + totalCostLocal
                                                        ^element^    ^-- constant for this iteration --^
```

If that isolation is awkward, or forming the right-hand constant risks overflow, that is precisely the signal to use `partition_point` instead.

*Step 2 — pick the boundary.* On an **ascending** range `v` and value `x`. Both functions return a *first*; the "last" forms are that minus one. Convert iterator to index with `- v.begin()`:

| want | call |
|---|---|
| **first** `i` with `v[i] >= x` | `lower_bound(b, e, x)` |
| **first** `i` with `v[i] > x` | `upper_bound(b, e, x)` |
| **last** `i` with `v[i] < x` | `lower_bound(b, e, x) - 1` |
| **last** `i` with `v[i] <= x` | `upper_bound(b, e, x) - 1` |

Both return `end()` when nothing qualifies and both can return `begin()`, so a `-1` is only safe when the first element is *provably* a match.

*Step 3 — descending ranges need a comparator.* A plain `lower_bound` on the non-increasing `post` array is **undefined behaviour**, not a wrong index: the range must be partitioned with respect to the comparison used. Either pass the order the range is actually sorted in — `std::lower_bound(b, e, x, std::greater<ll>{})`, which returns the first element *not greater than* `x` — or avoid the situation entirely (see "How it showed up here"). Argument order differs between the two and is a live trap: `lower_bound` calls `comp(*it, value)`, `upper_bound` calls `comp(value, *it)`.

*The general form.* `partition_point` returns the first element for which a predicate is **false**, given that the range is true-then-false throughout: *"all elements before `iter` satisfy `p`, while all elements starting from `iter` do not"*. So `pp` is the first false, `pp - 1` the last true, `pp == last` means all true, `pp == first` means all false. The law is that `p` must run **true-then-false, never false-then-true** — if the natural predicate reads backwards, negate it and the answer moves from `pp - 1` to `pp`. A non-monotone predicate is undefined behaviour, not a debuggable wrong answer. The other two are this function with the predicate baked in:

```cpp
std::lower_bound(b, e, x)  ==  std::partition_point(b, e, [&](auto&& v){ return v <  x; });
std::upper_bound(b, e, x)  ==  std::partition_point(b, e, [&](auto&& v){ return !(x < v); });
```

which is also how to re-derive which is which: `lower_bound` partitions on `< x`, so its first-false is the first `>= x`.

**Complexity:** O(log n) comparisons — or O(log n) *applications of the predicate* for `partition_point`, so if the predicate is itself an O(n) feasibility check the total is O(n log n). All three are O(log n) only on **random-access** iterators; on a `std::set` or `std::list` the comparisons stay logarithmic but the iterator increments are linear, so use the container's member `.lower_bound()` there. `std::views::iota` is random-access and safe.

**How it showed up here:** `c2.cpp`'s two hand-rolled loops both collapse. Three findings came out of the rewrite, in increasing order of transferability:

1. **`post` is entirely redundant with `pre`.** For `i <= j`, `post[i] - post[j] == pre[j] - pre[i]` — both are `dist(i, j)`. Dropping `post` means only one ascending array is ever searched, so the descending-range problem and `std::greater` never arise. The mirror loop becomes `lower_bound(pre.begin(), pre.begin() + s + 1, pre[s] - budget)`.
2. **The `if (left == minLeft) ... else std::max(startPos, left)` guard was an artefact of searching too wide a window**, not a real edge case. It existed because the search range `[minLeft, n)` included indices left of `S`, so the search could return a point short of `S` even though `S` was already visited for free on the way out. Starting the range at `pre.begin() + s` makes the result `>= s` by construction and the `-1` provably in-bounds: `pre[s] <= pre[s] + budget` always holds, so the window's first element is always a match. **Constraining `[first, last)` to the indices the problem permits is what removes the edge case** — most binary-search off-by-ones are really "I searched a wider window than the problem allows."
3. **The cost accounting has to be chosen once and kept.** The rewrite charges `2 * dist(S, extreme)` up front and then only `dist(S, far end)`, which makes the leftover budget a clean per-iteration constant. `c2.cpp` instead charges one trip out and then the full walk from the extreme — same total, but the two are *not* interchangeable mid-loop, and mixing them silently triple-charges `dist(S, extreme)`. (This was hit for real while drafting the `std::greater` variant for this section; a stress test against an O(n²) brute force caught it.)

The resulting solution, verified against an O(n²) brute force and against `c2.cpp` on 4,500 random tests including the `A_i = 10^9` / `L = 10^18` extremes:

```cpp
ll best = 1;

// left extreme first, then as far right as the rest of the budget allows
for (ll i = s; i >= 0; --i) {
    ll spent = 2 * (pre[s] - pre[i]);        // out to i and back through S
    if (spent > l) break;                    // monotone in i -> safe to stop
    ll r = std::upper_bound(pre.begin() + s, pre.end(),
                            pre[s] + (l - spent)) - pre.begin() - 1;
    best = std::max(best, r - i + 1);
}

// mirror: right extreme first, then as far left as the rest allows
for (ll i = s; i < n; ++i) {
    ll spent = 2 * (pre[i] - pre[s]);
    if (spent > l) break;
    ll j = std::lower_bound(pre.begin(), pre.begin() + s + 1,
                            pre[s] - (l - spent)) - pre.begin();
    best = std::max(best, i - j + 1);
}
```

The `partition_point` form of the same two loops, which needs no algebra at all:

```cpp
const ll budget = l - spent;

// loop A: cost grows with the index -> "affordable" is already true-then-false
auto affordable   = [&](ll p) { return p - pre[s] <= budget; };
ll r = std::partition_point(pre.begin() + s, pre.end(), affordable) - pre.begin() - 1;

// loop B: walking left, the affordable towns are the ones NEAREST S, i.e. on the
// right of the window -> "affordable" would be false-then-true, so negate it and
// the answer moves from (pp - 1) to pp itself.
auto unaffordable = [&](ll p) { return pre[s] - p > budget; };
ll j = std::partition_point(pre.begin(), pre.begin() + s + 1, unaffordable) - pre.begin();
```

Loop B's in-bounds proof mirrors loop A's: the window *ends* at index `s` and `unaffordable(pre[s])` is `0 > budget`, i.e. false, so there is always at least one false element and `pp` is never the window's `end`.

**The recognisable signal:** any monotone "largest/smallest index that still fits a budget" is one of these three calls. Reach for `lower_bound`/`upper_bound` when the predicate genuinely is "element vs. a constant" — it is shorter and needs no lambda. Reach for `partition_point` when the predicate involves anything else. In particular **"binary search the answer"** — *minimum capacity to ship in D days*, *maximise the minimum distance*, *smallest radius covering everything* — has no array to call `lower_bound` on and is always one `partition_point`, usually over an index range rather than over data:

```cpp
auto days = std::views::iota(1LL, hi + 1);              // lazy, no allocation
ll ans = *std::ranges::partition_point(days, [&](ll d) { return !feasible(d); });
```

That form is the one that transfers; this problem's `upper_bound` is merely the tighter code for this problem.

**Pitfalls worth a reflex:**
- **Overflow moves to the right-hand side.** Isolating the element means building a constant like `pre[s] + (l - spent)`. Here `pre` maxes at `7999 * 10^9 ~ 8*10^12` against `L <= 10^18`, so the sum is ~`10^18` and safe in `long long` (max ~`9.2*10^18`) — but this is exactly what step 1 can break, so check it every time.
- **State why the predicate is monotone over *this* window** before writing the call. Here `pre` is strictly increasing because `A_i >= 1`; a zero weight would still be fine (non-decreasing), a negative one would invalidate the whole approach.
- **`pp == end()` is a normal case, not an error** — in loop A it means "everything is affordable" and the arithmetic handles it. Only guard it before dereferencing.
- Using `- v.begin()` yields a signed `ptrdiff_t`, which also clears the `size_t`/`ll` mixing flagged in §2.1.
- C++20 `std::ranges::lower_bound(pre, x)` takes the container directly and supports projections (`std::ranges::lower_bound(items, x, {}, &Item::weight)`). In `std::ranges` these are customization point *objects*, so unlike the classic function templates they can be bound to a variable and passed around.

**Sources:**
- [cppreference — std::lower_bound](https://en.cppreference.com/w/cpp/algorithm/lower_bound) — *"Iterator to the first element which is not ordered before `value`"*; comparator called as `comp(*iter, value)`
- [cppreference — std::upper_bound](https://en.cppreference.com/w/cpp/algorithm/upper_bound) — *"Iterator to the first element which is ordered after `value`"*; comparator called as `comp(value, *iter)`; *"If `ForwardIt` is not a `LegacyRandomAccessIterator`, the number of iterator increments is linear"*
- [cppreference — std::partition_point](https://en.cppreference.com/w/cpp/algorithm/partition_point) — *"all elements before `iter` satisfy `p`, while all elements starting from `iter` do not"*; O(log N) applications of `p`

---

## 3. Key ideas that would have helped

- **C's `N ≤ 8000` was a green light to bank an AC before optimizing.** `8000² = 6.4×10⁷` — the brute-force O(n²) approach noted as "approach 1" in `c.cpp`'s header comment would have passed comfortably inside the time limit. Going straight for O(n log n) (or the O(n) two-pointer target) is the right instinct for practice, but on the clock, submitting the O(n²) version first and refining afterward banks the points without the risk of debugging the trickier binary-search edge case (the `std::max(startPos, ...)` guard covered in §2.1) live.
- **D's `|S| ≤ 7` bounded the whole search space to ≤ 9×10⁶ numbers.** The constraint doesn't need a number-theoretic shortcut — it's small enough that "sieve the window, then brute-force-check each candidate" is the intended complexity, and recognizing that early (rather than looking for a smarter construction) is what unlocks the problem.
- **`L ≤ 10^18` in C is itself a signal**, independent of the algorithm: it rules out `int` for any distance/cost variable well before you even design the approach. The submitted code got this right throughout (`ll` everywhere), which is worth keeping as a reflex — check the largest bound in the statement before writing the first line.

## 4. Reading the constraints

| # | limits | what that was signalling |
|---|--------|---------------------------|
| A | `\|S\|` ≤ 10 | trivially small — any correct construction works, no complexity target |
| B | `N ≤ 1000`, `A_i ≤ 10^5` | O(N) simulation; values small enough for `int`, though the accumulated coin counts still fit comfortably |
| C | `N ≤ 8000`, `L ≤ 10^18` | `N` small enough for O(n²) or O(n log n); `L`'s huge bound demands `long long`/`int64_t` for every distance and budget variable |
| D | `\|S\|` 1–7 | candidate search space capped at ~9×10⁶ — segmented sieve + brute-force bijection check, not a closed-form construction |
| E | `1 ≤ M ≤ N ≤ 3×10^4`, `K ≤ 200`, `Q ≤ 5×10^4` | `K·Q = 10^7` is fine, but `N·Q ≈ 1.5×10^9` is not — each query needs to avoid re-simulating all `N` participants from scratch |
| F | `H·W ≤ 2×10^5` | total cell count bounded, not `H` and `W` independently — the intended approach is near-linear in `H·W`, not quadratic in the number of rectangles |
| G | `N, D ≤ 10^18`, ≤ 10 test cases | far too large to enumerate — needs a constructive, factorization-based approach to build a maximal-divisor number directly |

## 5. Post-mortem

**Where the time went:** not recorded.

**What to drill:**
- The segmented-sieve pattern from D (§2.2) — it's a clean, reusable primitive worth having ready without re-deriving the `max(i·i, first multiple ≥ L)` bound each time.
- ~~Replacing the hand-rolled binary search in `c2.cpp` with `std::upper_bound`/`std::lower_bound` directly on the `pre`/`post` arrays.~~ **Done 2026-09-16 — written up as §2.4.** Three things carried forward: `post` is redundant with `pre` (`post[i] - post[j] == pre[j] - pre[i]`), so only one ascending array ever needs searching; the `left == minLeft` branch was guarding a symptom of too wide a search window, not a real edge case; and `std::partition_point` — not `lower_bound` — is the form that generalises to "binary search the answer". `c2.cpp` itself is left untouched as the historical record.
- Still open from this line of work: the O(n) two-pointer version of C reserved for `c.cpp`. §2.4 removes the binary search's sharp edges but does not change the complexity class.
- A quick habit check: given a fresh problem, name the complexity class the constraints demand *before* picking an approach — C's `N ≤ 8000` supporting O(n²) is the concrete example from this contest.

**Open:** E, F, and G are all still unsolved.
- **E** needs per-query maintenance of the qualification state without re-simulating all `N` participants each time (`N·Q` is too large; `K·Q` is not).
- **F** needs a way to count the distinct grid states reachable by a single rectangle fill without materializing every rectangle (`H·W ≤ 2×10^5` bounds total cells, not the number of possible rectangles, which is much larger).
- **G** needs a constructive number-theory approach — building a candidate with the maximum divisor count from prime factors while avoiding multiples of `D` — since `N, D` up to `10^18` rule out any enumeration.
