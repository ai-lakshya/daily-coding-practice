# T07 — Game Feel, Juice & Design Fundamentals
**Phase:** B | **Depends on:** T04, T05, T06 | **Borrows from:** T11 (sound is roughly half of perceived feel — do `A037` before `A023` if you can)
**Status:** stub | **Assignments:** A023–A024

## Why this topic, here
This is the topic that most engineering-led roadmaps leave out, and its
absence is why there are so many technically impressive projects that are
not fun to touch for ten seconds.

It sits mid-graph deliberately. You need enough systems to *have* feel
(`T04`–`T06`), and you need to learn it before `T08`+ so that the second
half of the roadmap is spent making things that feel good rather than
making things that render well. A developer who learns feel late tends to
build engines; a developer who learns it here builds games.

It ends in **M2**: a complete, packaged, playable 2D game. That artifact
is worth more to you than the next three topics combined.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | Game Feel: Input, Response, Context | Swink's three-part model, input latency budgets, response curves, animation-driven vs physics-driven motion, the "toy" test | T05.S05 | 6 | A024 | stub |
| S02 | Juice: The Technique Catalogue | Screenshake, hitstop/freeze frames, tweening and easing, squash & stretch, particles, trails, sound layering, camera lead and lag | S01, T04.S06 | 6 | A024 | stub |
| S03 | Core Loops & Systems Design | Verbs, the moment-to-moment loop, feedback loops, economies, progression and difficulty curves, and designing for a target emotion | — | 6 | A023 | stub |
| S04 | Level Design & Pacing | Teaching without tutorials, affordances and readability, encounter and rhythm design, the introduce → develop → twist → conclude structure | S03 | 5 | A023 | stub |
| S05 | Playtesting & Iteration | Running a test, what to watch instead of what to ask, telemetry basics, separating "what they felt" from "what they suggested" | S03, S04 | 5 | A023 | stub |

## Exit criteria
- [ ] You can take a movement system and produce a measurably better-feeling version, and enumerate every change you made and why.
- [ ] You have an A/B build (`A024`) and other people can tell the difference blind.
- [ ] You can state your game's core loop in one sentence with a verb in it.
- [ ] You can point at a level you designed and explain what each section teaches.
- [ ] You have watched at least three people play your game without helping them, and written down what you changed as a result.
- [ ] `A023` is packaged, runs on a machine that is not yours, and has a title screen and an ending.

## Traps specific to this topic
- **Treating juice as a post-processing step.** Feel is designed into the
  controller in `S01`; `S02` amplifies what is already there. Screenshake
  on a mushy character controller is lipstick.
- **Scope explosion in `A023`.** It is `XL` and it will still try to
  double. The design doc's cut list is not decoration — write it before
  you start and cut from the top when you slip.
- **Asking playtesters what to change.** They are excellent at reporting
  what they felt and unreliable at diagnosing why. `S05` is mostly about
  this distinction.
- **Believing you cannot do art.** You can ship with primitives, a
  three-colour palette, and good motion. Motion is the part you control.

## Primary sources for the whole topic
- `[GAMEFEEL]` — Swink. The whole book; it is short and it is the canon here
- `[JUICE]` — "Juice it or lose it" (12 min). Watch it before `A024`, then again after
- `[LENSES]` — Schell, for `S03`–`S04`. Use it as a question bank, not a read-through
- `[CELESTE]` — the physics/feel writeup, and Celeste's own level design talks on GDC Vault
- `[GDCVAULT]` — "Level Design Workshop" sessions; and Nintendo's *4 Steps* level design structure
