# T05 — Collision Detection & Physics
**Phase:** B | **Depends on:** T02, T03 | **Borrows from:** T04 (a debug line renderer — you cannot debug collision code you cannot see)
**Status:** stub | **Assignments:** A014–A018

## Why this topic, here
Physics is the topic where "it mostly works" and "it works" are separated
by a factor of ten in effort, and where the difference is entirely
visible to the player. It is also the best possible training ground for
numerical thinking, spatial data structures, and iterative solvers — all
of which pay off in `T10` and `T13`.

Two distinct skills live here and beginners conflate them:

- **Simulation physics** — bodies, forces, constraints, solvers. Correct,
  emergent, and almost never what you want for the player character.
- **Gameplay physics** — swept AABBs, coyote time, hand-authored jump
  arcs. Deliberately unphysical, and the reason platformers feel good.

You will build both, in that order, and `A018` is where the distinction
finally becomes obvious.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | Kinematics & Numerical Integration | Forces, explicit vs semi-implicit Euler vs Verlet vs RK4, stability, energy drift, damping, and why games use symplectic Euler | T02.S01, T03.S01 | 5 | A014 | stub |
| S02 | Narrowphase Collision Detection | AABB/circle/OBB tests, the separating axis theorem, GJK and EPA, swept and continuous tests, contact manifold generation | T02.S04 | 7 | A015 | stub |
| S03 | Broadphase & Spatial Partitioning | Uniform grids, sweep-and-prune, BVHs, quadtrees/octrees, spatial hashing, and choosing by workload not by fashion | S02, T03.S03 | 6 | A016 | stub |
| S04 | Constraint Solving & Response | Impulses, restitution, friction (Coulomb), sequential impulse solvers, warm starting, position correction/Baumgarte, stacking, sleeping | S01, S02 | 7 | A017 | stub |
| S05 | Character Controllers & Gameplay Physics | Swept AABB movement, slopes and steps, jump arcs derived from design constants, coyote time, jump buffering, triggers, raycasts | S02, S04 | 6 | A018 | stub |

## Exit criteria
- [ ] You can explain why explicit Euler gains energy and semi-implicit doesn't, and show the plot from `A014`.
- [ ] You can implement SAT for convex polygons including the minimum translation vector, and explain why it fails for curved shapes.
- [ ] You can explain GJK's core idea (Minkowski difference + simplex containing the origin) without reciting code.
- [ ] You can explain tunnelling and name two independent fixes with their costs.
- [ ] You can pick a broadphase for a stated workload and justify it with your own benchmark numbers from `A016`.
- [ ] You can derive the normal impulse magnitude for a 2-body collision with restitution.
- [ ] You can explain why sequential impulses converge, what warm starting buys, and why boxes sink without position correction.
- [ ] Given a desired jump height and time-to-apex, you can compute gravity and initial velocity — and you have a controller tuned that way.

## Traps specific to this topic
- **Using a rigid body simulation for the player.** Every beginner does
  this. The player then slides on slopes, catches on tile seams, and
  feels drunk. `S05` exists to prevent it.
- **Debugging without visualization.** Build the debug renderer *first*.
  `A015`'s spec requires it for exactly this reason.
- **Chasing physical accuracy.** Games want plausible and stable, not
  accurate. A solver that is wrong but never explodes beats the reverse.
- **Variable timestep physics.** Non-negotiable: physics steps at a fixed
  rate. This is why `T03.S01` comes first.

## Primary sources for the whole topic
- `[RTCD]` ch. 4–5 (bounding volumes, primitive tests), ch. 6–7 (spatial partitioning), ch. 9 (GJK) — the reference
- `[GPED]` — the constructive counterpart; builds a working engine step by step
- `[BOX2D]` / `[CATTO-GDC]` — Erin Catto's slides on sequential impulses and soft constraints, from the person who wrote Box2D
- `[GAFFER]` — the physics articles, especially on integration and fixed timesteps
- `[CELESTE]` — how a shipped, extremely good platformer does `S05`
