# T01 — Real-Time Programming Foundations
**Phase:** A | **Depends on:** — | **Borrows from:** —
**Status:** expanded | **Assignments:** A001–A002

## Why this topic, here
Every other topic in this roadmap assumes two things you probably do not
have yet: an instinct for the **frame budget** (that a game is a program
with a deadline 16.67 ms away, over and over, forever), and a toolchain
where building, debugging, and *measuring* a C++ program are not events.

You already know C++. This topic is not a C++ course. It is the shift
from "correct program" to "program that must finish on time", plus the
one week of tooling setup that stops the next two years being annoying.

Keep it short. Four subtopics, two small assignments, then move on — the
tooling gets refined continuously afterwards, not perfected here.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | [The Discipline Map & Anatomy of a Game](S01-discipline-map.md) | What a game program actually is, what an engine is made of, what roles exist and what they do all day | — | 4 | — | expanded |
| S02 | [The Real-Time Constraint](S02-real-time-constraint.md) | Frame budgets, the latency chain from input to photon, vsync, jitter, and why averages lie | S01 | 5 | A001 | expanded |
| S03 | [The C++ Game Toolchain](S03-toolchain.md) | CMake, dependencies, warnings, sanitizers, debuggers — a build you trust | — | 5 | A002 | expanded |
| S04 | [Measurement Discipline](S04-measurement.md) | Clocks that don't lie, histograms over averages, and your first profiling pass | S02, S03 | 4 | A001 | expanded |

Do `S03` in parallel with `S01`/`S02` if you like — it has no theoretical
dependency, and having the build working makes `A001` possible.

## Exit criteria
- [ ] You can state your frame budget in milliseconds for 60 Hz and 144 Hz, and name at least four things competing for it.
- [ ] You can explain why a 60 FPS *average* can feel worse than a 50 FPS average, in terms of the frame-time distribution.
- [ ] You can trace the full input-to-photon latency chain and name every buffer in it.
- [ ] You have a CMake project that builds clean with `-Wall -Wextra -Werror`, runs under ASan/UBSan, and opens an SDL3 window on your machine.
- [ ] You can time a section of code correctly — right clock, no measurement overhead in the measurement, reported as percentiles.
- [ ] You can name the major subsystems of a game engine and say which of them talk to which, in order, within one frame.

## Traps specific to this topic
- **Toolchain perfectionism.** You can spend a month on your build system
  and learn nothing about games. `A002` is capped at 4 hours on purpose.
- **Benchmarking with `-O0`.** Every performance number you take from a
  debug build is fiction. This will bite you again in `T13`.
- **Averaging frame times.** The single most common measurement error in
  games. `S04` exists mainly to kill this habit before it forms.
- **Reading `[GEA3]` front to back.** It is 1200 pages and it is a
  reference. Read ch. 1 and ch. 8 now; come back per topic.

## Primary sources for the whole topic
- `[GEA3]` ch. 1 (What is a game engine?), ch. 8 (The game loop / real-time simulation)
- `[GPP]` — the introduction and "Game Loop" chapter
- `[CMAKEBOOK]` — reference, not cover-to-cover
- `[SDLDOCS]`
