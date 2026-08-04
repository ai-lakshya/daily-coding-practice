# Amortized Analysis
**Category:** DSA-theory | **Date:** 2026-08-04

## What it is
Amortized analysis measures the average cost per operation over a worst-case
*sequence* of operations, rather than the worst-case cost of any single
operation in isolation. An operation can occasionally be expensive — as long
as expensive operations are rare enough, the average cost across many calls
is still low.

## Why it matters
Several data structures in everyday use have occasional expensive operations
that "average out": dynamic arrays (`std::vector`, JS arrays) resizing, hash
table rehashing, splay tree rotations. Judging only by worst-case single-call
cost, you'd wrongly conclude `push_back` is O(n) and might avoid a perfectly
efficient structure, or over-engineer a workaround for a problem that
doesn't exist. It's also a common interview probe: "why is this O(1) if it
sometimes copies the whole array?"

## Core mechanics — the dynamic array example
When a dynamic array is full and you push one more element, it allocates a
new buffer at 2x capacity and copies everything over — that single call
costs O(n). But because capacity only doubles when needed, resizes happen at
sizes 1, 2, 4, 8, ..., n — a geometric series. Total copying work across all
resizes is `1+2+4+...+n < 2n`, so n insertions cost O(n) total -> O(1)
amortized per insertion.

```cpp
struct DynArray {
    int* data;
    int size = 0;
    int capacity = 1;
    DynArray() { data = new int[capacity]; }

    void push_back(int val) {
        if (size == capacity) {
            capacity *= 2;                   // double when full
            int* newData = new int[capacity];
            for (int i = 0; i < size; ++i)   // O(size) — the "expensive" step
                newData[i] = data[i];
            delete[] data;
            data = newData;
        }
        data[size++] = val;                  // O(1) amortized
    }
};
```

Three formal ways to prove an amortized bound:
- **Aggregate method** — sum total cost over n ops, divide by n.
- **Accounting method** — overcharge cheap ops to "prepay" for future
  expensive ones, maintain a non-negative credit balance.
- **Potential method** — define a potential function Φ over the structure's
  state; amortized cost = actual cost + ΔΦ.

## Common pitfalls
- **Amortized != average-case.** Amortized is a deterministic worst-case-
  sequence guarantee — any sequence of n operations costs O(n) total.
  Average-case involves probability over random inputs. These get conflated
  constantly.
- **A single amortized-O(1) call can still be O(n).** The exact call that
  triggers a resize *is* slow. The guarantee is about the total/average over
  a sequence, not any individual call — matters in real-time contexts where
  one slow call is unacceptable even if rare.
- **The growth factor matters.** Doubling gives O(1) amortized. Growing by a
  fixed increment (e.g. always +10) instead of a multiplicative factor gives
  O(n) amortized per insertion instead — resizes then happen every constant-k
  operations rather than at exponentially-spaced points.

## Check your understanding
If a dynamic array grows by adding a fixed 10 slots each time it's full
(instead of doubling), what's the amortized cost per `push_back` over n
insertions — still O(1), or something else? Why?

## Follow-up Discussion

### Q: How does average-case analysis differ from amortized analysis? Give an example.
**Example: Quicksort with a naive pivot (always the first element).**
- Worst case: O(n^2) — reliably triggered by sorted/reverse-sorted input,
  since every partition splits off only one element (n+(n-1)+...+1).
- Average case: O(n log n) — the *expected* cost assuming inputs are drawn
  uniformly at random from all permutations (or the pivot is randomized).

| | Amortized (dynamic array) | Average-case (quicksort) |
|---|---|---|
| What's bounded | Total cost of a *sequence* of ops on one structure | Expected cost of a *single* run, over an assumed input distribution |
| Assumption | None — no randomness | Assumes random/uniform inputs, or a randomized algorithm |
| Bad case exists? | No — every possible sequence of n push_backs costs O(n) total, provably | Yes — a sorted array reliably costs O(n^2), with certainty |
| What the guarantee means | "This sequence, whatever it is, costs O(n) total" | "Averaged over the assumed distribution — this specific run might not get that" |

Side note: this exact gap (average-case-fast, worst-case-terrible) is what
hash-flooding DoS attacks exploit against naive hash tables — average-case
O(1) lookup assumes well-distributed keys; an attacker who crafts colliding
keys forces every lookup to O(n), on purpose.

### Q: Explain the three proof methods in detail, with concrete examples.
All three were worked through on the same dynamic array (capacity doubling
from 1) example, and all three independently converge on the same
constant — a nice cross-check that they're three views of the same truth.

**Aggregate method** — sum total cost of n ops, divide by n.
- Resizes happen at sizes 1, 2, 4, 8, ... (~log2 n times). Total copy cost
  = 1+2+4+...+n ~= 2n (geometric series).
- Plus n individual insert-writes = n.
- Total ~= 3n -> amortized = 3n/n = **3 = O(1)**.

**Accounting method** — charge a fixed price per op, bank the overcharge as
credit, prove credit never goes negative.
- Charge $3 per push_back: $1 pays for inserting this element now; $1 is
  banked on this element to prepay its own future copy; $1 is banked to
  help pay for copying one *older* element at the next resize.
- Why the pairing works: a resize doubling K->2K copies exactly K old
  elements, and exactly K new elements were inserted since the *previous*
  resize — a clean 1-to-1 pairing between new insertions' "buddy dollar"
  and old elements needing a copy. This only holds because of the
  *doubling* structure — a fixed-increment growth strategy breaks it,
  which ties back to the pitfall noted above.

**Potential method** — define Phi(state) >= 0; amortized cost = actual
cost + Delta-Phi; requires Phi never drops below its starting value.
- Using Phi = 2*size - capacity on the same array:
  - Non-resizing insert: actual=1, Delta-Phi = 2(size+1)-2*size = 2 ->
    amortized = 1+2 = **3**.
  - Resizing insert (size=capacity=C -> C+1, capacity doubles to 2C):
    actual = C+1, Phi_before=C, Phi_after=2, Delta-Phi=2-C -> amortized =
    (C+1)+(2-C) = **3**.
  - Both cases land on exactly 3, matching the other two methods.
- **Second example (shows it's not array-specific): binary counter
  increment.** Flipping bits with carry (e.g. 0111->1000) costs O(log n)
  bits in the worst case for a single increment, but is O(1) amortized.
  Using Phi = number of 1-bits set: an increment flipping k ones->zero and
  one zero->one has actual cost k+1, Delta-Phi = 1-k, amortized =
  (k+1)+(1-k) = **2**, constant regardless of k. Each 1-bit acts like a
  prepaid token for its own eventual flip back to 0.
