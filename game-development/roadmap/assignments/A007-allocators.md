# A007 — Arena/Pool Allocators + Handle-Based Resources
**Topic:** T03.S03, T03.S05 | **Size:** M (1–3 days) | **Target rung:** R3
**Status:** todo
**Code:** `game-development/code/A007-allocators/`
**Reuses:** A006 (the loop and engine skeleton), A001 (the histogram, as a benchmark harness) | **Reused by:** A008, A012, A016, A030, A037, A039, A043, A044

## Goal
Own your memory. Afterwards you can allocate with known lifetime and known
layout, refer to objects safely without pointers, and — critically — you
will have **measured** what that bought rather than assumed it.

## Concepts exercised
| unit | first met in | why it recurs here |
|---|---|---|
| T03.S03.U01–U06 | here | Arena, pool, alignment, handles, ownership policy |
| T03.S05.U01, U04, U05 | here | Resource cache, subsystem lifetime, cvars |
| T01.S03.U03 | A002 | ASan poisoning — the custom-allocator blind spot |
| T01.S04.U02–U03 | A001 | The benchmark methodology, applied for real |

## Prerequisites
`A006`. This turns its ad-hoc allocations into an explicit memory system.

## Specification

### Part 1 — Allocators
1. **`Arena`** per `T03.S03.U02`: `alloc(size, align)`, `reset()`,
   `mark()`/`release()`. Correct alignment. Out-of-space policy stated
   and implemented (assert in dev, return null in release — or grow;
   choose and document).
2. **`Pool<T>`** per `T03.S03.U03`: intrusive free list, O(1) alloc/free,
   fixed capacity, exhaustion policy stated.
3. **ASan integration**: `__asan_poison_memory_region` on `reset()`,
   `release()`, and `Pool::free`; unpoison on alloc. Guarded by
   `#if defined(__SANITIZE_ADDRESS__) || __has_feature(address_sanitizer)`.
4. **Debug fill**: freed memory filled with a recognisable pattern
   (`0xDEADBEEF`) in dev builds.
5. **Instrumentation**: peak usage, current usage, allocation count per
   arena/pool, exposed as cvars so they show in the debug overlay.

### Part 2 — Handles
6. **`Handle<T>`** per `T03.S03.U05`: packed index + generation. Choose
   and document the bit split.
7. **`HandlePool<T>`**: `create()`, `destroy(h)`, `get(h)` returning
   `T*` (null if stale), `is_valid(h)`. Generation bumped on destroy.
8. A test that demonstrates the stale-handle catch: create, destroy,
   create again (reusing the slot), and assert the old handle's `get()`
   returns null.

### Part 3 — The resource cache
9. **`ResourceCache<T>`** per `T03.S05.U01`: `load(AssetId)` with dedup,
   refcounting, `release(h)`, and unload at zero.
10. `AssetId` as a 64-bit hash of the path, with a dev-only reverse map
    for debugging.
11. **Placeholder resources**: a magenta checkerboard texture and a
    silent sound, returned when a load fails, with a loud log line. The
    game must keep running.
12. Load two textures, one of them twice; assert one underlying object and
    a refcount of 2.

### Part 4 — Engine lifetime
13. An explicit `Engine` struct per `T03.S05.U04` with ordered
    `startup()`/`shutdown()`. At least: Memory → Files → Resources →
    Window → Renderer → Input → World.
14. A **shutdown-order test**: each subsystem asserts on destruction that
    its dependents are already gone. Deliberately reorder shutdown once,
    confirm the assert fires, then put it back.
15. Zero leaks and zero ASan reports on exit, with leak detection on.

### Part 5 — The measurement
16. **The benchmark.** Allocate, iterate, and free 100,000 objects of a
    32-byte type, three ways:
    - (a) `new`/`delete` into a `std::vector<T*>`
    - (b) `std::vector<T>`
    - (c) your `Pool<T>` + `HandlePool<T>`, iterating the dense array
    Report allocation time, iteration time (summing a field), and free
    time, using `A001`'s histogram and the methodology from
    `T01.S04.U02` — median of ≥5 runs, warm-up discarded, RelWithDebInfo,
    machine stated.
17. Repeat the iteration benchmark at N = 1,000 / 10,000 / 100,000 /
    1,000,000 and plot or tabulate. **The shape of that curve is the
    actual deliverable of this assignment.**

### Constraints
- Allocators are in `core/`, dependency-free, testable without a window.
- No `shared_ptr` anywhere in this assignment.
- Benchmarks in RelWithDebInfo or Release, never Debug.

### Explicitly out of scope
Thread safety (that's `A044`), a general-purpose allocator, virtual memory
tricks, streaming, the two-format asset pipeline (that's `A039`).

## Acceptance criteria
- [ ] Arena and pool tests pass, including alignment (allocate a 32-byte-aligned type and assert the address) and exhaustion.
- [ ] ASan **catches** a deliberate use-after-reset of arena memory. Write that test, watch it fire, keep it.
- [ ] Stale handle test passes.
- [ ] Dedup test: same asset loaded twice, one object, refcount 2.
- [ ] Placeholder test: delete an asset file, run the game, it keeps running with a magenta texture and a log line.
- [ ] Shutdown-order assert fires when order is wrong, and doesn't when it's right.
- [ ] Clean exit under ASan with `detect_leaks=1`.
- [ ] The benchmark table exists in `NOTES.md`, with machine, config, and methodology stated.

## Deliverables
- `core/memory/` — arena, pool, handles
- `core/resources/` — cache, asset ids, placeholders
- `code/A007-allocators/` — the engine skeleton and the benchmark harness
- `NOTES.md` — **the benchmark table and the curve**, plus your bit-split choice for handles and your out-of-space policies

## Design notes / hints
- Do the ASan poisoning early. Once your allocators are in use everywhere,
  a custom allocator without poisoning means ASan is blind to exactly the
  bugs it exists to catch (`T01.S03.U03`) — and you will not notice that
  it has gone quiet.
- The interesting benchmark result is usually the **iteration** row, not
  the allocation row. Allocation being faster is unsurprising; the pointer
  chase in (a) being 5–10x slower to iterate is the lesson, and it's the
  one that motivates `A008` and `T13.S03`.
- Expect (b) and (c) to be close. That is the correct result — `vector<T>`
  is already contiguous. What (c) buys over (b) is stable references and
  O(1) removal, not raw speed. Say that in your notes; it is the kind of
  honest finding the review is looking for.
- Handle bit split: 24/8 gives 16.7M objects and 256 generations. For
  entities that die frequently, 20/12 is often better. Justify yours.

## Cut list
1. The placeholder resources (log and return an invalid handle).
2. The dev-only AssetId reverse map.
3. The `mark()`/`release()` scoped arena API.
4. The N-scaling curve (keep the single 100k benchmark).
5. **Never cut: the ASan poisoning, or the benchmark.**

## Rubric

| rung | passes when |
|---|---|
| R0 | Builds clean, tests pass, acceptance criteria met |
| R1 | Alignment correct for over-aligned types. Exhaustion handled per the stated policy in both allocators. No leaks, no ASan reports, clean shutdown. The stale-handle and use-after-reset tests both fire when they should. |
| R2 | Allocators are dependency-free and testable in isolation. The resource cache uses the pool and handles rather than reimplementing them. `Engine`'s declaration order matches its startup order and the shutdown is provably reverse. No `shared_ptr`. The ownership policy from `T03.S03.U06` is written down in a header comment and actually followed. |
| R3 | The benchmark table exists with full methodology, median-of-5, and a stated variance. You can explain the shape of the N-curve in terms of cache sizes on your machine (`T03.S03.U04`). Peak memory per arena is instrumented and visible in the overlay. |
| R4 | See extension below |

### R4 extension
Add a **double-buffered frame arena** and use it in `A006`'s loop for the
interpolation state (`T03.S01.U04`) — frame N's transforms allocated from
arena A, frame N+1's from arena B, alternating, so the previous frame's
data is still valid for interpolation without a manual copy. Then measure
the difference against the explicit-copy version. This is the extension
because it requires understanding *why* the double buffer exists, not just
how to write one.

## Common pitfalls
- Forgetting alignment and getting away with it on x86 until you add a
  SIMD type in `T13.S04`.
- A pool whose block size is smaller than a pointer, so the intrusive free
  list corrupts the next block. Assert `block_size >= sizeof(void*)`.
- Caching `get(handle)`'s raw pointer across frames — reintroducing every
  problem handles solved.
- Benchmarking in Debug and reporting a 50x speedup that is entirely
  `std::vector`'s iterator debugging.
- Concluding from the benchmark that pools always beat vectors. Read the
  numbers, not the expectation.

## Review Log
<!-- One dated block per rung. See ../CONVENTIONS.md §4. -->

## Recall schedule
| due | rung | done | result |
|---|---|---|---|
