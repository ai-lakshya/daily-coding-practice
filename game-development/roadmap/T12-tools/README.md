# T12 — Tools, Asset Pipeline & Editors
**Phase:** D | **Depends on:** T03, T06, T08 | **Borrows from:** —
**Status:** stub | **Assignments:** A039–A041

## Why this topic, here
Phase D is where a hobby codebase becomes a professional one, and tooling
is the first move. The reason is economic: from here on, **your iteration
time is your throughput**. Every second between changing something and
seeing it is multiplied by thousands of iterations.

It comes after `T08` because the asset pipeline needs real assets to have
opinions about — meshes, materials, animations, audio — and after `T06`
because the editor is editing the data-driven content you built there.

Tools engineering is also, pragmatically, one of the easiest ways into
the industry and one of the least contested. It is worth taking seriously
rather than treating as chores.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | The Asset Pipeline | Source vs intermediate vs runtime formats, offline baking, dependency graphs and incremental rebuilds, content hashing and caches, determinism, platform variants | T03.S05 | 6 | A039 | stub |
| S02 | Importers & Formats | glTF as the practical mesh/anim interchange, image formats and GPU block compression (BC/ASTC), audio encoding choices, what to strip and what to precompute at bake time | S01, T09.S01 | 5 | A039 | stub |
| S03 | In-Engine Tooling | Dear ImGui and immediate-mode tooling, entity inspectors driven by reflection, gizmos and manipulators, selection/picking, undo/redo as a command stack, and writing back to source data | T06.S02, T06.S05 | 7 | A040 | stub |
| S04 | Iteration Speed | Hot reload of shaders → data → code (the DLL-swap model and its constraints), live tuning, compile-time reduction (PCH, unity builds, dependency hygiene), and measuring your own edit-run loop | S03 | 6 | A041 | stub |
| S05 | Version Control & Team Workflow for Games | Why games break normal git workflows, LFS vs Perforce and the lock-based model, binary assets and merge conflicts, build artifacts, branching for content teams | S01 | 4 | — | stub |

## Exit criteria
- [ ] Your baker rebuilds only what changed, and you can prove it with a timing comparison on a no-op run.
- [ ] Your runtime loads a binary asset with zero parsing beyond a header — and you can state the load-time difference vs the source format.
- [ ] You can explain why the baker must be deterministic and what breaks in a team when it isn't.
- [ ] Your editor can select, inspect, move, and save an entity, with undo working across all of it.
- [ ] You can explain the constraints on hot-reloading C++ gameplay code (state, vtables, statics) and what your implementation does about each.
- [ ] You have measured your edit → see-it loop for a shader, a tuning value, and a code change, and reduced at least one of them by 5x.

## Traps specific to this topic
- **Building an editor before you have content to edit.** The order in
  `A039` → `A040` is the right one.
- **Reflection systems as a side quest.** You need enough to inspect
  components. A full C++ reflection system is a multi-month project and a
  well-known way to stop making games.
- **Non-incremental bakes.** A pipeline that rebuilds everything is a
  pipeline nobody uses; they will edit the runtime files by hand instead.
- **Ignoring `S05` because you work alone.** LFS and a sane repo layout
  cost an hour now; retrofitting them onto a repo with 4 GB of history is
  a weekend.

## Primary sources for the whole topic
- `[GEA3]` ch. 6 (resources & the file system), ch. 7 (tools), ch. 10 (the tools pipeline) — the spine for this topic
- `[IMGUI]` — the demo window source is the documentation; read `imgui_demo.cpp` directly
- `[GLTF]` — the spec, for `S02`
- `[GDCVAULT]` — talks on build/content pipelines; the *Destiny* and *Frostbite* pipeline talks are the classics
- `[CMAKEBOOK]` — return to it for `S04`'s build-time work
