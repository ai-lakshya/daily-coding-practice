# A006 — Framerate-Independent Pong
**Topic:** T03.S01–S02 | **Size:** M (1–3 days) | **Target rung:** R3
**Status:** todo
**Code:** `game-development/code/A006-fixed-timestep-pong/`
**Reuses:** A002 (skeleton), A003 (math), A005 (the damping helper) | **Reused by:** A008 (rebuilt on ECS), A012, A018, A021, A045, A047

## Goal
A game loop you can build the next two years on: fixed timestep with
interpolation, an action-mapped input layer that can be recorded and
replayed, and a **test that proves** the simulation is identical at every
framerate.

The game is Pong because the game is not the point. The loop is the point.

## Concepts exercised
| unit | first met in | why it recurs here |
|---|---|---|
| T03.S01.U01–U06 | here | The whole loop: accumulator, spiral guard, interpolation, clocks |
| T03.S02.U01–U05 | here | Events→state→actions, edges across variable step counts, latency |
| T01.S02.U05 | T01 | Determinism, now made real and tested |
| T01.S04.U03 | A001 | The histogram as the frame overlay |
| T02.S05.U01 | A005 | Framerate independence, now structural rather than per-value |

## Prerequisites
`A002`, `A003`, `A005`.

## Specification

### The loop
1. Fixed timestep with accumulator, `FIXED_DT = 1/60`, exactly as
   `T03.S01.U02`.
2. Frame-time clamp (`MAX_FRAME = 0.25`) and a log line when it fires.
3. Accumulator reset after a stall (add a debug key that sleeps 2 seconds,
   and verify the game does not fast-forward afterward).
4. **Render interpolation** with `alpha`. Add a runtime toggle so you can
   see the stutter with it off — at a render rate that is not a multiple
   of 60.
5. A `Time` struct per `T03.S01.U05`: `real_dt`, `dt`, `time_scale`,
   `elapsed`, and a `uint64 tick`. All gameplay timers use `tick`.
6. Slow motion (`time_scale = 0.3`) and pause (`time_scale = 0`) on keys,
   implemented by scaling what is added to the accumulator — `FIXED_DT`
   must not change.

### Input
7. Event→polled-state conversion with `down`/`pressed`/`released` edges,
   including the sub-frame press case (`T03.S02.U01`).
8. An **action layer**: `enum class Action`, analog values, bindings
   loaded from a config file, and at least two devices mapped
   (keyboard + gamepad if you have one, keyboard + a second key set if not).
9. **`TickInput`**: the simulation takes input as an explicit parameter,
   never reads a global. Edges are latched and consumed once per frame
   (`T03.S02.U03`), so a two-step catch-up frame does not double-fire.

### The game
10. Pong: two paddles, a ball, walls, scoring, a serve after each point.
11. Ball speed increases with each volley (so the physics is
    non-trivially framerate-sensitive if you get it wrong).
12. An AI opponent driven **through the same `TickInput` interface** — not
    by directly setting the paddle position. This constraint is the whole
    reason the action layer exists.

### The proof
13. **Input recording and playback**: record the `TickInput` stream to a
    file with its tick number; a `--replay <file>` mode feeds it back.
14. **A state hash**: a `uint64` over all simulation state (positions,
    velocities, scores, RNG state), computed per tick.
15. **The determinism test** — the deliverable that matters:
    ```
    run the same recorded input with the render loop capped at
    30, 60, 144, and 1000 FPS; assert the state hash at tick 600
    is identical in all four runs.
    ```
    This must be an automated test, not a manual check.

### Constraints
- `simulate()` must not call `now()`, read input globally, or touch
  rendering. It takes `(FIXED_DT, TickInput, State&)` and nothing else.
- No allocation inside `simulate()`.
- Cosmetic effects (screen flash on score, ball trail) run once per render
  frame, not per simulation step.

### Explicitly out of scope
ECS (that's `A008`), sprites and textures (that's `A012`), sound,
menus, a real physics engine.

## Acceptance criteria
- [ ] The determinism test passes at all four framerates, automated.
- [ ] Disabling render interpolation produces visible stutter at 144 Hz; enabling it removes it.
- [ ] The 2-second-stall key does not cause a fast-forward or a freeze.
- [ ] A quick tap of a paddle key (shorter than one frame) still registers — test by pressing as fast as you can and confirming no input is dropped, at 144 Hz render.
- [ ] Pause stops the ball but the UI still animates and the pause overlay responds to input.
- [ ] `--replay` reproduces a recorded match exactly, including the final score.
- [ ] The AI opponent's paddle is moved only by `TickInput` values it produces.

## Deliverables
- `code/A006-fixed-timestep-pong/`
- `NOTES.md` — the state-hash design (what you included and what you deliberately excluded, and why), and what broke the first time you ran the determinism test

## Design notes / hints
- Write the determinism test **early**, before the game is finished. It
  will fail, and the reason it fails is the lesson. Common first causes:
  cosmetic state included in the hash, a `float` accumulator, input read
  in the wrong place, an RNG shared between simulation and effects.
- The state hash should cover exactly what the simulation owns. If a
  particle position is in the hash, cosmetic changes break determinism for
  no benefit. The sim/cosmetic split (`T02.S05.U05`) is what makes this
  clean.
- FNV-1a over the raw bytes of the simulation state is fine. Be careful
  with padding bytes in structs — they are uninitialized and will differ
  between runs. Either hash fields explicitly or `memset` the state.
- For the 1000 FPS run, uncap and let it spin. Most frames will run zero
  simulation steps, which is exactly the case that exposes dropped edges.

## Cut list
1. The gamepad binding (keep two keyboard binding sets).
2. Slow motion (keep pause).
3. The AI opponent (make it two-player).
4. Ball speed escalation.
5. **Never cut: the determinism test, the action layer, or `TickInput` as
   an explicit parameter.** Those three are what `A021`, `A046`, and
   `A047` are built on.

## Rubric

| rung | passes when |
|---|---|
| R0 | Builds clean, runs, all acceptance criteria met |
| R1 | Determinism test green at all four rates. No dropped or doubled inputs at any rate. Alt-tab, window resize, and a debugger pause all recover without fast-forward. Ball never tunnels through a paddle at the highest speed (or if it can, you have documented it as a known limit and said what `A015` will fix). |
| R2 | `simulate()` has the signature above and nothing more. The `Time`, `Input`, and `Histogram` modules are reusable — droppable into `A008` unedited. Cosmetic and simulation state are in separate structs, and you can point at the boundary. |
| R3 | Frame time reported with the `A001` histogram at all four rates. Simulation step cost measured separately from render cost. Confirm `simulate()` allocates zero bytes (instrument `operator new`). |
| R4 | See extension below |

### R4 extension
Add **rollback**: keep a ring buffer of the last 16 simulation states.
Add a debug key that rewinds 10 ticks and re-simulates forward with the
recorded input, and assert the resulting state hash matches what it was
originally. This is a two-hour extension that is a direct rehearsal for
`A047`, and it will find any remaining hidden state in your simulation —
which is the real reason to do it.

## Common pitfalls
- Polling input inside the accumulator loop (doubles inputs on catch-up frames).
- Polling input after `simulate()` (adds a frame of latency, silently).
- Rendering `current_state` instead of the interpolated state.
- A `float` accumulator.
- Hidden state: a `static` local in a simulation function, an uninitialized
  struct field, or a `std::unordered_map` iterated in the sim. All three
  break determinism and all three are found by the R4 rollback test.
- Padding bytes in the hashed state, differing between runs.

## Review Log
<!-- One dated block per rung. See ../CONVENTIONS.md §4. -->

## Recall schedule
| due | rung | done | result |
|---|---|---|---|
