# Game Development Track

A long-horizon, self-driven path from "intermediate C++ engineer" to
"game developer", built as a three-layer roadmap plus a coding ladder.

This track is deliberately **build-it-yourself first**. The theory is
learned by writing the thing — a math library, a rasterizer, a physics
solver, an ECS — before touching a commercial engine. That order is the
whole point: it is what makes engine work later feel like using a tool
rather than obeying one. Commercial engines are not skipped, they are
**T15**, after the foundations that make them legible.

---

## The three layers

```
Layer 1   roadmap/INDEX.md                 16 topics, ordered by dependency
   |                                        and priority, grouped into 5 phases
   v
Layer 2   roadmap/T0X-<name>/README.md     subtopics of that topic, ordered,
   |                                        with dependencies + attached work
   v
Layer 3   roadmap/T0X-<name>/S0Y-<name>.md one subtopic, split into
                                            ## Theory        (units U01..UNN,
                                             end-to-end, with real sources)
                                            ## Implementation (assignments)
```

Everything is addressable. `T02.S03.U04` means topic 2, subtopic 3,
theory unit 4. Use that notation when starting a session and there is no
ambiguity about what you are studying.

### Assignments
Assignments are separate from the roadmap tree because they **span** it.
That is intentional — the ladder is designed so concepts recur:

- `roadmap/ASSIGNMENTS.md` — the flat registry: every assignment, its id,
  size, prerequisites, and which concepts it exercises.
- `roadmap/assignments/A0NN-<slug>.md` — one design document per
  assignment: goal, spec, acceptance criteria, review ladder, rubric.
- `code/A0NN-<slug>/` — **you** create this and write the code here.

An assignment never exercises exactly one concept. `A017` (rigid body
solver) re-uses the math library from `A003`, the allocators from `A007`,
the broadphase from `A016`, and the debug renderer from `A012`. Concepts
are meant to be hit from four different angles until they are reflex.

---

## Daily use

Start a session by saying what you are doing. An empty Claude session
picks it up through the `game-dev` skill (`agents/game-dev.md`).

**Studying:**
> "game dev: today I'm on T02.S03 — quaternions. Here's what I understood: ...
> My questions: 1) why is slerp better than nlerp for ... 2) ..."

You get: a correction pass on your understanding, an independent
end-to-end explanation of the unit, then cross-questioning. The whole
discussion is appended to the subtopic doc so it compounds.

**Finishing an assignment:**
> "game dev: I solved A006."

You get: the review ladder run against your actual code in
`code/A006-*/` — correctness, then design, then performance, then an
extension challenge — recorded in the assignment's design doc.

**Other things you can say:**
- "what's next in game dev" — reads the cursor, proposes the next unit
- "expand T05" — generates the Layer-3 docs for a topic that is still stubbed
- "recall check" — spaced-repetition explain-back on an old topic
- "I want to add an assignment for X" — extends the ladder

---

## Files

| path | what it is |
|---|---|
| `roadmap/INDEX.md` | **Layer 1.** Topic map, phases, dependency order, status |
| `roadmap/T0X-*/README.md` | **Layer 2.** Subtopics of a topic |
| `roadmap/T0X-*/S0Y-*.md` | **Layer 3.** Theory + implementation for a subtopic |
| `roadmap/ASSIGNMENTS.md` | Flat assignment registry across all topics |
| `roadmap/assignments/A0NN-*.md` | Assignment design doc + review log |
| `roadmap/CONVENTIONS.md` | Ids, statuses, review ladder, sizing, how to extend |
| `roadmap/SOURCES.md` | Master bibliography — every source the roadmap cites |
| `roadmap/templates/` | Templates for new topics, subtopics, assignments |
| `code/` | Your code. One directory per assignment. |
| `STATE.md` | Cursor: where you are, what's in flight, what's due |

Repo-wide: every session also appends a row to `context/progress.md`
with `type: gamedev`, same as the DSA and system-design tracks.

---

## Assumptions this roadmap was built on

Stated openly so you can override them:

1. **Language: C++** (your primary), with Python for tooling and offline
   pipeline work. Assignments specify C++ unless noted.
2. **Libraries: as few as possible.** SDL3 for window/input/audio device,
   OpenGL 4.x then Vulkan for GPU, Dear ImGui for tools, stb/glTF loaders
   for assets. You write the math, the ECS, the physics, the renderer.
3. **Platform: Linux desktop** primary (matches your machine), with
   cross-platform build hygiene from `T01.S03` so Windows is never far.
4. **Specialization is deferred.** Phases A–D are the shared trunk for
   *every* game programming role. The fork is `T15.S01` — by then you'll
   have enough contact with each area to choose honestly rather than
   picking a title you've only read about.
5. **Cadence: ~45–60 min/day** for theory (matching `context/profile.md`),
   with assignments absorbing weekend/longer blocks. The sizing tags in
   `ASSIGNMENTS.md` assume that.
