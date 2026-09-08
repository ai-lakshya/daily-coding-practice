# T06 — Gameplay Architecture & Data-Driven Design
**Phase:** B | **Depends on:** T03, T05 | **Borrows from:** —
**Status:** stub | **Assignments:** A019–A022

## Why this topic, here
By now you can render and simulate. `T06` is about the code that actually
*is the game*, and it fails differently from engine code: it is written
under time pressure, changed constantly, and has requirements that are
discovered rather than specified. Architecture that is correct for a
renderer is often wrong here.

The through-line is **iteration speed**. Every pattern in this topic
earns its place by shortening the loop between "I want to change a
number" and "I see what that number does". Data-driven design, hot
reload, and scripting are all the same idea attacked from three sides.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | Gameplay Patterns | State machines and hierarchical FSMs, command, observer/event bus, service locator, update method, deferred/double-buffered state — and when each becomes a liability | T03.S04 | 7 | A019 | stub |
| S02 | Data-Driven Design & Content Formats | Moving tuning out of code, JSON/TOML/binary trade-offs, schemas and validation, hot reload, and where data-driven becomes a worse programming language | S01 | 6 | A020 | stub |
| S03 | Scripting Integration | Embedding Lua, the C API and the stack, binding objects, ownership across the boundary, sandboxing, performance, and when *not* to script | S02 | 5 | A022 | stub |
| S04 | Serialization & Save Systems | Versioning and migration, stable ids, save-anywhere vs checkpoint, determinism, and input-recording replays | S02, T03.S05 | 6 | A021 | stub |
| S05 | Game State Flow & UI | Scene/screen stacks, transitions, pause semantics, immediate vs retained mode UI, HUD data flow, and input focus | S01 | 5 | A019 | stub |

## Exit criteria
- [ ] You can name a concrete case where an event bus made debugging harder, and say what you'd do instead.
- [ ] Your platformer's enemy behaviour is defined in data, and you changed one and saw it without a rebuild.
- [ ] You can explain the ownership rules across the Lua/C++ boundary and what happens when each side is wrong about them.
- [ ] Your save format survives a field being added, removed, and renamed — and you have tests for all three.
- [ ] You can record an input stream and replay it to a bit-identical outcome, and explain every requirement that makes that possible.
- [ ] You can explain why pause is harder than it looks (what still updates, and why).

## Traps specific to this topic
- **Over-abstracting.** Gameplay code has the highest change rate and the
  shortest lifetime in the codebase. Three lines of duplication beats a
  wrong abstraction, and `[MIKEACTON]`'s complaint applies double here.
- **Event buses everywhere.** They decouple sender from receiver, which
  also means you cannot answer "what happens when I do this?" by reading.
  Use them where the decoupling is genuinely needed.
- **Data-driving the wrong things.** Data-driven tuning: yes. Data-driven
  control flow: you have built a bad interpreter.
- **Deferring saves.** Retrofitting serialization onto a codebase that
  never considered it is one of the classic multi-week tax bills. `A021`
  is small deliberately, and early deliberately.

## Primary sources for the whole topic
- `[GPP]` — this is the topic that book was written for; read it nearly entirely
- `[GEA3]` ch. 15–16 (runtime gameplay foundation systems)
- `[OVERWATCH-ECS]` — Timothy Ford's GDC talk on gameplay architecture at scale
- Lua 5.4 reference manual + *Programming in Lua* ch. 24–28 (the C API) — https://www.lua.org/manual/5.4/
