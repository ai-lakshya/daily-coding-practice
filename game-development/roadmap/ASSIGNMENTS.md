# Assignment Registry

Every assignment in the track, in ladder order. This is the flat view;
the roadmap tree is the structured one. Sizes and rungs are defined in
`CONVENTIONS.md` §3–§4.

**How to read the `Reuses` column:** this is the overlap contract. An
assignment with an empty `Reuses` outside T01 is a design bug. The point
of the ladder is that `A003`'s math library is still being edited when you
reach `A030`, and by then you will have written `dot()` from memory a
dozen times.

Design docs live in `assignments/ANNN-<slug>.md`. Docs marked `stub` in
the last column are generated when you reach them — say *"expand A014"*
or just start the assignment and the design doc gets written first.

---

## Phase A — Foundations

| id | topic | title | size | rung | reuses | status | doc |
|---|---|---|:--:|:--:|---|---|---|
| [A001](assignments/A001-frame-time-harness.md) | T01.S04 | Frame-time harness & percentile reporting | XS | R2 | — | todo | written |
| [A002](assignments/A002-window-skeleton.md) | T01.S03 | Cross-platform CMake + SDL3 window skeleton | S | R2 | A001 | todo | written |
| [A003](assignments/A003-math-core.md) | T02.S01–S02 | Vector & matrix math library with tests | S | R3 | A002 | todo | written |
| [A004](assignments/A004-math-geometry.md) | T02.S03–S04 | Quaternions, transforms & intersection tests | M | R3 | A003 | todo | written |
| [A005](assignments/A005-orbit-and-aim.md) | T02.S01, S05 | "Orbit & aim" visual math toy | S | R2 | A002, A003 | todo | written |
| [A006](assignments/A006-fixed-timestep-pong.md) | T03.S01–S02 | Framerate-independent Pong (fixed timestep + input) | M | R3 | A002, A003, A005 | todo | written |
| [A007](assignments/A007-allocators.md) | T03.S03, S05 | Arena/pool allocators + handle-based resources | M | R3 | A006 | todo | written |
| [A008](assignments/A008-ecs-pong.md) | T03.S04 | Minimal ECS, and Pong rebuilt on it | L | R4 | A006, A007 | todo | written |

**M1 milestone** — `A008` complete. You have an engine core you wrote.

## Phase B — First Playable

| id | topic | title | size | rung | reuses | status | doc |
|---|---|---|:--:|:--:|---|---|---|
| A009 | T04.S01 | Software rasterizer: lines, triangles, barycentric, z-buffer | M | R3 | A003 | todo | stub |
| A010 | T04.S01–S02 | Software rasterizer II: perspective, texturing, culling — a spinning cube | M | R3 | A009, A004 | todo | stub |
| A011 | T04.S03–S04 | The same cube on the GPU (OpenGL) — and a written comparison | M | R3 | A010, A002 | todo | stub |
| A012 | T04.S05–S06 | 2D sprite renderer: atlas, batching, camera, layers | M | R3 | A011, A008 | todo | stub |
| A013 | T04.S06 | Tilemap renderer with culling and a scrolling level | M | R2 | A012 | todo | stub |
| A014 | T05.S01 | Integrator bake-off: Euler vs semi-implicit vs RK4, energy plots | S | R2 | A003 | todo | stub |
| A015 | T05.S02 | 2D narrowphase: AABB, circle, SAT — with a visual debugger | M | R3 | A014, A012 | todo | stub |
| A016 | T05.S03 | Broadphase: uniform grid + sweep-and-prune, benchmarked at 10k bodies | M | R3 | A015, A007 | todo | stub |
| A017 | T05.S04 | Impulse-based 2D rigid body solver: restitution, friction, stacking | L | R4 | A015, A016 | todo | stub |
| A018 | T05.S05 | Platformer character controller: swept AABB, coyote time, tuned jump arc | M | R4 | A015, A008 | todo | stub |
| A019 | T06.S01 | Event bus + component behaviours; refactor the platformer onto them | M | R3 | A018, A008 | todo | stub |
| A020 | T06.S02 | Data-driven entities and levels with hot reload | M | R3 | A019, A013 | todo | stub |
| A021 | T06.S04 | Versioned save/load, and a replay from recorded input | S | R3 | A020, A006 | todo | stub |
| A022 | T06.S03 | Embed Lua; script one enemy's behaviour without recompiling | M | R2 | A020 | todo | stub |
| A024 | T07.S01–S02 | Juice pass: screenshake, hitstop, tweens, particles — A/B'd | S | R2 | A018, A012 | todo | stub |
| A023 | T07 (all) | **Ship a complete 2D game**: menus, 3+ levels, audio, packaged build | XL | R4 | A012–A022, A024, A037 | todo | stub |

Note the deliberate inversion: **do `A024` before `A023`.** Learning juice
on a small isolated harness and then applying it to the full game is much
more effective than trying to discover it while also fighting scope.

**M2 milestone** — `A023` shipped. You are a game developer.

## Phase C — 3D & Systems Depth

| id | topic | title | size | rung | reuses | status | doc |
|---|---|---|:--:|:--:|---|---|---|
| A025 | T08.S02–S03 | Forward renderer: Blinn-Phong, multiple lights, normal mapping | M | R3 | A011, A004 | todo | stub |
| A026 | T08.S01–S02 | PBR: Cook-Torrance metallic-roughness, with a material test scene | L | R4 | A025 | todo | stub |
| A027 | T08.S04 | Shadow mapping, then cascaded shadow maps | M | R3 | A025 | todo | stub |
| A028 | T08.S05–S06 | Deferred pipeline + post stack (bloom, tonemap, FXAA, SSAO) | L | R3 | A026, A027 | todo | stub |
| A029 | T08.S07 | Vulkan: triangle to textured mesh — a port of `A011` | L | R3 | A011, A028 | todo | stub |
| A030 | T09.S01–S02 | glTF skinned mesh: CPU skinning, then GPU skinning, measured | M | R3 | A025, A004 | todo | stub |
| A031 | T09.S03 | Blend trees, an animation state machine, and root motion | M | R4 | A030, A019 | todo | stub |
| A032 | T09.S04 | Two-bone IK: foot placement and look-at on a moving character | M | R3 | A031, A004 | todo | stub |
| A033 | T10.S01 | Steering behaviours: seek, arrive, wander, obstacle avoidance, flocking | S | R2 | A014, A012 | todo | stub |
| A034 | T10.S02 | A* on a grid, then navmesh pathfinding with smoothing | M | R3 | A033, A016 | todo | stub |
| A035 | T10.S03–S04 | Behaviour tree runtime + a guard NPC with real perception | M | R4 | A034, A019 | todo | stub |
| A036 | T10.S03 | The same NPC on utility AI — and a written comparison with `A035` | M | R3 | A035 | todo | stub |
| A037 | T11.S01–S02 | Audio mixer: decode, resample, mix, voice management | M | R3 | A007, A006 | todo | stub |
| A038 | T11.S03–S04 | 3D positional audio, occlusion, and adaptive music layers | M | R3 | A037, A018 | todo | stub |

## Phase D — Engineering at Scale

| id | topic | title | size | rung | reuses | status | doc |
|---|---|---|:--:|:--:|---|---|---|
| A039 | T12.S01–S02 | Asset pipeline: offline baker (glTF/PNG/WAV → binary) + runtime loader | M | R3 | A030, A037, A007 | todo | stub |
| A040 | T12.S03 | ImGui level editor: inspector, gizmos, undo/redo, saves back to data | M | R4 | A020, A039 | todo | stub |
| A041 | T12.S04 | Hot reload: shaders, then data, then the gameplay module itself | M | R4 | A040, A022 | todo | stub |
| A042 | T13.S02 | Instrument a frame profiler; find and fix three real hotspots | S | R3 | A028, A017 | todo | stub |
| A043 | T13.S03–S04 | Data-oriented refactor + SIMD a hot loop; every claim measured | M | R4 | A042, A008 | todo | stub |
| A044 | T13.S05 | Work-stealing job system; parallelize broadphase and animation | L | R4 | A043, A016, A030 | todo | stub |
| A045 | T14.S01 | UDP layer: sequencing, acks, reliability, fragmentation, a loss simulator | M | R3 | A006, A021 | todo | stub |
| A046 | T14.S03–S04 | Client-server: snapshots, delta compression, prediction, reconciliation | L | R4 | A045, A018, A021 | todo | stub |
| A047 | T14.S05 | Deterministic lockstep with rollback for a 2-player action game | L | R4 | A046, A017, A021 | todo | stub |

## Phase E — Production & Delivery

| id | topic | title | size | rung | reuses | status | doc |
|---|---|---|:--:|:--:|---|---|---|
| A048 | T15.S02–S03 | Unreal C++: a replicated gameplay ability in a small level | M | R3 | A046 (concepts) | todo | stub |
| A049 | T15.S05 | Port an earlier prototype to Unreal/Godot; write the comparison | L | R3 | A048, A018 | todo | stub |
| A050 | T16.S01 | Vertical slice with a written production plan and a documented cut list | M | R3 | A023 | todo | stub |
| A051 | T16 (all) | **Ship publicly**: store page, build pipeline, playtests, postmortem | XL | R4 | A050, A023 | todo | stub |

**M3 milestone** — `A051` live. Strangers have played your game.

---

## Concept coverage matrix

Where the deliberate repetition is. Read a row as "this concept is hit
this many times, from these angles."

| concept | assignments | angles |
|---|---|---|
| Vector/matrix math | A003, A004, A005, A009, A010, A011, A015, A025, A030, A032 | write it → use it in 2D → use it in a rasterizer → use it on GPU → use it for skinning |
| Fixed timestep / determinism | A006, A014, A017, A021, A045, A047 | loop → integrator → solver → replay → network → rollback |
| Custom allocators | A007, A016, A037, A039, A043, A044 | learn → spatial structures → audio voices → asset loading → DOD → job system |
| Data layout (AoS/SoA) | A008, A012, A016, A030, A043, A044 | ECS → batching → broadphase → skinning → measured refactor → parallelism |
| Collision queries | A015, A016, A017, A018, A033, A034, A038 | narrowphase → broadphase → solver → controller → avoidance → paths → audio occlusion |
| GPU pipeline | A011, A012, A025, A027, A028, A029, A030 | first triangle → 2D batching → lighting → shadows → deferred → Vulkan → skinning |
| State machines | A019, A031, A035, A036 | gameplay → animation → AI → compared against utility AI |
| Serialization | A020, A021, A039, A046 | data-driven → saves → asset bake → network snapshots |
| Profiling & measurement | A001, A007, A010, A016, A030, A042, A043, A044 | it is in the rubric from the first hour and never leaves |
| Debug visualization | A005, A015, A016, A034, A040 | the tool you keep rebuilding until you finally make it reusable |

If you ever feel like you are repeating yourself: **good.** That is the
design. The second time you write a broadphase you will do it in an
afternoon and it will be better, and that gap is what "experienced" means.
