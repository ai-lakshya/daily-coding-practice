<!-- Template for case-1: you solved it and it was accepted.
     Copy to <platform>/<same-stem-as-the-cpp>.md. Section order is fixed —
     see CONVENTIONS.md §5. Delete every HTML comment as you fill it in. -->

# <cq-NNN> — <Problem title>

| | |
|---|---|
| **Platform** | <platform> |
| **Link** | <url> |
| **Solution** | [`<slug>.cpp`](<slug>.cpp) |
| **Status** | solved |
| **Difficulty** | <platform's own rating, or a one-word estimate> |
| **Concepts** | <tag>, <tag> |
| **Documented** | <YYYY-MM-DD> |

---

## 1. Premise

<!-- The problem in your own words, in 3-6 lines: what you're given, what
     you must produce, and the constraint that actually shapes the solution
     (n <= 1e5 means O(n log n); n <= 20 means bitmask). State the
     constraints explicitly — they are half the problem statement. -->

**Constraints that matter:** <...>

**The reframing:** <the one sentence that turns the story into a known
problem — "this is a shortest path on a DAG where edges go i -> i+1, i+2">

---

## 2. Your solution

<!-- SOLVE_FIRST: the user already has an accepted solution. Review it
     before teaching anything. Be specific and quote their actual code.
     If the solution is genuinely clean, say so in two lines and move on —
     do not manufacture criticism to fill this section. -->

**What you did:** <the approach in 2-3 lines>

**Complexity:** time `O(...)`, space `O(...)`

**What holds up:**
- <...>

**What doesn't:**
- <bug, latent UB, wrong-by-luck reasoning, or a complexity that only
  passed because the constraints were generous — with the input that
  would break it>

**Idiomatic notes:**
- <C++-specific: the signed/unsigned comparison, the redundant copy, the
  `std::vector` that should be sized once, the `int` that should be
  `long long`. Small, concrete, and each one actionable.>

---

## 3. The optimal solution

<!-- Even when the user's solution IS optimal, this section still exists —
     say so explicitly and use it to present the clean canonical form. -->

**Idea:** <the core insight, stated before any code>

**Why it's correct:** <the argument — the exchange argument, the induction,
the invariant. Not "it works because the DP covers all cases".>

**Complexity:** time `O(...)`, space `O(...)` — and why that's the floor
for this problem (or what the real lower bound is if this isn't it).

```cpp
// Clean, compilable, and commented where the reasoning isn't obvious.
```

**Where it differs from yours:** <one short paragraph, or "it doesn't">

---

## 4. Core concepts

<!-- One subsection per concept. This is the section the directory exists
     for, so it gets the most room. Every concept needs at least one real
     linkable source — CONVENTIONS.md §5. Add each one to CONCEPTS.md. -->

### 4.1 <Concept name>

**What it is:** <the definition, stated precisely enough to be useful>

**Why it applies here:** <the specific feature of THIS problem that makes
this concept the right tool — the part that transfers to the next problem>

**The general pattern:**
<the reusable shape, with the recurrence / invariant / template written out>

**Where it breaks down:** <the neighbouring problem where this approach
fails, and what you'd reach for instead>

**Sources:**
- [<title>](<url>)

### 4.2 <Concept name>

<!-- ... -->

---

## 5. Takeaways

<!-- 2-4 bullets, each one a trigger you could recognise in a future
     problem. "Sort by end time when covering intervals with points" is a
     takeaway; "dynamic programming is useful" is not. -->

- <...>

## 6. Related

- <links to other questions in this directory, `concepts/` notes, or
  `docs/` write-ups that share a concept>
