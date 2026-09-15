<!-- Template for a contest directory's single CONTEST.md.
     Copy to contests/<contest-slug>/CONTEST.md.
     A contest gets ONE doc covering every problem in the set — individual
     problems here do NOT get their own .md (CONVENTIONS.md §1).
     Delete every HTML comment as you fill it in. -->

# <ct-NN> — <Contest name>

| | |
|---|---|
| **Link** | <contest url> |
| **Date attempted** | <YYYY-MM-DD> |
| **Problems** | <n> (<A–G>) |
| **Solved in contest** | <n> — <letters> |
| **Solved after** | <n> — <letters> |
| **Unsolved** | <letters> |
| **Concepts** | <tag>, <tag> |

---

## 1. The set

<!-- One row per problem. Keep it to one line each — the depth goes in
     section 2. "Verdict" is how it went on the day, not the final state. -->

| # | problem | verdict | key concept | one-line idea |
|---|---------|---------|-------------|---------------|
| A | [<name>](<url>) | AC in contest | <tag> | <...> |
| B | [<name>](<url>) | AC after contest | <tag> | <...> |
| C | [<name>](<url>) | unsolved | <tag> | <...> |

**Solution files:** `a.cpp` … `<letter>.cpp` in this directory. Empty files
are problems not attempted.

---

## 2. Key concepts across the set

<!-- THE MAIN SECTION. Not a per-problem walkthrough — the concepts the set
     as a whole was testing, each explained properly and each tied back to
     the problems that used it. A concept used in two problems gets treated
     once, here, with both problems named.

     Every concept needs at least one real linkable source
     (CONVENTIONS.md §5). Add each one to CONCEPTS.md. -->

### 2.1 <Concept name> — used in <A, D>

**What it is:** <definition, stated precisely>

**How it works:** <the mechanism — recurrence, invariant, proof sketch>

**Complexity:** <time / space>

**How it showed up here:** <the specific form it took in each problem that
used it, and what varied between them>

**The recognisable signal:** <what in the statement should have made you
reach for this — the transferable part>

**Sources:**
- [<title>](<url>)

### 2.2 <Concept name> — used in <C>

<!-- ... -->

---

## 3. Key ideas that would have helped

<!-- The section that earns the doc. Concrete, contest-specific leverage —
     not general advice. Aim at what actually cost time or points on the
     day.

     Good: "D's n <= 18 was the whole hint — that ceiling only ever means
     bitmask DP or meet-in-the-middle, and you spent 20 minutes on greedy."
     Bad: "read the constraints carefully." -->

- **<the idea>** — <why it applied, which problem it would have unlocked,
  and roughly what it would have saved>
- <...>

## 4. Reading the constraints

<!-- A quick table mapping each problem's limits to what they were telling
     you. This is the single most reusable artefact of a contest post-mortem. -->

| # | limits | what that was signalling |
|---|--------|--------------------------|
| A | n <= 2e5 | O(n log n) at worst — sort or a sweep |
| D | n <= 18 | 2^n — bitmask DP or meet-in-the-middle |

## 5. Post-mortem

<!-- Honest and short. Only fill this in from what the user actually says
     about how the contest went — do not infer a narrative from the code. -->

**Where the time went:** <...>

**What to drill:** <2-3 concrete things, each pointing at a concept in
section 2 or a problem source to grind>

**Open:** <problems still unsolved and what's blocking each>
