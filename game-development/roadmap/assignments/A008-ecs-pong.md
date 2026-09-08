# A008 — Minimal ECS, and Pong Rebuilt On It
**Topic:** T03.S04 | **Size:** L (1–2 weeks) | **Target rung:** R4
**Status:** todo
**Code:** `game-development/code/A008-ecs-pong/`
**Reuses:** A006 (the loop, the game, the determinism test), A007 (pools, handles, arenas) | **Reused by:** A012, A017, A019, A020, A043, A044

> **M1 milestone.** Completing this means you have an engine core you
> wrote and understand end to end.

## Goal
Write a sparse-set ECS, port `A006`'s Pong onto it, and — the part that
distinguishes this from a tutorial — **measure what it bought**, at three
scales, and write down which architecture you would actually choose for a
real game of each size.

The honest finding is the deliverable. "At Pong's scale this bought
nothing measurable" is a *correct* result and reporting it is the point.

## Concepts exercised
| unit | first met in | why it recurs here |
|---|---|---|
| T03.S04.U01–U06 | here | Inheritance's failures, composition, AoS/SoA, sparse sets, systems, the honest counter-case |
| T03.S03.U04 | A007 | The cache facts, now producing a measurable difference |
| T03.S03.U05 | A007 | Entity ids *are* handles |
| T01.S04.U02–U03 | A001, A007 | The benchmark methodology, third application |
| T03.S01, T03.S02 | A006 | The loop and input, carried over unchanged |

## Prerequisites
`A006` and `A007`. You are porting `A006`'s game; keep it in the repo so
the two can be benchmarked against each other.

## Specification

### Part 1 — The ECS
1. **Entity** = `Handle` from `A007` (index + generation). `create()`,
   `destroy()`, `is_alive()`.
2. **`ComponentStore<T>`** as a sparse set per `T03.S04.U03`:
   `sparse` (entity index → dense index), `dense` (packed entity ids),
   `data` (packed components). O(1) `add`, `remove`, `has`, `get`.
3. **`Registry`**: type-erased storage of component stores, keyed by a
   stable type id. `add<T>`, `remove<T>`, `has<T>`, `get<T>` (returns
   null, never asserts).
4. **Views**: `view<Ts...>()` iterating the intersection, **iterating the
   smallest store first** (`T03.S04.U06`). Structured-binding friendly.
5. **A command buffer** per `T03.S04.U04`: deferred `create`, `destroy`,
   `add<T>`, `remove<T>`, flushed at a defined sync point. Systems must
   not mutate structure during iteration.
6. **Systems as an explicit ordered array** of functions. No scheduler, no
   dependency graph, no reflection.

### Part 2 — The port
7. Port `A006`'s Pong: `Transform`, `Velocity`, `Collider`, `Paddle`,
   `Ball`, `Score`, `Sprite` (or whatever your `A006` state was) as
   components; movement, collision, scoring, and AI as systems.
8. **The determinism test from `A006` must still pass**, unchanged in
   spirit: same recorded input at 30/60/144/1000 FPS, identical state
   hash at tick 600. This is harder than it sounds — component iteration
   order must be deterministic, which rules out iterating in hash-map
   order and constrains how the command buffer applies.
9. The AI opponent still drives through `TickInput`.

### Part 3 — The measurement
10. **A stress scene**: N entities with `Transform` + `Velocity`, updated
    by a movement system, at N = 100 / 10,000 / 100,000.
11. Benchmark **three architectures** on that identical workload:
    - (a) `A006`'s original approach (or a faithful OOP version: a
      `std::vector<GameObject*>` with `virtual Update()`)
    - (b) plain composition — per-type `std::vector<Component>` arrays,
      iterated directly
    - (c) your ECS view
    Methodology per `T01.S04.U02`: RelWithDebInfo, median of ≥5, warm-up
    discarded, machine and variance stated.
12. **A second benchmark** on the *entity-centric* workload
    (`T03.S04.U05`): for 1,000 random entities, read four components each
    and make a decision. This is the case where SoA is expected to lose —
    measure it and report it.
13. **The write-up** (`NOTES.md`), which is graded at R2:
    - the table for both benchmarks at all three scales
    - which architecture you'd choose for: Pong, a platformer with 200
      entities, a bullet-hell with 20,000 projectiles, an RTS with 5,000
      units — with the reason for each
    - what the ECS cost you in debuggability, concretely

### Constraints
- Sparse set, not archetypes. Understand archetypes well enough to explain
  the trade-off in `NOTES.md`; do not build them.
- No third-party ECS. Read EnTT's blog posts; write your own.
- Component stores allocate from `A007`'s arena/pool where sensible.
- Systems take `(Registry&, float dt, TickInput)` — no globals.

### Explicitly out of scope
Archetypes/chunks, parallel scheduling (`A044`), component reflection or
serialization (`A020`, `A039`), reactive/observer systems, hierarchies
(use a `Parent` component if you need one), and any attempt at a general
"engine".

## Acceptance criteria
- [ ] Pong plays identically to `A006`'s version — same feel, same rules.
- [ ] Determinism test passes at all four framerates on the ECS version.
- [ ] `destroy()` during a view iteration does not crash or skip entities (via the command buffer).
- [ ] Stale entity handle returns null from `get<T>()`.
- [ ] `view<A,B>()` iterates the smaller store — verify by instrumenting a counter and swapping which component is rarer.
- [ ] Both benchmark tables exist at all three scales with full methodology.
- [ ] The architecture recommendation write-up exists and takes a position on all four hypothetical games.

## Deliverables
- `core/ecs/` — the registry, stores, views, command buffer
- `code/A008-ecs-pong/` — the game and the stress benchmarks
- `NOTES.md` — **the two benchmark tables, the four recommendations, and the debuggability cost.** This file is the assignment's real output.

## Design notes / hints
- Read EnTT's "ECS back and forth" parts 1–4 before starting. Sparse sets
  are elegant and the article explains them better than any book.
- Start type-erased (a `std::vector<std::unique_ptr<IStore>>` indexed by a
  runtime type id). The fully compile-time-typed version is faster and
  much fiddlier; get it working first, then measure whether the
  metaprogramming is worth it. The measurement is more interesting than
  the template.
- Determinism will break the first time. The usual causes: iterating an
  `unordered_map` of stores, command buffer application order depending on
  insertion order in a hash container, and entity ids being reused in a
  different order. All three are `T01.S02.U05` in a new costume.
- For benchmark (a), write the OOP version honestly — a real
  `std::vector<GameObject*>` with objects allocated by `new` in a
  realistic interleaved order, not allocated contiguously by accident. A
  strawman benchmark teaches nothing.

## Cut list
1. The type-erased registry — hand-write three or four concrete stores.
2. Multi-type views — implement `view<T>()` and do the intersection
   manually at call sites.
3. The entity-centric benchmark (part 12).
4. The N=100,000 scale.
5. **Never cut: the determinism test, the command buffer, or the write-up.**

## Rubric

| rung | passes when |
|---|---|
| R0 | Builds clean, Pong plays, tests pass, acceptance criteria met |
| R1 | Determinism holds at all four framerates. Structural changes during iteration are safe. Stale handles are caught. `remove` on a non-existent component is a no-op, not UB. The sparse array handles entity ids beyond its current size (growth) correctly. |
| R2 | Sparse-set implementation is correct and clean — `dense` and `data` stay parallel through removals (swap-and-pop, and you can explain why it must be swap-and-pop). Views iterate the smallest store. The command buffer's flush point is explicit and documented. **`NOTES.md`'s architecture recommendations are specific and defended, not generic.** |
| R3 | Both benchmark tables complete at all scales, methodology stated. You can explain the crossover point (where ECS starts winning) in terms of your machine's cache sizes. The entity-centric benchmark shows ECS losing, and you report that rather than omitting it. |
| R4 | See extension below |

### R4 extension
Add **`std::vector<T>`-style SoA within a component**: split `Transform`
into parallel `positions`, `rotations`, `scales` arrays inside its store,
and add a system that touches only positions. Benchmark it against the
AoS `Transform` version at N=100,000, and report the bandwidth saved
(compute the theoretical saving from the struct sizes first, then measure
and compare the two numbers). Explaining the gap between predicted and
measured is the actual extension.

## Common pitfalls
- Removals that break the `dense`/`data` parallelism. Swap-and-pop must
  update the sparse entry of the *swapped-in* entity, and forgetting that
  is the classic sparse-set bug.
- Views that iterate the first named store rather than the smallest,
  making `view<Transform, RareThing>()` iterate every entity.
- Structural changes inside a system, discovered as an intermittent crash
  three assignments later.
- Building the ECS for two weeks and never porting Pong. The port is what
  makes it real.
- Reporting the benchmark you expected instead of the one you got.

## Review Log
<!-- One dated block per rung. See ../CONVENTIONS.md §4. -->

## Recall schedule
| due | rung | done | result |
|---|---|---|---|
