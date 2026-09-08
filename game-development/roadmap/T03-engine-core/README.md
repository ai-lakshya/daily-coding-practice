# T03 — The Game Loop & Engine Core
**Phase:** A | **Depends on:** T01, T02 | **Borrows from:** T04 (you need *a* way to draw before you can see any of this working — use SDL3's 2D renderer as a placeholder and replace it in `A011`)
**Status:** expanded | **Assignments:** A006–A008

## Why this topic, here
`T03` is the choke point of the whole roadmap. Everything downstream —
physics, gameplay, AI, networking, performance — is written *against* the
core you build here. A weak core does not fail loudly; it makes every
later topic 30% harder in ways that feel like your own stupidity.

Three ideas carry it:
1. **Time is a first-class system**, not a `deltaTime` you pass around.
2. **Memory is a decision**, not something `new` does for you.
3. **The shape of your data determines the shape of your code** — and
   choosing that shape is the entity-model question.

By the end you will have a small engine that runs a real game with a
fixed timestep, custom allocators, handle-based resources, and an ECS you
understand completely because you wrote all of it.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | [The Game Loop & Time](S01-game-loop.md) | Variable vs fixed timestep, the accumulator, render interpolation, spiral of death, pause and time dilation | T01.S02 | 6 | A006 | expanded |
| S02 | [Input](S02-input.md) | Events vs polling, device abstraction, action mapping, buffering, dead zones, and input latency | S01 | 5 | A006 | expanded |
| S03 | [Memory: Allocators & Ownership](S03-memory.md) | Stack/arena/pool/freelist allocators, alignment, lifetimes, handles vs pointers, and why `new` is a design smell in a frame | — | 6 | A007 | expanded |
| S04 | [Data Layout & Entity Models](S04-entity-models.md) | Inheritance → components → ECS; AoS vs SoA; archetype vs sparse-set; what an ECS actually buys and costs | S03, T02.S01 | 6 | A008 | expanded |
| S05 | [Resources & the Engine Spine](S05-resources.md) | Asset handles, reference counting, loading, subsystem lifetimes and startup order, config, and the shutdown you always get wrong | S03 | 5 | A007 | expanded |

## Exit criteria
- [ ] You can explain the accumulator pattern and the spiral of death, and show the code that prevents it.
- [ ] Your Pong is provably identical at 30, 60, 144 and 1000 FPS — and you have a test that proves it.
- [ ] You can explain why render interpolation is needed with a fixed timestep, and what visual artifact appears without it.
- [ ] You can state the input latency chain in your own program and where you added a frame to it.
- [ ] You can implement an arena allocator and a pool allocator from scratch, with correct alignment, and say when each is the wrong choice.
- [ ] You can explain what a generational handle protects against that a raw pointer doesn't, with the dangling-reference scenario written out.
- [ ] You can articulate the actual argument for ECS — and the honest counter-argument — without reciting cache-line marketing.
- [ ] You have measured your own AoS vs SoA iteration and can report the numbers.

## Traps specific to this topic
- **Building "the engine" instead of a game's engine.** The scope of
  `T03` is: enough core to run Pong well. Not a scene graph, not a
  reflection system, not a plugin architecture.
- **ECS as religion.** ECS is a data-layout answer to a specific
  performance question. `A008` requires you to write down *what* it bought
  you, measured. If the answer is "nothing at this scale", that is the
  correct and useful answer — most Pong-sized games do not need one.
- **Fixed timestep half-done.** Fixing the physics step but reading input
  in the render loop, or accumulating without interpolating, produces
  stutter that you will spend `T05` blaming on the physics.
- **Deferring shutdown.** Subsystem teardown order is where engines leak
  and crash-on-exit. Get it right in `A007` while it is three systems.

## Primary sources for the whole topic
- `[GEA3]` ch. 8 (game loop), ch. 6 (resources), ch. 15.3 (runtime object model) — the core reading for this topic
- `[GAFFER]` "Fix Your Timestep!" — https://gafferongames.com/post/fix_your_timestep/ — read this twice
- `[GPP]` — "Game Loop", "Update Method", "Component", "Object Pool", "Data Locality"
- `[DOD]` ch. 1–5 — for `S04`, the honest version of the argument
