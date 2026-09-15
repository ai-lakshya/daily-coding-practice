<!-- Template for case-2: you could NOT solve it.
     Copy to <platform>/<same-stem-as-the-cpp>.md.
     Section order is FIXED and differs from the solved template on purpose:
     CONCEPTS COME FIRST, the solution comes second, so you can close the
     file after section 2 and re-attempt the problem. Do not reorder.
     Delete every HTML comment as you fill it in. -->

# <cq-NNN> — <Problem title>

| | |
|---|---|
| **Platform** | <platform> |
| **Link** | <url> |
| **Solution file** | [`<slug>.cpp`](<slug>.cpp) |
| **Status** | unsolved |
| **Difficulty** | <platform's own rating, or a one-word estimate> |
| **Concepts** | <tag>, <tag> |
| **Documented** | <YYYY-MM-DD> |

---

## 1. Premise

<!-- Short. The point of this doc is sections 2 and 3, not a restatement. -->

<what you're given, what you must produce>

**Constraints that matter:** <...>

**Where you got stuck:** <what the user actually tried, if they said —
their dead end is the most useful thing in the file. Quote them. If they
didn't say, write "not recorded" rather than inventing a struggle.>

---

## 2. Concepts you need

<!-- THE FIRST DELIVERABLE. Everything required to solve this problem,
     explained from scratch, with no reference to the actual solution. The
     test: could someone read only this section, close the file, and have a
     fair shot at the problem? If a subsection leaks the answer, move it to
     section 3.

     One subsection per concept. Every concept needs at least one real
     linkable source — CONVENTIONS.md §5. Add each one to CONCEPTS.md. -->

### 2.1 <Concept name>

**What it is:** <definition, stated precisely>

**How it works:** <the mechanism — the recurrence, the invariant, the
amortized argument. Enough that the user could implement it cold.>

**Complexity:** <time / space, and what drives each>

**When to reach for it:** <the recognisable signal in a problem statement
that says "this one" — this is the part that transfers>

**Worked micro-example:** <a tiny instance, hand-traced, unrelated to this
problem so it doesn't spoil section 3>

**Common mistakes:** <the off-by-one, the overflow, the initialisation>

**Sources:**
- [<title>](<url>)

### 2.2 <Concept name>

<!-- ... -->

---

### Re-attempt checkpoint

> You now have every piece. Stop here and try the problem again before
> reading section 3 — that re-attempt is worth more than the solution is.

---

## 3. The solution

<!-- THE SECOND DELIVERABLE. Full detail. Assume the user has read
     section 2, so don't re-teach the concepts — apply them. -->

**The insight:** <the single observation that unlocks it, stated plainly.
If there's a moment where the problem "clicks", this is it.>

**How you'd get there:** <the derivation — what you'd notice first, what
that suggests, why the obvious approach fails. The reasoning path matters
more than the destination, because the destination doesn't transfer.>

**Why the naive approach fails:** <the approach most people try, its
complexity, and the specific constraint that kills it>

**Building the solution:**
1. <step, with the reasoning for it>
2. <...>

**Correctness argument:** <why this is right, not just why it passes>

**Complexity:** time `O(...)`, space `O(...)`

```cpp
// Complete, compilable, commented where the reasoning isn't obvious.
```

**Walkthrough on the sample:** <trace the code against the problem's own
first sample input, showing the state after each meaningful step>

**Edge cases the code handles:** <n = 1, all-equal values, overflow, ...>

---

## 4. Takeaways

<!-- 2-4 bullets, each a recognisable trigger for a future problem. -->

- <...>

## 5. Similar problems

<!-- Where to go for the reps — same pattern, so the concept sticks.
     Prefer problems that are already in context/problem-bank.md. -->

- [<problem>](<url>) — <what it shares, what it changes>

## 6. Related

- <links to other questions here, `concepts/` notes, or `docs/` write-ups>
