# T03.S05 — Resources & the Engine Spine
**Topic:** [T03 — The Game Loop & Engine Core](README.md) | **Depends on:** T03.S03
**Status:** expanded | **Assignments:** A007 | **Est. theory time:** 1–2 sessions

> The question this subtopic answers: *how does an engine load, share,
> and eventually unload the textures, meshes, sounds, and levels it needs
> — and how do the subsystems that use them get created and destroyed in
> the right order?*
>
> This is the least glamorous subtopic in Phase A and the one whose
> absence is most expensive. Resource management and subsystem lifetime
> are both things that are cheap to get right at three subsystems and
> extremely expensive to fix at twenty.

---

## Theory

### U01 — The resource manager's job
**What it is.** One place that owns loaded assets, deduplicates them, and
hands out references.

**Why it exists.** Three problems that appear immediately:
- **Duplication.** Twenty enemies referencing `enemy.png` must share one
  GPU texture, not load twenty.
- **Lifetime.** When does that texture get freed? Not when one enemy dies.
- **Identity.** Two code paths asking for the same file must get the same
  object, or you get subtle bugs (a material edited via one reference and
  not the other).

**The mechanism.** The minimum viable design:

```cpp
template <typename T>
struct ResourceCache {
    std::unordered_map<AssetId, Handle<T>> by_id;   // dedup index
    Pool<T>                                storage; // T03.S03.U03
    std::vector<uint32_t>                  refcounts;

    Handle<T> load(AssetId id) {
        if (auto it = by_id.find(id); it != by_id.end()) {
            ++refcounts[it->second.index];
            return it->second;                       // already loaded
        }
        Handle<T> h = storage.alloc();
        load_from_disk(id, storage.get(h));
        by_id[id] = h;
        refcounts[h.index] = 1;
        return h;
    }
    void release(Handle<T> h) { if (--refcounts[h.index] == 0) unload(h); }
};
```

Design points:

- **AssetId is a hashed path, not a string.** A 64-bit hash of the
  canonical path, computed once. Strings in the runtime are memory,
  comparison cost, and a serialization hazard. Keep the string only in
  development builds for debugging (a hash→path table), and strip it in
  shipping. `T12.S01` develops this.
- **Handles, not pointers** (`T03.S03.U05`). The cache can move, reload, or
  replace a resource — which is precisely what hot reload (`A041`) needs —
  and handles survive that.
- **Reference counting here is legitimate**, unlike `shared_ptr` in the
  simulation (`T03.S03.U06`). The difference: the count is explicit,
  inspectable ("what is still holding this texture?"), non-atomic if
  loading is single-threaded, and scoped to one subsystem.

**Placeholder / fallback resources.** When a load fails, return a
deliberately obvious placeholder — the magenta-black checkerboard texture,
a cube mesh, a silent sound — rather than null or a crash. This keeps the
game running through a broken asset, and the placeholder is *visible*, so
the problem gets reported rather than silently ignored. Every shipped
engine does this and it is worth building on day one.

**Where it bites.** Not deduplicating, and discovering at `A023` that the
game loads the same 4 MB texture thirty times. Also: loading synchronously
in the middle of a frame, which is `U03`'s topic.

**Read.**
- `[GEA3]` ch. 6.2 (resource manager) — the definitive treatment; read §6.2.1 through §6.2.4.
- `[GPP]` "Flyweight" — https://gameprogrammingpatterns.com/flyweight.html — the sharing pattern, stated generally.

**Check yourself.**
1. Why hash asset paths rather than storing strings at runtime?
2. Why is refcounting acceptable here but not for gameplay entities?
3. What is a placeholder resource for, and why is magenta the traditional choice?

### U02 — Asset identity, dependencies, and the two-format model
**What it is.** The distinction between the format an artist produces and
the format the engine loads, and the graph of references between assets.

**Why it exists.** A `.png` must be decoded, possibly resized,
mip-generated, and block-compressed before the GPU can use it. Doing that
at load time costs seconds; doing it offline costs nothing at runtime.
`T12.S01` builds the pipeline — this unit is why it needs to exist.

**The mechanism.**

```
   source assets            baked assets              runtime
   (artist-facing)          (engine-facing)
   ---------------          --------------            -------
   hero.png       ---->     hero.tex     (BC7, mips)  ---> GPU upload,
   hero.gltf      ---->     hero.mesh    (packed)          ~memcpy
   theme.wav      ---->     theme.snd    (vorbis)
   level1.json    ---->     level1.lvl   (binary)
```

The runtime format should be loadable with **as close to zero parsing as
possible** — ideally read the file into memory, fix up a few offsets, and
use it in place. That is the design target, and the difference between it
and "parse JSON at load" is seconds per level.

**Dependencies form a graph.** A level references materials; a material
references textures and a shader; a mesh references a skeleton. Loading a
level means loading its transitive closure. Two things follow:
- The graph must be **acyclic**, or refcounting leaks. Detect cycles in the
  baker (`T12.S01`), not at runtime.
- Load order matters for referenced-resource fixup, and the standard
  answer is to load dependencies first (post-order traversal).

**Handling references inside baked data.** A baked material file
referencing a texture can't store a pointer. Options, in increasing
sophistication: store the AssetId and resolve on load (simple, a hash
lookup per reference); or store an index into a per-file dependency table
that is resolved once at load (faster, and what real engines do).

**Where it bites.** Loading source formats at runtime because it's easier
during development. It *is* easier, and it is worth keeping as a
development-only path — but if it's the only path, load times grow until
iteration becomes painful and then it's a big change. Design for both
paths from the start: a `load_source()` and a `load_baked()` behind one
interface.

**Read.**
- `[GEA3]` §6.1 (file system) and §6.2.2 (asset conditioning pipeline) — the two-format model, explained properly.
- `[GLTF]` — skim the spec's structure to see a real interchange format's shape.

**Check yourself.**
1. Why bake assets offline rather than converting at load time? Estimate the saving for a 100-texture level.
2. Why must the asset dependency graph be acyclic?
3. How do you represent a reference from one baked asset to another?

### U03 — Loading without stalling the frame
**What it is.** The problem that any disk read takes longer than a frame,
and the standard answers.

**Why it exists.** A 16.67 ms budget and a disk read that takes 5–50 ms.
Synchronous loading in the frame loop means a visible hitch, every time.

**The mechanism.** The options, in increasing order of capability:

1. **Load screens.** Everything loads while the frame loop shows a spinner.
   Simple, honest, and completely adequate for `A023`. The loop still runs
   (so the spinner animates and the window responds), but the simulation
   is paused and the accumulator reset afterward (`T03.S01.U03`).

2. **Background loading with a worker thread.** A request queue, a worker
   that reads and decodes, and a completion queue the main thread drains
   at a defined point in the frame.
   ```cpp
   Handle<Texture> h = cache.request(id);   // returns immediately with
                                            // a placeholder-backed handle
   // ... later frames: cache.pump(); resolves completed loads
   ```
   The critical constraint: **GPU resource creation usually must happen on
   the thread that owns the GL context.** So the worker does file I/O and
   CPU-side decode; the main thread does the upload. Splitting the work
   this way is the standard structure and it's worth getting right early.

3. **Streaming.** Load and unload based on proximity/prediction, with a
   memory budget. This is open-world technology; `T12.S01` touches it and
   you almost certainly do not need it.

**The hitch inventory.** Frame-time spikes have a small number of causes,
and knowing the list makes them diagnosable rather than mysterious:
asset loading, shader compilation (the big one on PC — a first-use shader
compile can take 100+ ms; precompile and cache), memory allocation growth,
GC-like batch frees, and the first-touch page faults of a large
allocation. Every one of them shows up as a p99.9 spike in `A001`'s
histogram.

**Where it bites.** Shader compilation specifically. It is not obviously
"loading", it happens the first time an effect is used (so it hits during
gameplay, not on the load screen), and it is a multi-frame stall. The
standard fix is to compile every shader variant at load time or ship a
precompiled cache — `T04.S04` and `T12.S01`.

**Read.**
- `[GEA3]` §6.2.5 — asset streaming and async loading.
- `[GEA3]` §8.6.3 — the frame-time spike discussion.

**Check yourself.**
1. Why must GPU resource creation happen on a specific thread, and how does that split the load work?
2. Name five distinct causes of a frame-time spike.
3. Why is shader compilation a worse hitch than texture loading, despite being smaller?

### U04 — Subsystem lifetime and startup order
**What it is.** The engine's own construction and destruction: which
subsystem starts first, and who tears down whom.

**Why it exists.** Subsystems depend on each other, so order is a
correctness requirement, and C++'s defaults don't give it to you.

**The mechanism.** The problem with the obvious approaches:

- **Global objects / singletons.** Construction order across translation
  units is *unspecified* in C++ — the "static initialization order
  fiasco". Two globals that depend on each other work or crash depending
  on link order. Destruction order is likewise unreliable.
- **Meyers singletons** (`static T& instance()`) fix construction order
  (lazy, on first use) but not destruction, and they hide dependencies:
  you cannot tell from the code what depends on what, which makes the
  order unauditable.

**The answer used by essentially every engine: explicit ordered
startup/shutdown.**

```cpp
struct Engine {
    Memory      memory;      // 1. everything else allocates from it
    JobSystem   jobs;        // 2. (later, T13.S05)
    FileSystem  files;       // 3.
    Resources   resources;   // 4. needs files + memory
    Window      window;      // 5.
    Renderer    renderer;    // 6. needs window + resources
    Audio       audio;       // 7. needs resources
    Input       input;       // 8. needs window
    World       world;       // 9. needs everything

    bool startup();          // in that order, checking each
    void shutdown();         // in EXACTLY REVERSE order
};
```

Rules that make this work:
- **Declaration order in the struct is the startup order.** C++ constructs
  members in declaration order and destroys them in reverse, so if the
  constructors do the work, the language enforces the ordering for you.
  (If startup can fail, use explicit `startup()`/`shutdown()` methods and
  write the reverse order by hand — but keep the declaration order
  matching, as documentation.)
- **Dependencies are constructor parameters or explicit references**, not
  global lookups. This makes the graph readable and makes the ordering
  auditable.
- **Shutdown is reverse order, always.** The renderer must release its GPU
  resources before the window (and GL context) is destroyed. The resource
  cache must be empty before the memory it allocated from is freed.

**Why crash-on-exit matters.** It is easy to dismiss — the process is
dying. Two reasons not to: (1) it is a real ordering bug, and the same bug
will fire on a "return to main menu" path that tears down and rebuilds;
(2) it masks leak detection, because the leak report never runs. Fix it in
`A007` with three subsystems.

**Where it bites.** The service-locator/singleton pattern, adopted because
passing the renderer to everything is tedious. It works, it hides
dependencies, and eighteen months later the startup order is a mystery
that nobody can safely change. `[GPP]`'s own Service Locator chapter is
notably ambivalent about it — read the caveats section.

**Read.**
- `[GEA3]` ch. 5.1 — subsystem start-up and shut-down. Read the whole section; it is short and it is exactly this problem.
- `[GPP]` "Service Locator" — https://gameprogrammingpatterns.com/service-locator.html — especially "Why might you not want to use it".

**Check yourself.**
1. What is the static initialization order fiasco, and why doesn't a Meyers singleton fully solve it?
2. Why must shutdown be exactly reverse order? Give a concrete pair.
3. Why is crash-on-exit worth fixing?

### U05 — Configuration and the engine's dials
**What it is.** Where the numbers live: resolution, volume, key bindings,
tuning constants, debug toggles.

**Why it exists.** Because recompiling to change a number is the single
biggest drag on iteration speed, and because `T06.S02`'s data-driven design
starts here.

**The mechanism.** Three tiers with different lifetimes and audiences:

| tier | example | format | who changes it |
|---|---|---|---|
| **Engine config** | resolution, vsync, audio device, memory budgets | file, loaded at startup | player (settings menu) |
| **Tuning constants** | jump height, enemy speed, camera lag | file, hot-reloadable | designer (you) |
| **Debug cvars** | `r_wireframe`, `p_showcolliders`, `ai_disable` | in-memory, console/ImGui | programmer, at runtime |

The **cvar** system (Quake's contribution, still the standard) is worth
building early: a registry of named variables with types, defaults, and
optional bounds, settable from a console or an ImGui panel.
```cpp
CVar<float> g_gravity{"p.gravity", -9.81f, "world gravity (m/s^2)"};
CVar<bool>  g_show_colliders{"p.showColliders", false};
```
Cost: ~100 lines. Benefit: every tuning value becomes adjustable without a
rebuild, and every debug visualization becomes toggleable. This is the
cheapest large improvement to iteration speed available in Phase A.

**Format.** For hand-edited config, use something a human can edit and a
parser can validate — TOML or INI for flat settings, JSON for structured
data. Do not invent a format. Do not use YAML (the spec is enormous and
its surprises are legendary). For baked runtime data, binary (`U02`).

**Where it bites.** Constants scattered as literals through the code.
`velocity.y = 400.0f` in one file and `if (velocity.y > 380.0f)` in
another, and they must agree. Every tuning value should be named and live
in one place, from the start.

**Read.**
- `[GEA3]` ch. 6.3 — the engine config and the "game world" data.
- Quake's cvar system — https://github.com/id-Software/Quake/blob/master/WinQuake/cvar.c — 200 lines, and it is where the whole idea comes from. Worth reading.
- `[GPP]` "Data Locality" is unrelated but "Bytecode"'s introduction has a good statement of why moving behaviour to data matters.

**Check yourself.**
1. What are the three tiers of configuration and how do their lifetimes differ?
2. What is a cvar and what does it cost to implement?
3. Why not YAML?

---

## Implementation

| assignment | size | target rung | what it forces you to understand |
|---|:---:|:---:|---|
| [A007](../assignments/A007-allocators.md) | M | R3 | A resource cache on top of your pool + handles, with dedup, refcounting, placeholders — and an engine struct whose shutdown is provably reverse-order |

**Order and overlap.** `A007` covers `S03` (memory) and `S05` (resources)
together because the resource cache is the pool allocator's first real
consumer, and building them together makes the handle design concrete.

Specific requirements this subtopic contributes to `A007`: the dedup test
(load the same asset twice, assert one underlying object and refcount 2),
the placeholder path (delete an asset file, assert the game keeps running
with a magenta texture), and a shutdown-order test (a subsystem that
asserts on destruction if its dependents are still alive).

Forward reuse: the cache is extended by `A012` (textures), `A030` (meshes
and skeletons), `A037` (audio clips), `A039` (the baked-format loader —
`U02`'s second path), and `A041` (hot reload, which is where the handle
indirection finally pays off completely). The cvar system from `U05` is
used by every subsequent assignment's debug toggles and is what `A024`'s
juice A/B test switches on.

**If you are short on time.** The cache with dedup and handles (U01) and
the explicit engine struct with ordered shutdown (U04) are the two that
matter now. Async loading (U03) can wait for `A023`; the two-format model
(U02) is `T12`'s job and only the *awareness* is needed now. Do build the
cvars (U05) — 100 lines, and it pays back within a week.

---

## Discussion Log
<!-- Appended per study session. See ../CONVENTIONS.md §7. -->

## Open Questions

## Connections
- `T03.S03.U03`, `U05` — the pool and handles this subtopic is built on.
- `T03.S03.U06` — the ownership policy, of which U01's refcounting is the sanctioned exception.
- `T03.S01.U03` — resetting the accumulator after a load, which U03 requires.
- `T06.S02` — data-driven design, which U05's tuning tier grows into.
- `T12.S01`, `T12.S04` — the asset pipeline and hot reload, which U02 and U01 are the runtime half of.
