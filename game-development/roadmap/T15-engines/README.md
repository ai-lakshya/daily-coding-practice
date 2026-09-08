# T15 — Commercial Engines (Specialization Fork)
**Phase:** E | **Depends on:** Phases A–D | **Borrows from:** —
**Status:** stub | **Assignments:** A048–A049

## Why this topic, here
This is the fork, and its position is the most opinionated decision in
the roadmap. Most paths start here. This one ends here, for a specific
reason: an engine is a set of answers, and answers are only legible if
you have met the questions. Having written a renderer, a solver, an ECS,
and a netcode layer, Unreal's `UPROPERTY`, its replication model, and its
animation blueprint stop being arbitrary API and become *someone else's
solution to a problem you have solved differently* — which is a position
from which you can evaluate rather than obey.

The cost of this ordering is honest and worth naming: you will be later
to Unreal-specific fluency than someone who started there. The benefit is
that engine-specific fluency is the fastest thing on this list to acquire
(weeks) and the foundations are the slowest (years).

**`S01` is the actual fork.** By this point you will have touched
rendering, physics, tools, AI, and networking enough to choose a
specialization from experience rather than from job titles.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | Choosing a Lane | The real roles (engine, gameplay, graphics, physics, tools, AI, network, technical designer), what each does daily, what each is hired on, and mapping your Phase A–D experience onto them | — | 5 | — | stub |
| S02 | Unreal Engine: The C++ Model | Modules and the build system, UObject/reflection and the UHT, actors vs components vs subsystems, garbage collection, the gameplay framework, and C++/Blueprint interop boundaries | S01 | 7 | A048 | stub |
| S03 | Unreal: Subsystems as a User | The rendering path and materials, the animation system (ABP, state machines, control rig), the Gameplay Ability System, and Unreal's replication model against what you built in `A046` | S02, T08, T09, T14 | 6 | A048 | stub |
| S04 | Godot as the Lighter Alternative | Nodes/scenes as a composition model vs ECS, GDScript vs C# vs GDExtension in C++, the renderer, and where Godot is genuinely the better choice | S01 | 5 | — | stub |
| S05 | Porting & Comparison | Taking an earlier prototype into a commercial engine, what transferred and what didn't, reading engine source as a habit, and writing the comparison document | S02, S04 | 4 | A049 | stub |

## Exit criteria
- [ ] You can explain what UHT/reflection generates and why Unreal needs it, in terms of the problem it solves.
- [ ] You can write a replicated gameplay ability in C++ and explain where each piece of state lives and who is authoritative.
- [ ] You can map Unreal's replication onto the vocabulary from `T14.S03`–`S04` and name where it differs from what you built.
- [ ] You can state, in writing, which specialization you are pursuing and cite specific experiences from Phases A–D that support it.
- [ ] You have `A049`'s comparison document: what your engine did better, what the commercial engine did better, and what you'd change in yours.
- [ ] You can navigate the engine's source to answer a behaviour question rather than guessing from documentation.

## Traps specific to this topic
- **Blueprint-only.** For a C++ engineer this wastes the advantage you
  spent four phases building. Learn the C++ layer; use Blueprint where it
  is genuinely better (designer-facing composition, prototyping).
- **Fighting the engine.** The most common failure of people who arrive
  here from a custom engine. Unreal's answers are usually load-bearing;
  understand *why* before routing around.
- **Choosing a specialization by prestige.** Graphics is the glamorous
  one, tools and gameplay are the ones with the most open roles and the
  most direct impact on the game. Choose from `S01`'s daily-work
  descriptions, not from titles.
- **Skipping `S05`'s writeup.** The comparison document is the artifact
  that consolidates four phases. It is also, incidentally, the best
  interview material you will ever have.

## Primary sources for the whole topic
- `[UEDOCS]` — the C++ programming, gameplay framework, GAS, and networking sections
- Unreal Engine source on GitHub — the actual reference; `S05` requires reading it
- `[GODOTDOCS]` — the C++/GDExtension and architecture sections
- `[GDCVAULT]` — Epic's own technical talks, for how Unreal's systems are intended to be used
