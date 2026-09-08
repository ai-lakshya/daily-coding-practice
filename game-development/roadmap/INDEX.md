# Layer 1 — Topic Map

The whole track in dependency order. Read `CONVENTIONS.md` once before
using this; read this file at the start of every session.

**Ordering rule:** topics are listed in the order they should be taken.
That order is a merge of *dependency* (you cannot solve constraints
before you can integrate motion) and *priority* (you should be able to
make a playable thing before you learn PBR). Where the two conflict,
priority wins and the dependency is noted so you know what you are
borrowing on credit.

---

## Phases

| phase | name | what you can do at the end of it |
|---|---|---|
| **A** | Foundations | Read any engine's source without drowning. Have a math library and an engine core you wrote. |
| **B** | First Playable | Ship a complete, juicy 2D game on your own engine. This is the phase that makes you a game developer rather than a person learning graphics. |
| **C** | 3D & Systems Depth | Build a 3D renderer with modern lighting, animated characters, AI that reads as intentional, and audio that isn't an afterthought. |
| **D** | Engineering at Scale | Make it fast, make it tooled, make it multiplayer. This is the phase that separates hobby from professional. |
| **E** | Production & Delivery | Work in a commercial engine, scope a real project, and put a game in front of strangers. |

---

## Topics

`Deps` are hard prerequisites. `Borrows` means you will use something
from a later topic before you formally study it — that is fine and
deliberate, it is noted so it doesn't feel like a gap.

| id | phase | topic | deps | borrows | subtopics | assignments | status |
|---|:---:|---|---|---|---:|---|---|
| [T01](T01-foundations/README.md) | A | Real-Time Programming Foundations | — | — | 4 | A001–A002 | expanded |
| [T02](T02-math/README.md) | A | Math for Games | T01 | — | 5 | A003–A005 | expanded |
| [T03](T03-engine-core/README.md) | A | The Game Loop & Engine Core | T01, T02 | T04 (a window to draw in) | 5 | A006–A008 | expanded |
| [T04](T04-rendering-2d/README.md) | B | Rendering Foundations & the GPU Pipeline | T02, T03 | — | 6 | A009–A013 | stub |
| [T05](T05-physics/README.md) | B | Collision Detection & Physics | T02, T03 | T04 (debug draw) | 5 | A014–A018 | stub |
| [T06](T06-gameplay/README.md) | B | Gameplay Architecture & Data-Driven Design | T03, T05 | — | 5 | A019–A022 | stub |
| [T07](T07-game-feel/README.md) | B | Game Feel, Juice & Design Fundamentals | T04, T05, T06 | T11 (sound is half of feel) | 5 | A023–A024 | stub |
| [T08](T08-rendering-3d/README.md) | C | 3D Rendering: Lighting, Shading & Modern Pipelines | T04 | — | 7 | A025–A029 | stub |
| [T09](T09-animation/README.md) | C | Animation Systems | T02, T08 | — | 5 | A030–A032 | stub |
| [T10](T10-ai/README.md) | C | Game AI | T02, T05, T06 | — | 5 | A033–A036 | stub |
| [T11](T11-audio/README.md) | C | Audio | T03 | — | 4 | A037–A038 | stub |
| [T12](T12-tools/README.md) | D | Tools, Asset Pipeline & Editors | T03, T06, T08 | — | 5 | A039–A041 | stub |
| [T13](T13-performance/README.md) | D | Performance Engineering | T03, T05, T08 | — | 6 | A042–A044 | stub |
| [T14](T14-networking/README.md) | D | Networking & Multiplayer | T05, T06 | — | 6 | A045–A047 | stub |
| [T15](T15-engines/README.md) | E | Commercial Engines (specialization fork) | Phases A–D | — | 5 | A048–A049 | stub |
| [T16](T16-shipping/README.md) | E | Shipping: Scope, Production & Release | T07, T12 | — | 5 | A050–A051 | stub |

---

## Dependency graph

```
                          T01 Foundations
                                |
                          T02 Math for Games
                                |
                          T03 Game Loop & Engine Core
                          /            |            \
             T04 Rendering 2D    T05 Physics    T11 Audio
                    |     \        /    |            |
                    |      \      /     |            |
                    |       T06 Gameplay Arch        |
                    |          /       |   \         |
                    |         /        |    \        |
                    |    T07 Game Feel <----------- (audio feeds feel)
                    |         |        |     \
        T08 Rendering 3D      |    T10 Game AI \
           /       \          |                 \
   T09 Animation  T13 Perf    |            T14 Networking
           \       /          |                 /
            T12 Tools --------+----------------+
                       \      |      /
                        T15 Commercial Engines
                                |
                        T16 Shipping
```

Two things to read off that graph:

1. **T03 is the choke point.** Everything downstream assumes you have a
   loop, a memory strategy, and an entity model you wrote yourself. Do
   not rush it — a weak `T03` makes `T05`, `T06`, `T10`, and `T13` all
   harder than they need to be.
2. **T07 is not optional decoration.** It sits mid-graph on purpose.
   Learning game feel *after* you've built the systems but *before* you
   go deep on 3D is what stops you becoming an engineer who can render
   anything and make nothing worth playing.

---

## Milestones

Three points where the track stops teaching and demands a whole artifact:

| id | after | what |
|---|---|---|
| **M1** | T03 | Your engine core runs a framerate-independent Pong on an ECS you wrote. (`A008`) |
| **M2** | T07 | A finished, packaged, juicy 2D game with menus, 3+ levels, sound. (`A023`) |
| **M3** | T16 | A game published publicly with a build pipeline, playtest rounds, and a written postmortem. (`A051`) |

If you only ever get to **M2**, you are a game developer. Everything
after it is about the kind of game developer you become.

---

## Expansion status

Layer-3 docs (`SNN-*.md`) are written on demand — see `CONVENTIONS.md` §2.
Currently expanded: **T01, T02, T03** (all of Phase A).

To expand the next topic, say *"expand T04"*. The agent writes the Layer-3
docs for that topic's subtopics from the scope and sources already listed
in its `README.md`, so nothing is invented late — the plan is already
there, only the prose is deferred.
