# T03.S01 — The Game Loop & Time
**Topic:** [T03 — The Game Loop & Engine Core](README.md) | **Depends on:** T01.S02
**Status:** expanded | **Assignments:** A006 | **Est. theory time:** 2 sessions

> The question this subtopic answers: *how do I advance a simulation by
> the right amount of time, so that the game behaves identically on every
> machine and still looks smooth?*
>
> This is the most consequential single design decision in the roadmap.
> Get it wrong and physics behaves differently at different framerates,
> replays don't reproduce, netcode is impossible, and every one of those
> problems is discovered much later and blamed on something else.

---

## Theory

### U01 — Why a naive loop is wrong
**What it is.** The obvious loop, and the three distinct ways it fails.

**Why it exists.** Because it is what everyone writes first, and because
each of its failures teaches you what the real solution is buying.

**The mechanism.** The naive versions, in the order people write them:

**Attempt 1 — no time at all:**
```cpp
while (running) { poll(); update(); render(); }
// player moves `speed` units per FRAME
```
The game runs at whatever speed the hardware allows. This is why old DOS
games are unplayably fast on modern machines — a genuine historical
artifact of this exact bug. Immediately obviously wrong.

**Attempt 2 — variable timestep:**
```cpp
while (running) {
    float dt = seconds_since_last_frame();
    poll(); update(dt); render();
}
```
This is correct for *linear* quantities and subtly wrong for everything
else. Three separate problems:

1. **Nonlinear integration error depends on `dt`.** Gravity integrated
   with `v += g*dt; p += v*dt` gives a different trajectory for one 32 ms
   step than for two 16 ms steps — jump heights literally differ by
   framerate. (`T05.S01` has the details; the roadmap-level point is that
   the error is `O(dt)`, so `dt` variation is trajectory variation.)
2. **Collision and constraint solvers become unstable at large `dt`.** A
   solver tuned for 16 ms steps will jitter or explode at 100 ms. And
   large `dt` values arrive whenever a level loads or the user drags the
   window.
3. **Nothing is reproducible.** `dt` is never the same twice, so the same
   inputs never produce the same result. Replays, deterministic netcode,
   and reproducible bug reports are all off the table permanently.

**Attempt 3 — clamp the timestep:**
```cpp
float dt = min(seconds_since_last_frame(), 0.05f);
```
Prevents the explosion, and now the game runs in *slow motion* whenever
the frame is slow, because you're simulating less time than has elapsed.
Better failure mode, still a failure.

**Where it bites.** Attempt 2 is what most tutorials show, and it works
fine for a Pong with linear velocities — which is exactly why the bug
survives until you add gravity or a solver, at which point the cause is
several weeks behind you.

**Read.**
- `[GAFFER]` "Fix Your Timestep!" — https://gafferongames.com/post/fix_your_timestep/ — read the whole thing now. It is the single most-cited article in game programming and it earns it.
- `[GPP]` "Game Loop" — https://gameprogrammingpatterns.com/game-loop.html — the same progression, from a patterns angle.

**Check yourself.**
1. Give a concrete case where variable `dt` changes gameplay outcome, with numbers.
2. Why does clamping `dt` produce slow motion rather than fixing anything?
3. Which quantities *are* correctly handled by a variable timestep?

### U02 — The fixed timestep with an accumulator
**What it is.** Simulate in fixed-size steps; render as often as you can;
carry the leftover time forward.

**Why it exists.** It decouples simulation rate from render rate, which
gives you framerate-independent physics *and* smooth rendering at any
refresh rate, and it makes the simulation reproducible.

**The mechanism.** The canonical loop:

```cpp
const double FIXED_DT = 1.0 / 60.0;   // simulation step, seconds
const double MAX_FRAME = 0.25;        // spiral-of-death guard

double accumulator = 0.0;
double prev = now();

while (running) {
    double curr  = now();
    double frame = curr - prev;
    prev = curr;

    if (frame > MAX_FRAME) frame = MAX_FRAME;   // see U03
    accumulator += frame;

    poll_input();                                // once per RENDER frame

    while (accumulator >= FIXED_DT) {
        previous_state = current_state;          // for interpolation, U04
        simulate(FIXED_DT);                      // always the same dt!
        accumulator -= FIXED_DT;
    }

    double alpha = accumulator / FIXED_DT;       // in [0, 1)
    render(lerp(previous_state, current_state, alpha));
}
```

Read the inner `while` carefully — it may run **zero, one, or many times**
in a given frame:

```
  render @144Hz (6.9ms), sim @60Hz (16.7ms):
    frame 1: acc=6.9   -> 0 sim steps, render interpolated
    frame 2: acc=13.8  -> 0 sim steps
    frame 3: acc=20.7  -> 1 sim step, acc=4.0
    ...  simulation runs at exactly 60Hz regardless of render rate

  render @30Hz (33.3ms), sim @60Hz:
    every frame: acc=33.3 -> 2 sim steps, acc=0
    ...  still exactly 60Hz of simulation per second of wall time
```

**Choosing `FIXED_DT`.** 60 Hz is the common default. Physics-heavy games
often run 120 Hz for solver stability, sometimes with gameplay at 30 Hz.
Fighting games use 60 Hz because the design is expressed in frames. The
constraint: it must be fast enough that a typical frame's steps fit in the
budget, with room for a catch-up step.

**Where it bites.**
- **Reading input inside the inner loop.** Input is sampled once per render
  frame; if you poll inside the sim loop you get the same input applied
  twice on a double-step frame, or you introduce ordering weirdness. The
  correct model: input is sampled once, and the *same* input state is fed
  to each of that frame's simulation steps.
- **Non-simulation work inside the inner loop.** Anything cosmetic
  (particles that don't affect gameplay, audio triggers, UI animation)
  should run once per render frame, not per sim step, or it happens twice
  as often on catch-up frames.
- **`float` accumulator.** Use `double` for time. A `float` accumulating
  16 ms increments loses meaningful precision within hours of runtime.

**Read.**
- `[GAFFER]` "Fix Your Timestep!" — the "Free the Physics" section specifically.
- `[GEA3]` §8.5.4–8.5.6 — the engine-scale treatment, including multi-rate updates.

**Check yourself.**
1. At 144 Hz render and 60 Hz sim, what fraction of frames run zero simulation steps?
2. Why must input be polled outside the inner loop, and what is fed to each step?
3. Why `double` for the accumulator?

### U03 — The spiral of death
**What it is.** The failure mode where simulation catch-up makes the next
frame slower, requiring more catch-up, until the game locks up.

**Why it exists.** The inner `while` loop's work is proportional to elapsed
time. If simulating N steps takes longer than N steps of wall clock, the
accumulator grows faster than it drains and the loop never exits.

**The mechanism.**
```
  frame takes 50ms  ->  3 sim steps needed
  3 sim steps take 60ms  ->  next frame's accumulator is even larger
  ->  4 steps needed  ->  80ms  ->  5 steps  ->  ...  ->  frozen
```
This is a positive feedback loop with no natural equilibrium. It is
triggered by: a level load, alt-tabbing away and back, a debugger
breakpoint, or simply a machine slower than the game was tuned for.

**The fixes, all of which you should have:**

1. **Clamp the frame time** (`MAX_FRAME` in U02). This is the essential
   one. It means "if more than 250 ms elapsed, pretend it was 250 ms".
   The game runs briefly in slow motion, which is a vastly better failure
   than freezing. The clamp is not optional — it is the safety valve.
2. **Cap the steps per frame** (e.g. max 5). Equivalent effect, expressed
   as a step count.
3. **Detect and report.** Log when the clamp fires. Repeated clamping
   means the machine can't sustain the game, which is information the
   player (and you) want.
4. **Reset after known stalls.** After loading a level or regaining focus,
   zero the accumulator and reset `prev = now()`. Don't try to simulate
   the eight seconds the loading screen took.

**Where it bites.** Debugging. A breakpoint for thirty seconds means
thirty seconds of accumulated time; without the clamp, resuming freezes
the game and you conclude your breakpoint broke something. Every developer
meets this once.

**Read.**
- `[GAFFER]` "Fix Your Timestep!" — "The Spiral of Death" section.
- `[GEA3]` §8.5.6.

**Check yourself.**
1. Explain the spiral as a feedback loop. What breaks it?
2. Why is slow motion an acceptable failure mode and freezing not?
3. What should happen to the accumulator after a two-second level load?

### U04 — Render interpolation
**What it is.** Rendering at `lerp(previous_state, current_state, alpha)`
rather than at the current simulation state.

**Why it exists.** With a fixed timestep, the simulation is almost never
exactly aligned with the render moment. Rendering the raw simulation state
means the visual position quantizes to sim ticks, producing **temporal
aliasing** — a subtle stutter that is very visible on smooth motion and
that people usually misattribute to a framerate problem.

**The mechanism.** `alpha = accumulator / FIXED_DT` is "how far past the
last simulation tick are we, as a fraction". Rendering at that fraction
between the previous and current state puts the visual exactly where the
object would be at the render instant.

```
  sim ticks:     |--------|--------|--------|      (60Hz)
  render frames: | | | | | | | | | | | | | |       (144Hz)
                     ^
                     render here, alpha=0.42
                     -> draw at lerp(prev, curr, 0.42), not at `prev`
```

Without it, at 144 Hz render / 60 Hz sim, you draw the same position for
2–3 consecutive frames and then jump. The motion is *mathematically*
correct and *visually* stuttery.

**What to interpolate.** Positions and rotations (slerp — `T02.S03.U04`).
Not: booleans, discrete state, health values, anything where an
intermediate value is meaningless. This means your renderable state is a
subset of your simulation state, which is a useful structural pressure —
it pushes you toward separating "what the simulation knows" from "what the
renderer needs", which pays off in `T13` and `T14`.

**The cost.** One extra copy of the interpolatable state per entity per
step, and a lerp per entity per frame. Real, and worth it.

**Extrapolation as the alternative.** Predict forward from the current
state using velocity instead of interpolating between two known states.
Saves the extra state copy and removes the one-tick latency that
interpolation adds, but produces visible corrections when the prediction
is wrong (a collision the extrapolation didn't know about). Interpolation
is the right default; extrapolation is what `T14.S04` uses for *remote*
entities, where you have no choice.

**Where it bites.** Skipping interpolation and then tuning
`FIXED_DT` upward to hide the stutter. You end up at 120 or 144 Hz
simulation, paying double the CPU cost, to fix a problem that a lerp
solves for free.

**Read.**
- `[GAFFER]` "Fix Your Timestep!" — "The Final Touch" section.
- `[GEA3]` §8.5.5.

**Check yourself.**
1. Why does rendering the raw sim state stutter at 144 Hz even when the sim is perfectly regular?
2. What is `alpha` and what are its bounds?
3. Which state should not be interpolated? Give three examples.
4. What does interpolation cost in latency, and why is that acceptable?

### U05 — Time as a system: pause, slow motion, and multiple clocks
**What it is.** Recognising that "time" is several different things and a
game needs to distinguish them.

**Why it exists.** Because pause is not `if (paused) return;` — the UI
still animates, the pause menu still responds to input, and the audio
still needs to fade. And hitstop (`T07.S02`) requires slowing gameplay
time without slowing the UI.

**The mechanism.** Maintain at least three clocks:

| clock | affected by pause/slowmo | drives |
|---|---|---|
| **real time** | no | profiling, UI animation, networking timeouts, audio |
| **game time** | **yes** | simulation, gameplay timers, animation, physics |
| **unscaled game time** | pause yes, slowmo no | some UI, some cosmetic effects |

```cpp
struct Time {
    double real_dt;       // wall clock, always
    double dt;            // real_dt * time_scale, zero when paused
    double time_scale;    // 1.0 normal, 0.3 slow-mo, 0.0 paused
    double real_elapsed;
    double elapsed;       // accumulated `dt` — the simulation's clock
    uint64_t tick;        // simulation step count, THE canonical time for
                          // anything that must be deterministic
};
```

**The tick counter is the important one.** For anything that has to be
reproducible — cooldowns, spawn timers, replay events, network state — use
the integer tick count, not accumulated floating-point seconds. Ticks are
exact, they don't drift, and comparing them is exact. `A021` and `A047`
depend on this.

Time scaling with a fixed timestep: scale the *amount added to the
accumulator*, not `FIXED_DT` itself. `FIXED_DT` must stay constant or you
have lost everything U02 bought you.
```cpp
accumulator += frame * time_scale;    // right
// FIXED_DT *= time_scale;            // wrong: destroys determinism
```

**Where it bites.** Using real time for a gameplay timer. The cooldown
keeps ticking while paused; the ability is ready when the player unpauses.
Small bug, very visible, and a symptom of not having made this distinction
structurally.

**Read.**
- `[GEA3]` §8.5.7 — multiple clocks, and the engine's time system.
- `[GPP]` "Update Method" — https://gameprogrammingpatterns.com/update-method.html

**Check yourself.**
1. Name three things that must keep running while the game is paused.
2. Why use tick counts rather than accumulated seconds for gameplay timers?
3. How do you implement 0.3x slow motion without changing `FIXED_DT`?

### U06 — Where the loop goes from here
**What it is.** A forward look at how this loop is modified by later
topics, so the design you write in `A006` has room for them.

**Why it exists.** Two of the later modifications are hard to retrofit, and
knowing they're coming shapes the interfaces you write now.

**The mechanism.**

- **Multi-rate updates.** Not everything needs 60 Hz. AI decision-making
  at 10 Hz, distant entity updates at 5 Hz, and pathfinding time-sliced
  across frames are all standard (`T10`). Structure: give subsystems an
  update rate and a phase offset so they don't all land on the same tick
  (the "bucketing" approach — `[AIFG]` ch. 11).
- **Fixed-step rendering decoupling** already done in U02/U04.
- **Job-based parallelism** (`T13.S05`). The loop becomes a task graph:
  the phases from `T01.S01.U01` become nodes with dependencies, and
  independent work within a phase runs in parallel. This is a large change
  and the thing that makes it tractable is having phases that are already
  cleanly separated — which is an argument for keeping `simulate()`
  structured as explicit ordered phases now, rather than as a soup of
  `entity->Update()` calls.
- **Networked time** (`T14`). The server's tick becomes authoritative and
  the client's loop runs ahead or behind it. If your loop's time comes from
  a single `Time` struct that the netcode can drive, this is a
  modification; if `now()` is called from thirty places, it is a rewrite.
- **Frame pipelining.** Letting the CPU work on frame N+1 while the GPU
  renders frame N (already implicit) and, more aggressively, running
  simulation for N+1 while rendering N. Adds a frame of latency; used when
  throughput matters more.

**Where it bites.** Scattered `now()` calls and scattered `deltaTime`
plumbing. Funnel time through one struct passed explicitly, and all of the
above stays possible.

**Read.**
- `[GEA3]` §8.5.4 — multi-rate and the engine loop's real structure.
- `[NAUGHTYDOG-JOB]` — for where this ends up at scale. Watch now for the shape, revisit at `T13.S05`.

**Check yourself.**
1. Why run AI at 10 Hz rather than 60 Hz, and what problem does phase-offsetting solve?
2. What about your loop's design would make job-based parallelism a rewrite rather than a modification?
3. Why should `now()` be called in exactly one place?

---

## Implementation

| assignment | size | target rung | what it forces you to understand |
|---|:---:|:---:|---|
| [A006](../assignments/A006-fixed-timestep-pong.md) | M | R3 | A loop that is *provably* framerate-independent — with the test that proves it |

**Order and overlap.** `A006` covers `S01` and `S02` (input) together,
because a loop with no input isn't testable. The critical requirement in
its spec is the **determinism test**: run the same recorded input at 30,
60, 144, and 1000 FPS, hash the simulation state at tick 600, and assert
all four hashes are equal. That test is four lines and it is the
difference between believing your loop is correct and knowing it.

That state-hash idea then recurs: `A021` uses it for replays, `A047`
uses it for desync detection, and `A042` uses the same tick counter for
profiling. `A006`'s `Time` struct is used unchanged for the rest of the
roadmap.

**If you are short on time.** The accumulator (U02) and the clamp (U03)
are non-negotiable. Interpolation (U04) can be added a week later —
but add it, because without it you will over-tune `FIXED_DT`. The
multi-clock system (U05) can start as just `time_scale` and grow.

---

## Discussion Log
<!-- Appended per study session. See ../CONVENTIONS.md §7. -->

## Open Questions

## Connections
- `T01.S02.U04` — the frame-time measurement that tells you whether your loop is healthy.
- `T01.S02.U05` — determinism, of which the fixed timestep is the precondition.
- `T02.S05.U01` — the framerate-independent lerp, which is the same lesson for non-simulation values.
- `T05.S01` — why the integrator specifically requires a fixed `dt`.
- `T14.S04`, `T14.S05` — where this loop's determinism is finally cashed in.
