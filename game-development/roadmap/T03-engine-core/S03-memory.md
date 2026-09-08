# T03.S03 — Memory: Allocators & Ownership
**Topic:** [T03 — The Game Loop & Engine Core](README.md) | **Depends on:** —
**Status:** expanded | **Assignments:** A007 | **Est. theory time:** 2 sessions

> The question this subtopic answers: *why do games not just use `new`,
> and what do they use instead?*
>
> This is where the real-time constraint stops being an abstraction. A
> general-purpose allocator is a shared, variable-latency, cache-hostile
> data structure, and every one of those adjectives is a problem when you
> have 16.67 ms.

---

## Theory

### U01 — What's wrong with `new` and `malloc`
**What it is.** The specific costs of a general-purpose allocator, in a
context that cares.

**Why it exists.** Because "allocation is slow" is folklore until you know
*which* property is expensive, and the right replacement depends on that.

**The mechanism.** Four distinct problems, only one of which is speed:

1. **Unpredictable latency.** `malloc` usually takes tens of nanoseconds
   and occasionally takes microseconds — when it needs a new arena from
   the OS, when it coalesces free blocks, when it takes a contended lock.
   For a soft-real-time system the tail is what matters (`T01.S02.U04`),
   and the tail here is unbounded in principle.
2. **Fragmentation.** Interleaved allocations and frees of varying sizes
   leave holes. Over hours of runtime, a 4 GB heap may be unable to serve
   a 1 MB request. On PC this is survivable; on a fixed-memory console it
   is a crash, which is why console-targeted engines are so strict here.
3. **Cache behaviour.** `malloc` gives you a pointer, not a layout. Objects
   allocated together in time are not necessarily together in space, and
   iterating a linked structure of them is a pointer chase — a cache miss
   per element, at ~200–300 cycles each. This is usually the largest cost
   and it is invisible in a profile of `malloc` itself, because the time
   is spent in *your* loop, waiting on memory.
4. **Thread contention.** A shared heap needs synchronization. Modern
   allocators use per-thread caches to avoid most of it, but the cross-
   thread free path still costs.

Plus a fifth, specific to this roadmap: `malloc`'s hidden metadata and
alignment behaviour make it a poor base for SIMD (`T13.S04`) and for GPU
upload buffers.

**The consequence, stated as the design rule of this subtopic:** allocation
in games is not "get me some memory", it is **"get me memory with a known
lifetime and a known layout"**. Once lifetime and layout are explicit, the
allocator can be trivial — which is why the allocators below are 30 lines
each and beat `malloc` by an order of magnitude.

**Where it bites.** Allocating per-frame temporaries with `new`. A
`std::vector` built and destroyed inside the frame loop is a malloc/free
pair per frame per site, and there are hundreds of such sites.

**Read.**
- `[GEA3]` ch. 6.2 — memory management. The whole section; it is the source for this subtopic.
- `[DOD]` ch. 2 — https://www.dataorienteddesign.com/dodbook/node3.html — the cache-behaviour argument.

**Check yourself.**
1. Which of `malloc`'s four costs is usually the largest in a game, and why is it invisible when profiling `malloc`?
2. Why is fragmentation a crash on console and a nuisance on PC?
3. Restate the design rule of this unit in one sentence.

### U02 — The linear/arena allocator
**What it is.** A block of memory and a bump pointer. Allocation is
`ptr += size`. There is no individual free — you reset the whole thing.

**Why it exists.** It is the simplest allocator and it covers the single
most common game pattern: **temporary allocations with a shared lifetime**
(everything allocated this frame dies at the end of this frame).

**The mechanism.**
```cpp
struct Arena {
    uint8_t* base;
    size_t   size;
    size_t   used;

    void* alloc(size_t bytes, size_t align) {
        size_t p = align_up((size_t)base + used, align) - (size_t)base;
        if (p + bytes > size) return nullptr;      // or grow, or assert
        used = p + bytes;
        return base + p;
    }
    void reset() { used = 0; }                     // frees everything, O(1)
    // scoped save/restore for nested temporary scopes:
    size_t mark() const { return used; }
    void   release(size_t m) { used = m; }
};
```

Properties:
- **Allocation is ~3 instructions.** No search, no metadata, no locks.
- **Perfect locality.** Sequential allocations are adjacent in memory, so
  iterating them is a linear scan — the prefetcher's best case.
- **Free is O(1) for everything at once.**
- **Cannot free individually.** That is the trade, and it's why you need
  the other allocators too.

**The standard uses:**
- **Frame arena.** Reset at the top of every frame. All per-frame temporary
  data lives here: render command lists, culling results, string
  formatting, physics contact scratch. This one allocator removes most
  per-frame `malloc` traffic in a typical engine.
- **Double-buffered frame arena.** Two arenas, alternating; frame N's data
  survives into frame N+1, which you need for `T03.S01.U04`'s interpolation
  and for anything the GPU is still reading.
- **Level arena.** Reset on level unload. All level-lifetime data.
- **Scratch/mark-release.** `mark()` at the top of a function, `release()`
  at the end — a stack discipline for nested temporaries.

**Alignment.** `align_up(x, a) = (x + a - 1) & ~(a - 1)` for power-of-two
`a`. Required, not optional: SIMD types need 16 or 32 bytes, and
misaligned access is a fault on some architectures and a slowdown on x86.
Your arena's `alloc` must take alignment as a parameter; defaulting it to
`alignof(std::max_align_t)` is a reasonable default.

**Where it bites.** Holding a pointer into an arena past its reset. This
is a use-after-free with no allocator metadata to detect it, so it reads
as garbage data rather than crashing. Mitigations: fill freed regions with
`0xDEADBEEF` in debug builds, and — importantly — use ASan's manual
poisoning (`__asan_poison_memory_region` on reset) so the sanitizer can
catch it. `A007`'s rubric requires this, because a custom allocator
otherwise makes ASan blind to exactly the bugs it exists to find
(`T01.S03.U03`).

**Read.**
- `[GEA3]` §6.2.1 — stack-based and double-ended stack allocators.
- Ryan Fleury, "Untangling Lifetimes: The Arena Allocator" — https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator — the best modern argument for arenas as a general strategy.
- ASan manual poisoning — https://github.com/google/sanitizers/wiki/AddressSanitizerManualPoisoning

**Check yourself.**
1. Why is arena allocation ~3 instructions when `malloc` is tens of nanoseconds?
2. Write `align_up` for power-of-two alignment and explain the bit trick.
3. Why does a frame arena need double buffering in a loop with render interpolation?
4. What does a custom allocator do to ASan, and what's the fix?

### U03 — Pool and free-list allocators
**What it is.** A pool holds fixed-size blocks; allocation pops from a
free list, free pushes back. Constant time both ways.

**Why it exists.** Because arenas can't free individually, and a great
deal of game data is "many objects of the same type, created and destroyed
independently": bullets, particles, entities, components, audio voices,
network packets.

**The mechanism.** The elegant part is that the free list lives **inside
the free blocks themselves** — no separate storage:

```cpp
struct Pool {
    uint8_t* base;
    size_t   block_size;    // >= sizeof(void*)
    void*    free_head;

    void init(void* mem, size_t count, size_t bs) {
        base = (uint8_t*)mem; block_size = bs;
        free_head = base;
        for (size_t i = 0; i < count - 1; ++i)          // thread the list
            *(void**)(base + i*bs) = base + (i+1)*bs;
        *(void**)(base + (count-1)*bs) = nullptr;
    }
    void* alloc() {
        if (!free_head) return nullptr;
        void* p = free_head;
        free_head = *(void**)p;                          // next in list
        return p;
    }
    void free(void* p) {
        *(void**)p = free_head;                          // push front
        free_head = p;
    }
};
```

Properties:
- **O(1) alloc and free**, a handful of instructions each.
- **Zero fragmentation** — all blocks are the same size, so any free block
  serves any request.
- **Bounded memory** — you decide the count up front, which is a feature:
  it converts "we ran out of memory somewhere" into "we ran out of
  bullets", which is a designer-visible, testable limit.
- **Iteration is not automatically ordered or dense.** After many
  alloc/free cycles the free list is scattered, so newly allocated blocks
  are not adjacent. If you iterate a pool every frame, this matters — and
  the answer is to keep a separate dense array of live indices, which is
  the idea that leads directly to `T03.S04`'s sparse-set ECS.

**Variants worth knowing:** a **buddy allocator** (power-of-two blocks,
splitting and coalescing) when you need several size classes; a
**segregated free list** (a pool per size class) which is roughly what
general-purpose allocators do internally.

**Where it bites.** Pool exhaustion handled by returning null and not
checking. Decide the policy — assert in dev, recycle the oldest, or fail
gracefully — and make it explicit. "Silently stop spawning particles" is a
perfectly good policy; "dereference null" is not.

**Read.**
- `[GEA3]` §6.2.1 — pool allocators.
- `[GPP]` "Object Pool" — https://gameprogrammingpatterns.com/object-pool.html — including its discussion of the fragmentation and locality trade-offs.

**Check yourself.**
1. How does the free list avoid needing separate storage? What is the minimum block size?
2. Why does a pool have zero fragmentation?
3. Why does pool iteration order degrade over time, and what does that lead you toward?

### U04 — Alignment, layout, and the cache
**What it is.** The physical facts about memory access that determine
whether your data layout is fast or slow.

**Why it exists.** Because `T13.S03`'s data-oriented design is entirely
built on these facts, and because allocator design without them is
guesswork.

**The mechanism.**

**Cache lines.** Memory moves between RAM and cache in 64-byte lines
(on x86-64 and most ARM). Touching one byte pulls in 64. Consequences:
- **Locality is free performance.** If the next thing you need is in the
  same line, it costs nothing.
- **A 64-byte struct that you only read 4 bytes of wastes 94% of the
  bandwidth** you paid for.
- **False sharing** (`T13.S05`): two threads writing to different variables
  in the *same line* cause the line to ping-pong between cores. Fix: pad
  per-thread data to a cache line.

**Approximate latencies** (order of magnitude; the ratios are what matter):

| access | ~cycles |
|---|---:|
| L1 hit | 4 |
| L2 hit | 12 |
| L3 hit | 40 |
| main memory | 200–300 |
| TLB miss + page walk | +100s |

A main-memory miss costs roughly what 100 arithmetic operations cost. This
single ratio is why "reduce the number of operations" is often the wrong
optimization and "reduce the number of cache misses" is usually the right
one.

**Alignment.** A type must be at an address that is a multiple of its
alignment. `alignof(float) == 4`, `alignof(double) == 8`, SIMD types 16 or
32. Structs are padded so that arrays of them keep every element aligned:
```cpp
struct Bad  { char a; double b; char c; };   // 24 bytes: 7+7 bytes padding
struct Good { double b; char a; char c; };   // 16 bytes: 6 bytes padding
```
Ordering members from largest to smallest alignment is a free size
reduction, and smaller structs mean more per cache line.

**Prefetching.** Hardware prefetchers detect sequential and
constant-stride access and fetch ahead. They are very effective — which
means **a linear scan of an array is dramatically faster than a pointer
chase over the same data**, often 10x, even though both "touch N objects".
This is the mechanical basis of `T13.S03`'s entire argument.

**Where it bites.** `std::list`, `std::map`, and node-based containers in
hot loops — each node is a separate allocation, so iteration is a pointer
chase and the prefetcher cannot help. This is not a style objection; it is
a measurable order-of-magnitude difference, and `A007`'s benchmark asks
you to produce the number yourself.

**Read.**
- `[DOD]` ch. 2–3 — the cache argument at length.
- `[AGNERFOG]` "optimizing_cpp" §9 (memory access) and §7.2 (alignment).
- Ulrich Drepper, "What Every Programmer Should Know About Memory" — https://people.freebsd.org/~lstewart/articles/cpumemory.pdf — long; read §3 (CPU caches). Dated in specifics, correct in principles.

**Check yourself.**
1. How many cache lines does a 200-element array of 24-byte structs occupy? What if you shrink the struct to 16 bytes?
2. Why is iterating `std::vector<T>` typically much faster than `std::list<T>` with identical `T` and N?
3. What is false sharing and how do you prevent it?

### U05 — Handles vs pointers
**What it is.** Referring to objects by an index plus a generation counter,
rather than by address.

**Why it exists.** Pointers into game data have three problems: they
dangle when the object is destroyed, they break when the container
reallocates or compacts, and they are 8 bytes that must be patched on
serialization.

**The mechanism.**
```cpp
struct Handle {
    uint32_t index      : 24;   // slot in the pool: 16.7M objects
    uint32_t generation : 8;    // bumped on every free of that slot
};

struct Pool {
    std::vector<T>        items;
    std::vector<uint8_t>  generations;

    T* get(Handle h) {
        if (h.index >= items.size())            return nullptr;
        if (generations[h.index] != h.generation) return nullptr;  // stale!
        return &items[h.index];
    }
    void destroy(Handle h) {
        if (!get(h)) return;
        ++generations[h.index];                 // invalidates all old handles
        free_list.push_back(h.index);
    }
};
```

The **generation counter** is what makes this better than a bare index. A
bare index dangles exactly as badly as a pointer: destroy entity 5, spawn
a new one that reuses slot 5, and every stale reference now silently
points at the wrong entity. The generation makes that stale reference
*detectable* — `get()` returns null, at the point of use, deterministically.
That converts a class of "the missile followed the wrong target" bugs into
an explicit null check.

Other properties:
- **Stable across container growth and compaction.** The pool can move
  items around; handles keep working.
- **Small.** 4 bytes instead of 8, which matters when components store
  many references.
- **Serializable directly.** No pointer patching on save/load (`T06.S04`)
  or over the network (`T14.S03`).

**Generation width.** 8 bits wraps after 256 reuses of a slot, at which
point a very old handle could alias. For entities that live seconds this
is fine; 12–16 bits is the common compromise. State the choice.

**Where it bites.** Caching the raw pointer from `get()` across frames —
which reintroduces every problem the handle solved. The discipline is:
resolve the handle at point of use, every time. The resolve is an index
and a compare, which is cheap enough that there is no excuse.

**Read.**
- `[GEA3]` §16.3 and §15.5 — handles and object references in a real engine.
- Andre Weissflog, "Handles are the better pointers" — https://floooh.github.io/2018/06/17/handles-vs-pointers.html — the definitive short argument.

**Check yourself.**
1. What does a generation counter catch that a bare index doesn't? Write the failure scenario.
2. Why are handles better than pointers for serialization?
3. What is the risk of an 8-bit generation, and when does it matter?

### U06 — Ownership, lifetime, and the policy you write down
**What it is.** The set of decisions about who owns what, made once and
documented, rather than per-site.

**Why it exists.** Because the C++ default (RAII, `unique_ptr`,
`shared_ptr`) is a good general answer and a mediocre game-engine answer,
and departing from it deliberately requires knowing what you're replacing.

**The mechanism.** The policy this roadmap uses, stated as rules:

1. **Lifetime is a property of the system, not the object.** Most game
   objects belong to a phase: frame, level, session, application. Allocate
   from the arena or pool for that phase and free the whole phase at once.
   Individual object lifetime management is the exception, not the rule.
2. **`shared_ptr` is banned in the simulation.** Atomic refcount traffic,
   16-byte pointers, nondeterministic destruction order, and it makes the
   ownership question *unanswerable* by reading the code. Reference-counted
   *resources* (`T03.S05`) are a legitimate separate case, with an explicit
   refcount you can inspect.
3. **`unique_ptr` is fine for subsystems**, which are created once at
   startup and destroyed at shutdown. It is wrong for entities.
4. **Cross-object references are handles**, per U05. Never raw pointers
   stored across frames.
5. **RAII for scope-bound resources** — file handles, GPU resource
   bindings, profiler scopes, arena mark/release. This is what RAII is
   genuinely best at and there is no reason to avoid it.

**Shutdown order.** The thing everyone gets wrong. Subsystems must be
destroyed in reverse creation order, and any subsystem holding handles
into another must release them first. The symptom of getting it wrong is
crash-on-exit, which is easy to ignore ("the process is dying anyway") and
which is a real bug that will bite when you add a "return to main menu"
path that tears down and rebuilds without exiting. Get it right in `A007`
while it is three subsystems, not in `A023` while it is twenty.

**Where it bites.** Using `shared_ptr` "temporarily" during prototyping.
It spreads: once one interface takes a `shared_ptr`, its callers hold
them, and unwinding that later touches everything.

**Read.**
- `[GEA3]` ch. 5.2 and §15.4 — ownership models in engines.
- `[GPP]` "Component" and "Service Locator" — for how ownership is arranged around subsystems, and the caveats.

**Check yourself.**
1. Give three concrete reasons `shared_ptr` is a poor fit for game entities.
2. Why is crash-on-exit worth fixing?
3. When is RAII exactly the right tool in an engine that otherwise uses arenas?

---

## Implementation

| assignment | size | target rung | what it forces you to understand |
|---|:---:|:---:|---|
| [A007](../assignments/A007-allocators.md) | M | R3 | Arena, pool, handles, and the benchmark that tells you what any of it bought — with ASan poisoning so the safety net still works |

**Order and overlap.** `A007` covers `S03` and `S05` (resources) together,
because a resource manager is the natural first consumer of a pool plus
handles.

The benchmark is the part that matters most. `A007`'s R3 rung requires:
allocate and iterate 100k objects via (a) `new`/`delete` with a
`std::vector<T*>`, (b) `std::vector<T>`, (c) your pool with handles — and
report the numbers with the methodology from `T01.S04.U02`. The expected
result is a large gap between (a) and the others and a smaller one between
(b) and (c); what matters is that **you produce the numbers**, including
the case where the difference is smaller than the folklore suggests.

The arena from `A007` is used by every subsequent assignment: `A012`'s
render command list, `A016`'s broadphase pair buffer, `A030`'s skinning
matrix palette, `A039`'s asset loading, `A044`'s job system. The handle
type is used by `A008`'s ECS, `A020`'s entity references, and `A046`'s
network object ids.

**If you are short on time.** The arena (U02) and handles (U05) are the
two that everything downstream assumes. The pool (U03) is 30 lines once
you have the arena. U04's cache material can be read quickly now and
studied properly at `T13.S01` — but do run the benchmark, because the
number is more persuasive than the chapter.

---

## Discussion Log
<!-- Appended per study session. See ../CONVENTIONS.md §7. -->

## Open Questions

## Connections
- `T01.S03.U03` — the ASan interaction, which U02 resolves.
- `T01.S04` — the measurement methodology `A007`'s benchmark must follow.
- `T03.S04` — ECS, which is this subtopic's ideas applied to entity storage.
- `T03.S05` — resources, the first consumer of pools and handles.
- `T13.S01`, `T13.S03`, `T13.S06` — where U04's cache facts become the whole topic.
