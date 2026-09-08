# T03.S04 — Data Layout & Entity Models
**Topic:** [T03 — The Game Loop & Engine Core](README.md) | **Depends on:** T03.S03, T02.S01
**Status:** expanded | **Assignments:** A008 | **Est. theory time:** 2–3 sessions

> The question this subtopic answers: *how do I represent "the things in
> my game", given that there will be thousands of them, they will each
> have a different arbitrary combination of properties, and I have to
> touch all of them every 16 ms?*
>
> This subtopic has more dogma attached to it than any other in the
> roadmap. The goal here is that you can make the argument in *both*
> directions and choose from measurement — which is why `A008` requires
> you to write down what your ECS actually bought, in numbers.

---

## Theory

### U01 — The inheritance model, and how it fails
**What it is.** The first answer everyone reaches for: a `GameObject` base
class, with `Player`, `Enemy`, `Projectile` derived from it.

**Why it exists.** It matches how you describe the game in English, it is
what OOP training suggests, and for a small game it genuinely works.

**The mechanism, and the three failures:**

```
        GameObject
       /    |     \
  Actor   Pickup   Trigger
   /  \
Player  Enemy
        /    \
  FlyingEnemy  ShootingEnemy
                    |
          FlyingShootingEnemy ???
```

1. **The diamond / combinatorial explosion.** You need a flying enemy and a
   shooting enemy, then a flying shooting enemy. Every new orthogonal
   capability doubles the leaf count. The hierarchy encodes an "is-a" tree
   over what is actually a set of independent "has-a" capabilities.

2. **The fat base class.** The pressure from (1) pushes functionality up
   into `GameObject`, which grows a `health`, a `velocity`, an
   `inventory`, and a `render_mesh` — most of which most objects don't
   use. Every object now pays memory for every feature any object has.

3. **`virtual void Update()` over a vector of base pointers** — the
   performance failure, and the one that motivates everything else:
   ```cpp
   for (GameObject* o : objects) o->Update(dt);
   ```
   Per object, this costs: a cache miss to load the pointer's target
   (they're scattered — `T03.S03.U04`), a cache miss on the vtable, an
   indirect call the branch predictor may miss, and then the actual work
   on data that's interleaved with data you don't need. The *work* is
   often a small fraction of the total.

The real-world evidence: essentially every engine that started with this
model (Unreal's actors, Unity's GameObjects, and their own internals)
has moved toward composition and, more recently, toward data-oriented
storage — not because inheritance is inelegant but because of (1) and (3)
specifically.

**Where it bites.** Failure (1) arrives at roughly 15 entity types, which
is about three weeks into a project — early enough to be worth avoiding
from the start.

**Read.**
- `[GPP]` "Components" — https://gameprogrammingpatterns.com/component.html — the failure and the fix, in the clearest available form. Read this first.
- `[GEA3]` §16.2 — runtime object model architectures, with the historical progression.

**Check yourself.**
1. Draw the hierarchy for: enemies that can fly, shoot, be invisible, or drop loot, in any combination. How many leaf classes?
2. Name the four costs of `virtual Update()` over scattered objects.
3. Why does the combinatorial problem push functionality into the base class?

### U02 — Composition: entities as bags of components
**What it is.** An entity has no type. It has a set of components, each
holding one aspect's data. Behaviour comes from what it has, not what it is.

**Why it exists.** It replaces the "is-a" tree with "has-a" sets, which
compose without explosion: N independent capabilities give 2^N
combinations with N component types.

**The mechanism.**

```cpp
struct Entity {
    Handle<Transform> transform;
    Handle<Sprite>    sprite;      // null handle = doesn't render
    Handle<Collider>  collider;
    Handle<Health>    health;
    // ...
};
```
or, more commonly, a component map / bitmask per entity plus separate
component storage.

This is the model Unity's `GameObject`/`MonoBehaviour` exposes, and it is
already a large improvement over U01: no explosion, no fat base class, and
entities become *data* (which makes `T06.S02`'s data-driven definitions
possible — an enemy is now a list of components with values, i.e. a file).

**What it does not fix:** the iteration cost. If components are heap
objects referenced by pointer, `for (auto& c : components) c->Update()` is
the same pointer chase as U01. Composition solves the *design* problem;
it does not by itself solve the *memory* problem.

**The intermediate step that most engines actually use:** components live
in per-type contiguous arrays, and systems iterate the array of one type.
```cpp
std::vector<Transform> transforms;   // contiguous, iterate linearly
std::vector<Sprite>    sprites;
```
Now a system that only needs `Transform` reads only transforms, densely.
This gets you most of the benefit of a "real" ECS with a fraction of the
machinery, and for a great many games it is the correct stopping point.

**Where it bites.** The "component that needs three other components"
problem. A movement system needs `Transform` and `Velocity`; getting both
for the same entity means a lookup, and how that lookup is structured is
exactly what U03's architectures differ on.

**Read.**
- `[GPP]` "Components" — again; the second half is this unit.
- `[GEA3]` §16.2.2 — component-based object models.

**Check yourself.**
1. How many component types for the four-capability enemy from U01, and how many combinations does that support?
2. What does composition fix, and what does it explicitly not fix?
3. Why does making entities pure data enable data-driven content?

### U03 — AoS vs SoA, and what ECS actually is
**What it is.** The memory-layout question, which is the real content of
"ECS".

**Why it exists.** Because the performance argument for ECS is entirely a
memory-layout argument, and stating it that way makes it evaluable.

**The mechanism.**

**Array of Structs (AoS)** — the natural layout:
```cpp
struct Particle { vec3 pos; vec3 vel; vec4 color; float life; };  // 44 bytes
std::vector<Particle> particles;
// updating only pos and vel:
for (auto& p : particles) p.pos += p.vel * dt;
```
Each 64-byte cache line holds ~1.45 particles, and of the 44 bytes loaded
per particle you use 24. **Roughly 45% of the bandwidth is wasted** on
`color` and `life` that this loop never touches.

**Struct of Arrays (SoA)** — one array per field:
```cpp
struct Particles {
    std::vector<vec3>  pos;
    std::vector<vec3>  vel;
    std::vector<vec4>  color;
    std::vector<float> life;
};
for (size_t i = 0; i < n; ++i) pos[i] += vel[i] * dt;
```
Now the loop touches only `pos` and `vel`, both densely packed. Every byte
loaded is used, the prefetcher sees two clean sequential streams, and the
loop is trivially vectorizable (`T13.S04`).

```
   AoS:  [pos|vel|col|life][pos|vel|col|life][pos|vel|col|life]
          ^^^^^^^^         ^^^^^^^^          ^^^^^^^^
          used             used              used     <- 45% waste

   SoA:  [pos][pos][pos][pos]...  [vel][vel][vel][vel]...
         all used                 all used
```

**That is the entire performance case for ECS.** An "archetype ECS" is a
system for automatically maintaining SoA storage grouped by component
combination, so that a system requesting `(Transform, Velocity)` gets
dense parallel arrays of exactly those.

**The two dominant architectures:**

**Archetype / chunk-based** (Unity DOTS, Flecs, EnTT's groups):
Entities with identical component sets are stored together in chunks, SoA
within the chunk. Iteration over a query is maximally dense — you walk
matching chunks and stride linearly.
- Best iteration performance.
- **Adding or removing a component moves the entity to a different
  archetype** — a structural change that copies its data. Cheap per event,
  expensive if you toggle a component every frame (a common accident).

**Sparse set** (EnTT's default, and the easiest to write):
Per component type: a dense array of components, a dense array of entity
ids, and a sparse array mapping entity id → dense index.
```
  sparse: [_, 0, _, 1, 2, _]        indexed by entity id
  dense:  [e1, e3, e4]              packed entity ids
  data:   [c1, c3, c4]              packed components, parallel to dense
```
- O(1) add, remove, and has-component, with no structural moves.
- Iterating one component type is dense and fast.
- Iterating an *intersection* of two types requires checking membership
  per element (iterate the smallest set, look up in the others) — good,
  but not as fast as archetype chunks.

**For `A008`, write the sparse set.** It is ~200 lines, the data structure
is genuinely elegant, and it teaches the concepts without the archetype
bookkeeping. Understand the archetype design well enough to explain the
trade-off.

**Where it bites.** SoA is not free: random access to a single entity's
full state now touches N arrays instead of one, which is *worse* than AoS
for that pattern. Gameplay code that inspects one entity deeply (an AI
making a decision about a specific target) is an AoS-shaped workload. This
is why real engines are hybrid, and why "just use SoA" is not the lesson.

**Read.**
- `[DOD]` ch. 3–5 — https://www.dataorienteddesign.com/dodbook/node4.html — the layout argument at length.
- `[MIKEACTON]` — CppCon 2014, if you haven't yet.
- EnTT's "ECS back and forth" series — https://skypjack.github.io/2019-02-14-ecs-baf-part-1/ — the best written explanation of sparse sets, by EnTT's author. Read parts 1–4 before `A008`.
- `[OVERWATCH-ECS]` — Ford's GDC 2017 talk, a shipped ECS at scale.

**Check yourself.**
1. Compute the wasted bandwidth for a 64-byte struct where a loop reads 12 bytes.
2. Explain the sparse-set structure and why add/remove is O(1).
3. What is a structural change in an archetype ECS, and what does it cost?
4. Give a workload where AoS beats SoA.

### U04 — Systems, queries, and ordering
**What it is.** The other half of ECS: the code. A system is a function
over all entities matching a component query.

**Why it exists.** Once data is stored by component, behaviour naturally
becomes "for all entities with X and Y, do Z" — which is a loop over dense
arrays rather than a virtual call per object.

**The mechanism.**

```cpp
// instead of: for (obj : objects) obj->Update(dt);
void movement_system(World& w, float dt) {
    for (auto [e, transform, velocity] : w.view<Transform, Velocity>()) {
        transform.position += velocity.value * dt;
    }
}
```

The gains are structural as much as mechanical: no virtual dispatch, dense
iteration, and — importantly — **the system's data dependencies are
declared in its signature**, which is what makes automatic parallelization
possible in `T13.S05`.

**Ordering is the hard part.** Systems must run in a defined order and
the order is a correctness property: physics before collision response,
collision before damage, damage before death, death before render. Options:

- **Explicit list.** A hand-written ordered array of systems. Obvious,
  debuggable, and completely adequate for a solo project. **Use this.**
- **Declared dependencies + topological sort.** Each system declares what
  it reads and writes; the engine derives an order and can parallelize
  independent systems. More machinery; the right answer at scale.
- **Phases/stages.** A coarse grouping (Input → Simulation → Physics →
  Late → Render) with explicit ordering inside each. A good middle ground,
  and it maps directly onto `T01.S01.U01`'s frame diagram.

**Structural changes during iteration** — the trap. Spawning or destroying
an entity while iterating the array you're spawning into invalidates the
iteration. Every ECS has to answer this; the standard answer is a
**command buffer**: systems record structural changes into a queue, and
the queue is applied at a defined sync point between systems.
```cpp
cmd.destroy(entity);              // recorded, not executed
cmd.add<Burning>(entity, {5.0f});
// ... after the system finishes:
cmd.flush(world);
```
This is not optional and it is much easier to build in from the start than
to add later. It also happens to be exactly what you need for determinism
(`T01.S02.U05`) — deferred, ordered application means the result doesn't
depend on iteration order.

**Where it bites.** Entity destruction mid-frame. Entity A kills entity B,
which was going to shoot entity C this frame. Whether C gets shot depends
on system order and on whether B was already processed. Without a command
buffer and a defined order, this is nondeterministic and unfixable.

**Read.**
- EnTT docs on views, groups, and the registry — https://github.com/skypjack/entt/wiki
- `[OVERWATCH-ECS]` — the system-ordering discussion specifically.
- `[GEA3]` §16.8 — updating game objects in real time, and the ordering problem.

**Check yourself.**
1. Why does declaring a system's components in its signature enable parallelization?
2. What is a command buffer and what two problems does it solve?
3. Give a concrete ordering bug: two systems whose order changes the outcome.

### U05 — The honest case against ECS
**What it is.** The counterarguments, stated properly, because you should
be able to make them.

**Why it exists.** ECS has become a default recommendation, and defaults
adopted without their conditions are how people spend three weeks building
infrastructure for a game with 40 entities.

**The mechanism.** The real costs:

1. **The performance argument is conditional on N.** The cache benefit
   scales with the number of entities iterated. At 100 entities everything
   fits in L2 and the layout is nearly irrelevant. The benefit is large at
   10,000+ and dominant at 100,000+. **Most games are not in that
   regime for most of their entities.** A platformer with 50 enemies gains
   nothing measurable; a bullet-hell with 20,000 projectiles gains a lot —
   and could have got that specific win with one SoA particle array.

2. **Debugging is harder.** "What is this entity?" is answered by
   inspecting one object in the OOP model, and by querying N component
   stores in an ECS. You need tooling (`T12.S03`) to make this tolerable,
   and that tooling is real work.

3. **Some logic is genuinely entity-centric.** An AI decision that
   inspects a specific target's health, position, faction, and inventory
   is a random access across four stores — the worst case for SoA. Forcing
   it into a system-shaped loop makes it more complex and slower.

4. **Relationships are awkward.** Parent-child hierarchies, inventories,
   "the entity that fired this projectile" — none are naturally expressed
   as components, and every ECS grows a bolted-on answer.

5. **It is infrastructure, and infrastructure is not a game.** The most
   common failure mode of an ECS-first hobby project is that the ECS gets
   finished and the game does not.

**The honest synthesis:** ECS is a *storage and iteration* strategy that
pays off for large homogeneous populations processed uniformly. Use it
there. Elsewhere, plain composition with per-type arrays (U02's
intermediate) captures most of the benefit for a tenth of the machinery.
Real engines are hybrid, and being able to say *why* each part is the way
it is beats picking a side.

**Where it bites.** Building a fully general ECS as assignment `A008` and
then discovering it was never the bottleneck. Which is exactly why `A008`
requires the measurement — the finding "at Pong's scale this bought
nothing" is a *correct* and valuable result, and it is the point of doing
it at a scale where you can see the whole picture.

**Read.**
- Aras Pranckevičius, "Entity Component Systems & Data Oriented Design" — https://aras-p.info/texts/files/2018Academy%20-%20ECS-DoD.pdf — balanced, from a Unity graphics lead.
- Sander Mertens, "Why Vanilla ECS Is Not Enough" and the "ECS FAQ" — https://github.com/SanderMertens/ecs-faq — from Flecs' author, includes the limitations honestly.
- Then re-read `[MIKEACTON]` and notice he argues for data-oriented design, **not** for ECS specifically. The conflation of the two is the source of much of the dogma.

**Check yourself.**
1. At what entity count does the cache argument start to matter? Justify with cache sizes.
2. Give a game and a workload where ECS is clearly right, and one where it is clearly overkill.
3. Why is "data-oriented design" not the same claim as "use an ECS"?

### U06 — What to build for A008
**What it is.** The concrete scope for the ECS you will write.

**Why it exists.** Because "write an ECS" is unbounded, and the bounded
version teaches the same things.

**The mechanism.** Build a **sparse-set ECS** with:

```cpp
// Entities: handle from T03.S03.U05 — index + generation
struct Entity { uint32_t index : 24, generation : 8; };

// Storage: one sparse set per component type
template <typename T> struct ComponentStore {
    std::vector<uint32_t> sparse;   // entity index -> dense index
    std::vector<Entity>   dense;    // packed entity list
    std::vector<T>        data;     // packed components, parallel to dense
};

// API:
Entity create();
void   destroy(Entity);             // deferred via command buffer
template<class T> T&   add(Entity, T);
template<class T> void remove(Entity);
template<class T> bool has(Entity) const;
template<class T> T*   get(Entity);          // null if absent — never assert
template<class... Ts> View<Ts...> view();    // iterate the intersection

// Systems: a plain ordered array of function pointers. No scheduler.
```

Explicitly out of scope for `A008` — these are how the assignment stays
a week rather than a month:
- Archetypes / chunks (understand them, don't build them)
- Automatic parallel scheduling (that's `A044`)
- Reflection / serialization of components (that's `A020`/`A039`)
- Hierarchies and relationships (add a `Parent` component if you need one)
- Component change detection / reactive systems

**The view over multiple types** is the piece worth thinking about: iterate
the *smallest* store and check `has<>` on the others, rather than
iterating the first one named. That single decision is a large constant
factor and it is the kind of thing you only notice by writing it.

**Where it bites.** Templates. A type-erased component store with
`std::type_index` keys is a reasonable simple design; a fully
compile-time-typed one is faster and much more fiddly. Start type-erased,
measure, and only reach for the template machinery if the measurement
justifies it.

**Read.**
- EnTT "ECS back and forth" parts 1–4 — the reference implementation walkthrough.
- `[GEA3]` §16.2.3 — a real engine's version of the same decisions.

**Check yourself.**
1. Why iterate the smallest component store in a multi-type view?
2. Why should `get<T>()` return null rather than assert?
3. Name three things deliberately out of scope for `A008` and say which later assignment covers each.

---

## Implementation

| assignment | size | target rung | what it forces you to understand |
|---|:---:|:---:|---|
| [A008](../assignments/A008-ecs-pong.md) | L | R4 | Sparse sets, views, command buffers, and — critically — a measurement of what it bought over the `A006` version |

**Order and overlap.** `A008` is the M1 milestone. It rebuilds `A006`'s
Pong on the ECS, which is deliberate: **the same game twice, two
architectures**, so the comparison is real rather than hypothetical.
`A008`'s R3 rung requires benchmarking both at 100, 10,000, and 100,000
entities, and its R2 rung requires you to write down which architecture
you would choose for a real game of each size.

The ECS is then the substrate for everything: `A012` (sprite rendering as
a system), `A017` (physics bodies as components), `A019` (events between
systems), `A020` (data-driven component definitions), `A043` (the DOD
refactor, on this code), `A044` (parallel systems, which needs U04's
dependency declarations).

The command buffer from U04 is what `A047`'s rollback will save and
restore, so its design matters more than it appears now.

**If you are short on time.** A single-component-type sparse set plus a
one-type view is the core and is maybe 100 lines. Multi-type views and
the command buffer are what make it usable. Skip the templates entirely
and hand-write three component stores if the metaprogramming becomes the
bottleneck — the lesson is the data structure, not the C++.

---

## Discussion Log
<!-- Appended per study session. See ../CONVENTIONS.md §7. -->

## Open Questions

## Connections
- `T03.S03.U04` — the cache facts that are this subtopic's entire performance argument.
- `T03.S03.U05` — handles, which entity ids are an instance of.
- `T06.S01` — gameplay patterns, which sit on top of whatever this subtopic decides.
- `T06.S02` — data-driven entities, which U02's "entities are data" makes possible.
- `T13.S03` — the measured version of this argument.
- `T13.S05` — parallel systems, enabled by U04's declared dependencies.
