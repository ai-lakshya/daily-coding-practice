# cq-007 — Find X Value of Array I

| | |
|---|---|
| **Platform** | leetcode |
| **Link** | https://leetcode.com/problems/find-x-value-of-array-i/ |
| **Solution file** | [`3524-array.cpp`](3524-array.cpp) |
| **Status** | unsolved |
| **Difficulty** | Medium |
| **Concepts** | subarray counting, modular arithmetic, modular inverse, rolling frequency over bounded states, overflow accounting, monoid transitions |
| **Documented** | 2026-09-21 |

---

## 1. Premise

You are given an array of positive integers `nums` and a positive integer
`k`. You perform **one** operation: remove a (possibly empty) prefix and a
(possibly empty, non-overlapping) suffix, leaving `nums` non-empty. For
each `x` in `[0, k-1]`, count the number of ways to do this such that the
product of the remaining elements leaves remainder `x` when divided by
`k`. Return those `k` counts.

**Constraints that matter:**

- `1 <= nums.length <= 10^5` — so `O(n^2)` (about `5 * 10^9` operations)
  is out, and `O(n)` or `O(n log n)` is in.
- `1 <= k <= 5` — *absurdly* small, and stated separately from `n`. When a
  problem gives you a second parameter with a tiny bound, the intended
  complexity almost always has that parameter as a factor: `O(n*k)`. This
  is the strongest hint in the statement.
- `1 <= nums[i] <= 10^9` — the product of even 3 elements overflows `int`,
  and the product of all of them has ~900,000 digits. You can never form
  the actual product.

**Where you got stuck:** not recorded — `3524-array.cpp` has an empty
`Solution idea:` header and an empty function body, so this is documented
from scratch rather than against an attempt.

---

## 2. Concepts you need

### 2.1 Subarray counting

**What it is:** the bridge between an operation described as *editing* an
array and a static object you can count.

**How it works:** removing a prefix `nums[0..l-1]` and a suffix
`nums[r+1..n-1]` leaves exactly `nums[l..r]`. The operation is therefore
in bijection with the choice of the pair `(l, r)` with `0 <= l <= r <= n-1`
— that is, with a **non-empty contiguous subarray**. "Possibly empty
prefix" is `l = 0`, "possibly empty suffix" is `r = n-1`. So the question
is not about operations at all:

> `result[x]` = the number of non-empty subarrays whose product is
> congruent to `x` modulo `k`.

Two consequences you get for free:

- There are exactly `n(n+1)/2` subarrays, so **the returned array must sum
  to `n(n+1)/2`**. That is a checksum you can assert in testing and verify
  by hand against every sample.
- Each subarray is counted once, so no inclusion-exclusion and no
  double-counting worry.

**Complexity:** the enumeration itself is `Θ(n^2)` subarrays, which
immediately tells you that any `O(n)`-ish solution must count in groups
rather than one at a time.

**When to reach for it:** any statement phrased as "remove a prefix and a
suffix", "keep a contiguous block", "choose `i <= j`". Translate to
subarrays before thinking about anything else. The reverse translation is
also worth knowing: "remove a prefix **or** a suffix (not both)" leaves a
suffix or a prefix, of which there are only `2n`, not `n^2`.

**Worked micro-example:** `n = 3`. Subarrays: `[0,0] [0,1] [0,2] [1,1]
[1,2] [2,2]` — six, and `3*4/2 = 6`. Sample 1 of this problem has `n = 5`
and output `[9,2,4]`, which sums to `15 = 5*6/2`. Sample 3 has `n = 5` and
output `[9,6]`, also 15. The checksum holds.

**Common mistakes:** counting the empty subarray. The statement requires
`nums` to stay non-empty, so `l <= r` strictly, and the total is
`n(n+1)/2`, not `n(n+1)/2 + 1`.

**Sources:**

- [CPH ch. 8 "Amortized analysis" — subarray techniques](https://cses.fi/book/book.pdf)
- [LeetCode 3524 statement](https://leetcode.com/problems/find-x-value-of-array-i/)

### 2.2 Modular arithmetic for products

**What it is:** the fact that reduction mod `k` commutes with multiplication:

```
(a * b) mod k  ==  ((a mod k) * (b mod k)) mod k
```

Formally, `Z -> Z_k` is a ring homomorphism; practically, it means you may
reduce **at every step** instead of at the end.

**How it works:** you never build the true product. You carry a single
number in `[0, k-1]` and multiply it by `nums[i] mod k`, reducing again
each time. The intermediate never exceeds `(k-1)^2 <= 16`, so an `int`
holds it with room to spare — even though the true product has hundreds of
thousands of digits.

This is the *only* reason the problem is tractable at all, and recognising
it is step one: the product is unrepresentable, the product **mod k** is a
number smaller than 5.

**Complexity:** `O(1)` per element, with no big-integer anything.

**When to reach for it:** the instant a statement mentions a product (or a
power, or a factorial) together with "modulo". If you catch yourself
reaching for `__int128` or a big-integer type in a problem that ends in
"mod k", you have missed this.

**Worked micro-example:** `k = 5`, `a = [7, 8, 9]`. True product `504`,
`504 mod 5 = 4`. Rolling: `1*7 = 7 -> 2`; `2*8 = 16 -> 1`; `1*9 = 9 -> 4`.
Same answer, never exceeding 16.

**Common mistakes:**

- Reducing only at the end (overflow, and here the value is unrepresentable
  regardless).
- Forgetting that `k` may be `1`. Then every remainder is `0`, and
  initialising a running product to `1` rather than `1 % k` leaves a stale
  `1` in a world where the only legal value is `0`.
- Assuming the intermediate fits without checking. Here it does because
  `k <= 5`; with `k <= 10^9` you would need `long long` for `a*b`.

**Sources:**

- [CP-Algorithms — Modular inverse (opens with the arithmetic of `Z_k`)](https://cp-algorithms.com/algebra/module-inverse.html)
- [CPH ch. 21 "Number theory"](https://cses.fi/book/book.pdf)

### 2.3 Why the prefix-product trick fails

**What it is:** the single most important negative result for this problem,
and the reason it is not a two-line exercise.

**How it works:** for *sums*, the standard subarray technique is prefix
sums: `sum(l..r) = P[r] - P[l-1]`, and mod `k` this lets you count
subarrays with a given remainder in one pass by bucketing prefix values
(this is LeetCode 974, "Subarray Sums Divisible by K"). It works because
**every element of `Z_k` has an additive inverse** — subtraction is always
defined mod `k`.

The multiplicative analogue would be `prod(l..r) = P[r] * P[l-1]^{-1} mod k`.
It fails, because `a` has a multiplicative inverse mod `k` **iff
`gcd(a, k) = 1`**. Two ways that breaks here:

- Any `nums[i]` divisible by a factor of `k` is non-invertible. If
  `nums[i] % k == 0`, then every prefix product from that point on is `0`
  and all information about earlier elements is destroyed — you cannot
  recover `prod(l..r)` from two prefix products.
- Even for invertible elements, `k` is not required to be prime, so
  `Z_k^*` is not all of `Z_k \ {0}`. With `k = 4`, the element `2` is
  non-zero and still non-invertible.

**When the trick *would* work:** `k` prime and no element divisible by `k`.
Then every prefix product is a unit and you can bucket
`P[r] * P[l-1]^{-1}` exactly like sums. Keep that as the boundary case —
recognising *why* the standard tool breaks is what points you at the
replacement.

**Worked micro-example:** `k = 4`, `nums = [2, 2]`. Prefix products mod 4:
`P[0] = 2`, `P[1] = 0`. The subarray `[1,1]` has product `2`, but there is
no `q` with `2 * q ≡ 0 (mod 4)` having a unique solution — `q = 0` and
`q = 2` both satisfy it. The map is not invertible, so the counting
argument collapses.

**Common mistakes:** writing the prefix-product solution, passing the
samples that happen to have no zero-divisors, and failing on a hidden test.
Sample 2 of this problem (`nums = [1,2,4,8,16,32], k = 4`) is specifically
constructed to contain them.

**Sources:**

- [CP-Algorithms — Modular Inverse (existence condition, `gcd(a,m) = 1`)](https://cp-algorithms.com/algebra/module-inverse.html)
- [LeetCode 974 — Subarray Sums Divisible by K](https://leetcode.com/problems/subarray-sums-divisible-by-k/) —
  the additive version where the trick *does* work

### 2.4 Rolling a frequency vector over a bounded state set

**What it is:** the technique the problem is actually testing, and a
general one. Statement of the pattern:

> You must evaluate `f(l..r)` for all `O(n^2)` subarrays, where `f` is
> built by folding an associative operation over the elements. Fix the
> right endpoint `r` and consider the multiset
> `S_r = { f(l..r) : 0 <= l <= r }`. If the number of **distinct values**
> in `S_r` is bounded by some small `m`, store `S_r` as a frequency table
> of size `m` instead of a list of `r+1` endpoints, and roll it forward
> from `S_r` to `S_{r+1}` in `O(m)`.

**How it works:** the roll has exactly two parts.

1. **Extend every existing subarray.** Every `l <= r` that had value `v`
   now has value `v ⊕ a[r+1]`. Because the operation is applied uniformly,
   you transform the *table* rather than the endpoints: for each distinct
   `v`, move its whole count to the bucket `v ⊕ a[r+1]`.
2. **Add the new singleton.** The subarray `[r+1, r+1]` did not exist
   before; add one to the bucket for `a[r+1]`.

Then `S_{r+1}` is complete, and you fold it into the running answer.

**Why the distinct count is bounded** varies by operation, and this is the
part that transfers:

| operation | bound on distinct values | why |
|---|---|---|
| product mod `k` | `k` | the value *is* an element of `Z_k` |
| bitwise OR / AND | `O(log V)` | extending only ever adds (OR) or removes (AND) bits, and each bit changes at most once, so the values form a chain |
| GCD | `O(log V)` | each change at least halves the value |
| sum | unbounded | which is why sums use prefix sums instead |

**Complexity:** `O(n * m)` time, `O(m)` space. You are replacing "iterate
over `l`" with "iterate over the value space", which is the whole idea.

**When to reach for it:** a subarray-counting problem where (a) the
quantity folds associatively along the array, and (b) the quantity lives
in a small universe or provably takes few distinct values. Signal in the
statement: a tiny second bound like `k <= 5`, or an operation that is
monotone in bits.

**Worked micro-example** (bitwise OR, so nothing about this problem is
spoiled). `a = [1, 2, 1]`, tracking `{OR value: count of left endpoints}`:

```
r = 0:  extend nothing; add singleton 1        -> {1: 1}         (1 subarray)
r = 1:  1|2 = 3, so {1:1} becomes {3:1};
        add singleton 2                        -> {3: 1, 2: 1}   (2 subarrays)
r = 2:  3|1 = 3 and 2|1 = 3, both land in 3;
        add singleton 1                        -> {3: 2, 1: 1}   (3 subarrays)
```

Total counted `1 + 2 + 3 = 6 = 3*4/2`. Never more than 2 distinct values
in the table, though there were 6 subarrays.

**Common mistakes:**

- **Updating the table in place.** The map `v -> v ⊕ a` is generally not a
  permutation — two source buckets can collide into one target — so
  writing into the table you are reading corrupts it. Build a fresh table
  (or a scratch buffer you clear) and swap.
- Forgetting the singleton, which drops exactly the `n` length-1 subarrays.
  The `n(n+1)/2` checksum catches this instantly.
- Adding the table into the answer *before* the roll, which counts
  subarrays ending at the previous index twice.

**Sources:**

- [CPH ch. 8 / ch. 21](https://cses.fi/book/book.pdf)
- [LeetCode 898 — Bitwise ORs of Subarrays](https://leetcode.com/problems/bitwise-ors-of-subarrays/) —
  the canonical `O(log V)`-distinct-values instance of the same pattern

### 2.5 Overflow accounting on the answer, not the input

**What it is:** the reminder that counting problems overflow in the
*output*, and it is easy to miss when the inputs are small.

**How it works:** the counts here sum to `n(n+1)/2`. At `n = 10^5`, that
is `5,000,050,000` — about `5 * 10^9`, which exceeds the `int` maximum of
`2,147,483,647`. With `k = 1`, every one of those subarrays lands in
`result[0]`, so a single bucket genuinely reaches `5 * 10^9`.

That is why the signature is `vector<long long>`, and reading the
signature is how you were supposed to notice. **A return type of
`long long` in a counting problem is a message from the setter**: the
answer does not fit in 32 bits, and there is a test case that proves it.

**Complexity:** irrelevant — but a wrong answer either way.

**When to reach for it:** any time the answer is "number of pairs /
subarrays / subsequences" and `n` exceeds ~65,000, since `n^2/2` passes
`2^31` right around there.

**Common mistakes:** declaring the accumulator `int` and converting at the
end. The overflow already happened.

**Sources:**

- [cppreference — Fundamental types (integer ranges)](https://en.cppreference.com/w/cpp/language/types)

### 2.6 Transitions as a monoid (why this generalises)

**What it is:** the structural reason §2.4 works, and the door to the
harder sequel. Keep it short on a first pass and come back to it.

**How it works:** "product mod `k`" is a **monoid**: an associative binary
operation with an identity (`1 mod k`). Associativity is what allows a
subarray's value to be assembled from any split point, which is what lets
a segment tree merge two children.

For this problem, associativity is only used to roll left-to-right, and a
single pass is enough. But if the problem added *updates* — "set
`nums[i] = v`, then re-answer" — the rolling pass would be `O(n)` per
query and you would instead store, at each segment tree node, the full
`k × k` table "an incoming remainder `x` becomes `y` after this segment",
merging two children by composing their tables in `O(k^2)`. That is
precisely LeetCode 3525, *Find X Value of Array II*.

**When to reach for it:** whenever a fold over a subarray needs to support
point updates or arbitrary range queries. If the state space is tiny, the
"table of transitions" is itself a mergeable value.

**Common mistakes:** trying to make a *non*-associative fold mergeable.
The segment tree buys you nothing if the operation depends on order beyond
concatenation.

**Sources:**

- [CP-Algorithms — Segment Tree (merging arbitrary associative values)](https://cp-algorithms.com/data_structures/segment_tree.html)
- [LeetCode 3525 — Find X Value of Array II](https://leetcode.com/problems/find-x-value-of-array-ii/)

---

### Re-attempt checkpoint

> You now have every piece: the reframe (§2.1), the tool that makes the
> product representable (§2.2), the reason the obvious technique is
> unavailable (§2.3), the technique that replaces it (§2.4), and the type
> to count in (§2.5). Stop here, fill in `3524-array.cpp`, and only then
> read §3. The re-attempt is worth more than the solution — and the
> `n(n+1)/2` checksum lets you grade yourself without submitting.

---

## 3. The solution

**The insight:** for a fixed right endpoint, the `r+1` subarrays ending
there take at most `k` different values mod `k` — and `k <= 5`. So you
never track left endpoints; you track *how many* left endpoints currently
sit in each of the `k` buckets, and extending the right endpoint by one
element permutes-and-merges those buckets in `O(k)`.

**How you would get there:** write the `O(n^2)` double loop first. Look at
what the inner loop over `l` actually produces for a fixed `r`: a stream of
`r+1` values that are all in `[0, k-1]`. That is at most 5 distinct
values being recomputed up to 100,000 times. The moment you phrase it as
"these `r+1` numbers only take 5 values", the compression is forced —
store the counts, not the numbers.

**Why the naive approaches fail:**

- Forming the product: unrepresentable (§2.2).
- Prefix products with modular division: no inverse mod a composite `k`,
  and `0` destroys information (§2.3).
- The `O(n^2)` double loop: `~5 * 10^9` iterations at `n = 10^5` (§3.1).

### 3.1 Brute force — enumerate every subarray

Correct, and the thing to diff against.

```cpp
class Solution {
public:
    std::vector<long long> resultArray(std::vector<int>& nums, int k) {
        int n = nums.size();
        std::vector<long long> result(k, 0);

        for (int l = 0; l < n; ++l) {
            int product = 1 % k;                       // 1 % k, not 1 — k can be 1
            for (int r = l; r < n; ++r) {
                product = (product * (nums[r] % k)) % k;  // < 25, no overflow
                ++result[product];
            }
        }
        return result;
    }
};
```

**Complexity:** time `O(n^2)`, space `O(k)`.

**What kills it:** `n(n+1)/2 ≈ 5 * 10^9` iterations. At a generous `10^9`
simple operations per second that is several seconds of pure arithmetic —
a hard TLE. Note it is not the modular arithmetic that is slow; it is that
the same 5 values are recomputed billions of times.

**Keep it.** At `n <= 2000` it is the right answer, and here it is your
oracle: run both on random small arrays and assert equality.

### 3.2 Optimal — roll a frequency vector over the `k` remainders

**Invariant.** After processing index `r`, let

```
cnt[v] = #{ l : 0 <= l <= r  and  prod(nums[l..r]) mod k == v }
```

**Roll from `r` to `r+1`,** with `a = nums[r+1] mod k`:

1. every subarray `[l..r]` with value `v` becomes `[l..r+1]` with value
   `(v * a) mod k` — so `next[(v*a) % k] += cnt[v]` for each `v`;
2. the new singleton `[r+1..r+1]` has value `a` — so `++next[a]`.

Then add all of `next` into `result`, because `next` now describes exactly
the subarrays whose right endpoint is `r+1`, and every subarray has exactly
one right endpoint.

```cpp
class Solution {
public:
    std::vector<long long> resultArray(std::vector<int>& nums, int k) {
        std::vector<long long> result(k, 0);
        std::vector<long long> cnt(k, 0);   // left endpoints, bucketed by product % k
        std::vector<long long> next(k, 0);  // scratch: writing into cnt would corrupt it

        for (int x : nums) {
            const int a = x % k;

            std::fill(next.begin(), next.end(), 0LL);
            for (int v = 0; v < k; ++v)
                if (cnt[v]) next[(v * a) % k] += cnt[v];   // 1. extend
            ++next[a];                                     // 2. the singleton

            cnt.swap(next);

            for (int v = 0; v < k; ++v) result[v] += cnt[v];
        }
        return result;
    }
};
```

**Correctness argument** — induction on `r`.

*Base.* Before the loop, `cnt` is all zero, correctly describing the empty
set of subarrays ending at index `-1`.

*Step.* Assume `cnt` satisfies the invariant for `r-1`. The subarrays
ending at `r` are exactly: `[l..r]` for `l <= r-1`, and `[r..r]`. The first
group is in bijection with the subarrays ending at `r-1` (drop the last
element), and by §2.2 the value of `[l..r]` is
`(value of [l..r-1] * a) mod k`, which is precisely what step 1 computes —
counts are added, not overwritten, so two source buckets colliding into one
target bucket sum correctly. Step 2 adds the one remaining subarray. So
`next` satisfies the invariant for `r`.

*Answer.* Every non-empty subarray has exactly one right endpoint, so
summing `cnt` into `result` once per index counts each subarray exactly
once. Hence `result[x]` is the number of subarrays with product `≡ x`,
which by §2.1 equals the number of legal operations.

**Complexity:** time `O(n*k)` — at most `5 * 10^5` bucket updates. Space
`O(k)`, i.e. constant. This is optimal up to the constant: you must read
all `n` elements, so `Ω(n)` is a lower bound, and `k <= 5` is a constant.

### 3.3 Walkthrough on the sample

`nums = [1,2,3,4,5]`, `k = 3`. Expected `[9,2,4]`.

| step | `x` | `a = x%3` | transition | `cnt` after | `result` after |
|---|---|---|---|---|---|
| r=0 | 1 | 1 | — ; `+1` at 1 | `[0,1,0]` | `[0,1,0]` |
| r=1 | 2 | 2 | `1→(1*2)%3=2` ; `+1` at 2 | `[0,0,2]` | `[0,1,2]` |
| r=2 | 3 | 0 | `2→(2*0)%3=0` ; `+1` at 0 | `[3,0,0]` | `[3,1,2]` |
| r=3 | 4 | 1 | `0→0` ; `+1` at 1 | `[3,1,0]` | `[6,2,2]` |
| r=4 | 5 | 2 | `0→0`, `1→2` ; `+1` at 2 | `[3,1,2]` | `[9,2,4]` |

`[9,2,4]` — matches, and `9+2+4 = 15 = 5*6/2`. Read the `r=2` row closely:
`nums[2] = 3` is divisible by `k`, so all three left endpoints collapse
into bucket `0` at once. That collapse is the information loss that
§2.3 said would break prefix products — here it is harmless, because the
table only ever moves forward.

Sample 3, `nums = [1,1,2,1,1], k = 2 -> [9,6]`: the odd-product subarrays
are exactly those avoiding index 2, i.e. the 3 subarrays inside `[0,1]`
plus the 3 inside `[3,4]`, giving `result[1] = 6` and `result[0] = 15-6 = 9`.
Worth checking by hand, since it verifies the `k = 2` behaviour without
tracing.

### 3.4 Implementation notes

- **`x % k` once per element.** `nums[i]` is up to `10^9`; reducing it
  before the inner loop keeps every subsequent multiplication tiny. With
  `v, a < k <= 5`, `v * a <= 16` — an `int` is ample, and no `long long`
  arithmetic is needed anywhere except the counters.
- **Do not update `cnt` in place.** `v -> (v*a) % k` is not injective when
  `gcd(a,k) > 1` (the `r=2` row above), so reading and writing the same
  array loses counts. The scratch-plus-swap costs `O(k)`.
- **Fixed-size storage.** Since `k <= 5`, `std::array<long long,5>` avoids
  the heap entirely and lets the compiler unroll the inner loop. A real
  micro-optimisation, not needed to pass.
- **`if (cnt[v])`** skips empty buckets. With `k <= 5` this is noise; the
  habit matters when the state space is larger.
- **`1 % k`, not `1`.** Only relevant in the brute force, where a running
  product is initialised to the multiplicative identity. In §3.2 there is
  no such initialisation, which is one small reason the rolling version is
  less error-prone than it looks.

### 3.5 Edge cases the code handles

- **`k = 1`** — every `a` is `0`, every product is `0`, and `result[0]`
  ends at `n(n+1)/2 ≈ 5 * 10^9`. This is the case that requires
  `long long` (§2.5), and the case a `std::vector<int>` accumulator fails.
- **`n = 1`** — one iteration, `cnt` becomes a single `1` in bucket
  `nums[0] % k`, `result` sums to `1`.
- **An element divisible by `k`** — bucket `0` is **absorbing**: once a
  subarray's product hits `0 mod k`, extending it keeps it at `0`, since
  `(0 * a) % k == 0`. The code needs no special case; the transition
  already expresses it. This is also a useful sanity check when debugging:
  after the first such element, `cnt[0]` can never decrease.
- **All elements coprime to `k`** — no bucket ever collapses, `result[0]`
  stays `0`, and the transition is a permutation of the buckets. This is
  the regime where the prefix-product approach of §2.3 would also have
  worked.
- **Large values** — `nums[i] = 10^9` is reduced to `< 5` immediately; no
  intermediate ever exceeds 16.

---

## 4. Takeaways

- **"Remove a prefix and a suffix" is "choose a subarray."** Do that
  rewrite before anything else, and use `n(n+1)/2` as a checksum on the
  answer — it caught a dropped singleton faster than any debugger would.
- **A tiny second bound (`k <= 5`) is the setter telling you the intended
  complexity has `k` in it.** When `n` is large and some other parameter is
  absurdly small, look for `O(n * small)`.
- **Prefix sums have a multiplicative analogue only when every element is
  invertible mod `k`.** `gcd(a,k) = 1` is the condition; a `0` mod `k`
  destroys the information the subtraction trick relies on.
- **When the fold's value space is small, count left endpoints per value
  instead of tracking them individually, and roll the table forward.** The
  same skeleton solves subarray OR / AND / GCD counting, where the bound on
  distinct values is `O(log V)` rather than `k`.
- **Read the return type.** `vector<long long>` on a counting problem is a
  statement that the answer exceeds `2^31`.

## 5. Similar problems

- [3525. Find X Value of Array II](https://leetcode.com/problems/find-x-value-of-array-ii/) —
  the direct sequel: same fold, plus point updates, solved by putting the
  `k × k` transition table in a segment tree (§2.6). Do this one next.
- [898. Bitwise ORs of Subarrays](https://leetcode.com/problems/bitwise-ors-of-subarrays/) —
  the same rolling-set pattern where the bound on distinct values comes
  from bit monotonicity instead of a modulus. The purest rep of §2.4.
- [2447. Number of Subarrays With GCD Equal to K](https://leetcode.com/problems/number-of-subarrays-with-gcd-equal-to-k/) —
  same skeleton again, `O(log V)` distinct values, and the counting
  question is phrased almost identically to this one.
- [974. Subarray Sums Divisible by K](https://leetcode.com/problems/subarray-sums-divisible-by-k/) —
  the additive contrast from §2.3. Solve it with prefix sums, then
  articulate exactly why that approach is unavailable here.
- [1521. Find a Value of a Mysterious Function Closest to Target](https://leetcode.com/problems/find-a-value-of-a-mysterious-function-closest-to-target/) —
  the same technique under a bitwise-AND fold, with an optimisation
  objective instead of a count.

## 6. Related

- [cq-005 — Count Nodes Equal to Average of Subtree](2265-tree.md) — the
  sibling write-up from this batch, and the same underlying move: write
  the brute force, find the quantity being recomputed, and carry a summary
  forward so it is computed once.
- [cq-006 §2.3 — complexity accounting](../contests/abc-475/e.md#23-complexity-accounting-cost-the-whole-query-not-the-clever-part)
  — the discipline that turns "`k <= 5`, so `O(n*k)`" from a guess into a
  budget you check before writing code.
