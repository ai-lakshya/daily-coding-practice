# Progress Log
| date | type | id | title | time_spent | self_rating | key_takeaway |
|------|------|----|-------|-----------:|:------------:|--------------|
| 2026-08-04 | dsa | lc150-0051 | Minimum Number of Arrows to Burst Balloons | | 3 | Interval point cover pattern (sort by end, sweep with one running boundary); solved correctly in one pass with a valid but less standard descending-start + fixed-anchor greedy |
| 2026-08-04 | concept | dsa-01 | Amortized analysis | | | Total cost over a worst-case sequence divided by n, not per-call worst case; dynamic array doubling gives O(1) amortized push_back despite occasional O(n) resizes because resizes happen at geometrically-spaced sizes |
| 2026-08-04 | concept | dsa-01 | Amortized analysis (follow-up) | | | Contrasted amortized (deterministic, sequence-based, no bad case) vs average-case (probabilistic, e.g. quicksort O(n^2) worst / O(n log n) average) analysis; worked all three amortized proof methods (aggregate, accounting $3-charge, potential Phi=2*size-capacity) on array doubling, all converging on the same constant, plus a second potential-method example (binary counter, Phi=1-bit count) |
| 2026-08-04 | dsa | lc150-0028 | Container With Most Water | | 4 | Two Pointers (inward-moving); correctly deduced greedy choice to discard shorter line |
