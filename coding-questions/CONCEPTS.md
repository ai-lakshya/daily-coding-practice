# Concept Index

Every concept explained in a `coding-questions/` write-up gets a row here,
and the row accumulates each question that used it. This is the file that
makes the directory worth more than the sum of its explanations: it turns
"I've seen this before" into a link.

Use it two ways:
- **Before writing an explanation** — if the concept already has a row,
  link to the best existing treatment and only write what *this* question
  adds, rather than explaining prefix sums for the fourth time.
- **When revising** — a concept with four questions attached and a shaky
  confidence rating is the thing to drill next.

## Concepts
| concept | best explanation | also used in | repo cross-refs | confidence |
|---------|------------------|--------------|-----------------|:----------:|
| range covering on a line | [ct-01 §2.1](contests/abc-475/CONTEST.md#21-range-covering-on-a-line-prefix-sums--binary-search--used-in-c) | | | |
| segmented sieve | [ct-01 §2.2](contests/abc-475/CONTEST.md#22-segmented-sieve--used-in-d) | | | |
| canonical coin systems | [ct-01 §2.3](contests/abc-475/CONTEST.md#23-canonical-coin-systems--digit-decomposition--used-in-b) | | | |
| binary search with the STL | [ct-01 §2.4](contests/abc-475/CONTEST.md#24-binary-search-with-the-stl--used-in-c) | cq-006, ct-04 | [cq-006 §2.2](contests/abc-475/e.md#22-partition_point-on-a-sub-range) — sub-range searching, the local precondition, predicate cost; [ct-04 §3 C](contests/abc-477/CONTEST.md#c--range-search-query) — by-value predicate parameter copies a vector per comparison | |
| prefix descent | [cq-006 §3.1](contests/abc-475/e.md#31-the-insight) | ct-01 | | |
| binary trie | [cq-006 §2.4](contests/abc-475/e.md#24-binary-trie) | ct-01 | [ct-01 §2.6](contests/abc-475/CONTEST.md#26-counting-keys-in-a-prefix-group-while-the-keys-change-trie-vs-offline-ranks--fenwick--used-in-e) — trie vs. offline Fenwick trade-off, measured | |
| fenwick tree | [cq-006 §2.5](contests/abc-475/e.md#25-fenwick-tree-binary-indexed-tree) | ct-01, ct-04 | [ct-04 §2.6](contests/abc-477/CONTEST.md#26-offline-sweep--fenwick-tree-for-sums-of-minv-x-over-an-index-range--used-in-f) — offline sweep over values, Fenwick over row index, for Σ min(v, x) | |
| coordinate compression | [cq-006 §2.6](contests/abc-475/e.md#26-coordinate-compression) | | | |
| offline query processing | [cq-006 §2.7](contests/abc-475/e.md#27-offline-query-processing) | ct-01, ct-04 | [ct-04 §2.6](contests/abc-477/CONTEST.md#26-offline-sweep--fenwick-tree-for-sums-of-minv-x-over-an-index-range--used-in-f) — sort queries by x to make one dimension monotone; [ct-04 §2.8](contests/abc-477/CONTEST.md#28-mos-algorithm--used-in-g) — Mo's as offline reordering | |
| bit packing | [cq-006 §2.8](contests/abc-475/e.md#28-bit-packing-into-machine-words) | | | |
| complexity accounting | [cq-006 §2.3](contests/abc-475/e.md#23-complexity-accounting-cost-the-whole-query-not-the-clever-part) | cq-005, cq-007, ct-01 | [cq-005 §2.7](leetcode/2265-tree.md#27-complexity-accounting-for-tree-recursions) — `sum of subtree sizes == sum of depths`, the `O(n*h)` identity | |
| postorder dfs | [cq-005 §2.2](leetcode/2265-tree.md#22-postorder-dfs) | | | |
| subtree aggregation | [cq-005 §2.3](leetcode/2265-tree.md#23-subtree-aggregation-and-the-redundancy-it-removes) | | | |
| multi-value returns in c++ | [cq-005 §2.4](leetcode/2265-tree.md#24-returning-several-values-from-a-recursion-in-c) | | | |
| integer division semantics | [cq-005 §2.5](leetcode/2265-tree.md#25-integer-division-and-floor) | | | |
| recursion depth | [cq-005 §2.6](leetcode/2265-tree.md#26-recursion-depth-on-trees) | | | |
| subarray counting | [cq-007 §2.1](leetcode/3524-array.md#21-subarray-counting) | | | |
| modular arithmetic | [cq-007 §2.2](leetcode/3524-array.md#22-modular-arithmetic-for-products) | | | |
| modular inverse | [cq-007 §2.3](leetcode/3524-array.md#23-why-the-prefix-product-trick-fails) | | | |
| rolling frequency over bounded states | [cq-007 §2.4](leetcode/3524-array.md#24-rolling-a-frequency-vector-over-a-bounded-state-set) | | | |
| overflow accounting | [cq-007 §2.5](leetcode/3524-array.md#25-overflow-accounting-on-the-answer-not-the-input) | ct-02, ct-03, ct-01, ct-04 | [ct-01 §3 F](contests/abc-475/CONTEST.md#f--rectangle-filling) — a rectangle count reaches ~10^10, so the *answer* needs `long long`; [ct-03 §2.9](contests/cf-1122/CONTEST.md#29-stdabs-and-64-bit-hygiene--used-in-b) — `std::abs` header discipline; [ct-02 §3](contests/abc-476/CONTEST.md#d--automat) — `X + K·Y ≈ 10^18` and `long long` prefix sums; [ct-04 §3 E/F](contests/abc-477/CONTEST.md#e--wheel-distance) — cycle length 2×10¹⁴, rectangle count 4×10¹⁰ | |
| monoid transitions | [cq-007 §2.6](leetcode/3524-array.md#26-transitions-as-a-monoid-why-this-generalises) | | | |
| rolling order statistics | [ct-02 §2.1](contests/abc-476/CONTEST.md#21-rolling-order-statistics-under-append--used-in-c) |  |  | |
| exchange argument | [ct-02 §2.2](contests/abc-476/CONTEST.md#22-one-sort-order-that-minimises-two-costs-at-once--used-in-d) | ct-03, ct-01 | [ct-03 §2.2](contests/cf-1122/CONTEST.md#22-all-or-nothing-an-exchange-argument-on-a-single-resource--used-in-b) — convexity, so only the endpoints need testing; [ct-01 §2.10](contests/abc-475/CONTEST.md#210-the-divisor-count-function-and-the-exchange-argument-on-exponents--used-in-g) — swapping prime exponents, and [§2.11](contests/abc-475/CONTEST.md#211-not-a-multiple-of-d-is-a-cap-on-one-primes-exponent--used-in-g) keeping it intact around one capped prime | |
| two pointers | [ct-02 §2.3](contests/abc-476/CONTEST.md#23-monotone-prefix-budgets-binary-search-then-two-pointers--used-in-d) | ct-01 | [ct-01 §2.1](contests/abc-475/CONTEST.md#21-range-covering-on-a-line-prefix-sums--binary-search--used-in-c) — the O(n) refinement of the same sweep | |
| segment tree | [ct-02 §2.4](contests/abc-476/CONTEST.md#24-a-segment-tree-that-returns-an-argmax-not-a-max--used-in-e) | ct-03 | [ct-03 §2.8](contests/cf-1122/CONTEST.md#28-prefix-and-suffix-minima-as-a-feasibility-characterisation--used-in-h) — lazy suffix-clear plus range max | |
| chebyshev-manhattan rotation | [ct-02 §2.5](contests/abc-476/CONTEST.md#25-chebyshev--manhattan-rotation--used-in-f) |  |  | |
| weighted absolute-difference sums | [ct-02 §2.6](contests/abc-476/CONTEST.md#26-weighted-sums-of-absolute-differences-in-o1--used-in-f) |  |  | |
| dilworth's theorem | [ct-02 §2.7](contests/abc-476/CONTEST.md#27-dilworths-theorem-minimum-chain-cover--maximum-antichain--used-in-g) |  |  | |
| aligned block decomposition | [ct-02 §2.8](contests/abc-476/CONTEST.md#28-aligned-block-decomposition-of-l-r--used-in-g) |  |  | |
| c++ stream i/o cost | [ct-02 §2.9](contests/abc-476/CONTEST.md#29-output-volume-endl-at-510-lines--used-in-c) |  |  | |
| bounding the complement | [ct-03 §2.1](contests/cf-1122/CONTEST.md#21-reading-the-answer-off-the-binding-constraint--used-in-a) |  |  | |
| split-point enumeration | [ct-03 §2.3](contests/cf-1122/CONTEST.md#23-split-point-scanning-with-prefix-and-suffix-counts--used-in-c) |  |  | |
| invariants under an operation | [ct-03 §2.4](contests/cf-1122/CONTEST.md#24-invariants-under-an-operation--used-in-d) |  |  | |
| dp over the divisor lattice | [ct-03 §2.5](contests/cf-1122/CONTEST.md#25-dp-over-the-divisor-lattice-and-the-smallest-prime-factor-sieve--used-in-e) |  |  | |
| smallest-prime-factor sieve | [ct-03 §2.5](contests/cf-1122/CONTEST.md#25-dp-over-the-divisor-lattice-and-the-smallest-prime-factor-sieve--used-in-e) |  | [ct-01 §2.2](contests/abc-475/CONTEST.md#22-segmented-sieve--used-in-d) — segmented sieve, the other sieve variant in this directory | |
| backward reasoning from the goal | [ct-03 §2.6](contests/cf-1122/CONTEST.md#26-backward-reasoning-plus-binary-search-on-a-monotone-predicate--used-in-f) |  |  | |
| binary search on the answer | [ct-03 §2.6](contests/cf-1122/CONTEST.md#26-backward-reasoning-plus-binary-search-on-a-monotone-predicate--used-in-f) |  | [ct-01 §2.4](contests/abc-475/CONTEST.md#24-binary-search-with-the-stl--used-in-c) — `partition_point` as the form that transfers when there is no array | |
| gcd-closure of reachable values | [ct-03 §2.7](contests/cf-1122/CONTEST.md#27-gcd-closure-of-reachable-values--used-in-g) |  |  | |
| prefix and suffix minima | [ct-03 §2.8](contests/cf-1122/CONTEST.md#28-prefix-and-suffix-minima-as-a-feasibility-characterisation--used-in-h) |  |  | |
| canonical representatives | [ct-01 §2.8](contests/abc-475/CONTEST.md#28-counting-distinct-outcomes-count-canonical-representatives-not-operations--used-in-f) |  |  | |
| fix two rows and sweep | [ct-01 §2.9](contests/abc-475/CONTEST.md#29-fix-two-rows-sweep-the-columns--and-square-the-short-side--used-in-f) |  |  | |
| sqrt from a product bound | [ct-01 §2.9](contests/abc-475/CONTEST.md#29-fix-two-rows-sweep-the-columns--and-square-the-short-side--used-in-f) |  |  | |
| divisor-count function | [ct-01 §2.10](contests/abc-475/CONTEST.md#210-the-divisor-count-function-and-the-exchange-argument-on-exponents--used-in-g) |  | [ct-03 §2.5](contests/cf-1122/CONTEST.md#25-dp-over-the-divisor-lattice-and-the-smallest-prime-factor-sieve--used-in-e) — the other divisor-structure problem in this directory | |
| per-prime exponent caps | [ct-01 §2.11](contests/abc-475/CONTEST.md#211-not-a-multiple-of-d-is-a-cap-on-one-primes-exponent--used-in-g) |  |  | |
| bounded candidate enumeration | [ct-01 §2.12](contests/abc-475/CONTEST.md#212-enumerate-a-small-structured-candidate-set-once-filter-per-query--used-in-g) |  |  | |
| nearest neighbour after sorting | [ct-04 §2.1](contests/abc-477/CONTEST.md#21-after-sorting-the-nearest-element-is-a-neighbour--used-in-b) |  |  | |
| pattern occurrence precompute | [ct-04 §2.2](contests/abc-477/CONTEST.md#22-precompute-every-pattern-occurrence-then-answer-windows-in-o1--used-in-c) |  |  | |
| lazy timestamps | [ct-04 §2.3](contests/abc-477/CONTEST.md#23-lazy-evaluation-with-timestamps-dont-apply-update-everything--used-in-d) |  |  | |
| shortest path through a hub | [ct-04 §2.4](contests/abc-477/CONTEST.md#24-shortest-paths-through-a-hub-split-on-whether-the-path-uses-it--used-in-e) |  |  | |
| interval intersection as prefix difference | [ct-04 §2.5](contests/abc-477/CONTEST.md#25-intersection-length-as-a-difference-of-prefix-counts--used-in-f) |  |  | |
| euler tour | [ct-04 §2.7](contests/abc-477/CONTEST.md#27-euler-tour--lca-a-tree-path-as-an-array-range--used-in-g) |  |  | |
| lca binary lifting | [ct-04 §2.7](contests/abc-477/CONTEST.md#27-euler-tour--lca-a-tree-path-as-an-array-range--used-in-g) |  |  | |
| mo's algorithm | [ct-04 §2.8](contests/abc-477/CONTEST.md#28-mos-algorithm--used-in-g) |  |  | |
| frequency of frequencies | [ct-04 §2.9](contests/abc-477/CONTEST.md#29-frequency-of-frequencies--used-in-g) |  |  | |

## Column notes
- **concept** — a short, stable, lowercase tag. Reuse an existing string
  rather than coining a near-duplicate (`0/1 knapsack`, not
  `knapsack (0/1)`).
- **best explanation** — the one write-up to re-read first. Usually where
  the concept was treated in most depth, not necessarily the first time it
  appeared.
- **also used in** — `cq-NNN` / `ct-NN` ids, not paths.
- **repo cross-refs** — deeper treatments elsewhere: `concepts/`,
  `system-design/`, `game-development/roadmap/`.
- **confidence** — 1–5, set by you, not by the agent. Blank until you say.
