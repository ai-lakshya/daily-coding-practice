# T02.S05 — Interpolation, Curves & Numerical Care
**Topic:** [T02 — Math for Games](README.md) | **Depends on:** T02.S01, T02.S03
**Status:** expanded | **Assignments:** A004, A005 | **Est. theory time:** 2 sessions

> The question this subtopic answers: *how do I move a value from A to B
> over time in a way that looks intentional?* Interpolation is the
> mathematical substrate of game feel — `T07` is largely this subtopic
> applied with taste.
>
> It also collects the floating-point care that the rest of `T02` has been
> deferring, because this is where it starts producing visible artifacts.

---

## Theory

### U01 — Lerp, and the framerate trap
**What it is.** `lerp(a, b, t) = a + (b - a) * t`. The most-used function
in games.

**Why it exists.** Everything continuous is a lerp: positions, colours,
volumes, camera FOV, health bars, blend weights.

**The mechanism.** Two forms, not equivalent in floating point:
```cpp
a + (b - a) * t          // precise: exactly a at t=0; may overshoot at t=1
a * (1 - t) + b * t      // "monotonic": exactly b at t=1; a*(1-t) may drift
```
Use the first for most things. `std::lerp` (C++20) handles both endpoints
exactly *and* is monotonic, at slightly more cost — it is the right default
when correctness at endpoints matters (animation keyframes, where drifting
past `b` means a bone overshoots).

**The framerate-dependence trap** — this is the important part of the unit.
The exponential-smoothing idiom, seen in an enormous amount of game code:

```cpp
// WRONG: framerate-dependent
camera_pos = lerp(camera_pos, target_pos, 0.1f);
```

This moves 10% of the remaining distance *per frame*. At 30 FPS the camera
converges half as fast as at 60 FPS, and at 144 FPS it is snappy to the
point of being jittery. The game feels different on different hardware,
which is a feel bug that is very hard to attribute.

The correct form makes the rate a time constant:
```cpp
// frame-rate independent exponential smoothing
// `rate` = how much of the gap remains after 1 second
camera_pos = lerp(target_pos, camera_pos, std::exp(-rate * dt));
// or equivalently, with a half-life which is easier to reason about:
float t = 1.0f - std::exp2(-dt / half_life);
camera_pos = lerp(camera_pos, target_pos, t);
```
The half-life form is worth adopting: `half_life = 0.1` means "closes half
the remaining distance every 100 ms", which is a number a designer can
reason about and which is identical at every framerate.

**Where it bites.** Everywhere, silently. The symptom is "the game feels
different on my other machine", and it is usually blamed on input latency.
Grep your own code for `lerp(x, target, constant)` — if the third argument
is a bare constant rather than a function of `dt`, it is this bug.

**Read.**
- Freya Holmér, "Lerp smoothing is broken" — https://www.youtube.com/watch?v=LSNQuFEDOyQ — this exact problem, well explained, 10 minutes.
- Rory Driscoll, "Frame Rate Independent Damping Using Lerp" — https://www.rorydriscoll.com/2016/03/07/frame-rate-independent-damping-using-lerp/ — the short written version with the derivation.
- `[3DMP]` ch. 5.7 for the basics.

**Check yourself.**
1. Why is `lerp(pos, target, 0.1f)` per frame framerate-dependent? Give the convergence at 30 vs 144 FPS after 1 second.
2. Derive the `exp(-rate*dt)` form. What continuous process is it the exact solution to?
3. Why do the two lerp formulations differ in floating point, and when does it matter?

### U02 — Easing and shaping functions
**What it is.** Remapping `t` before feeding it to a lerp, so motion
accelerates and decelerates rather than moving linearly.

**Why it exists.** Nothing in the physical world starts and stops
instantly. Linear motion reads as mechanical; eased motion reads as
intentional. This is the cheapest possible improvement to game feel and
it is why `T07.S02` leans on it heavily.

**The mechanism.** The vocabulary:

```
  linear         ease-in         ease-out        ease-in-out
    /               _|              --              _--
   /               / |             /               /
  /              _/  |            /              _/
 /            __/    |          _/            __/
```
- **ease-in** (slow start): `t²`, `t³`. For things that accelerate — a
  falling object, a charging attack.
- **ease-out** (slow stop): `1-(1-t)²`. **The most useful one in UI and
  game feel** — things arriving should decelerate. A menu that eases out
  feels responsive because it starts fast.
- **ease-in-out**: `smoothstep(t) = t²(3-2t)`. The default for anything
  that both starts and stops.
- **back / overshoot**: goes past 1 and comes back. Reads as physical
  weight, used constantly in UI pops and impact effects.
- **elastic / bounce**: damped oscillation. Cartoonish; use sparingly.

`smoothstep` deserves special mention: `t*t*(3-2t)` is a cubic Hermite
with zero derivative at both ends, which is why it looks smooth. Its
higher-order cousin `smootherstep` (`t³(6t²-15t+10)`, Perlin's) has zero
*second* derivative too, and is what you want when the smoothstep's
acceleration discontinuity is visible.

**Springs as an alternative.** For anything that follows a moving target,
a damped spring is often better than an easing curve, because it handles
the target moving mid-animation gracefully — an easing curve has to be
restarted. The critically-damped spring (no overshoot, fastest
convergence) is the workhorse; `[GAMEFEEL]` and Ryan Juckett's writeup
cover it. This is genuinely the tool of choice for cameras.

**Where it bites.** Applying easing to a value that is *already* being
smoothed by physics or by an exponential lerp — the two compose into
something sluggish and hard to tune. Pick one mechanism per value.

**Read.**
- https://easings.net/ — the visual catalogue with formulas. Bookmark it.
- `[IQ]` "Useful little functions" — https://iquilezles.org/articles/functions/ — shaping functions with their derivatives and use cases, from someone who thinks about this professionally.
- Ryan Juckett, "Damped Springs" — https://www.ryanjuckett.com/damped-springs/ — the spring alternative, derived and made stable.

**Check yourself.**
1. Why does ease-out feel "responsive" while ease-in feels "sluggish" for UI?
2. What is `smoothstep`'s derivative at `t=0` and `t=1`, and why does that matter visually?
3. When is a damped spring better than an easing curve? Give a concrete case.

### U03 — Bezier and Hermite curves
**What it is.** Parametric curves defined by control points. Cubic is the
practically universal choice.

**Why it exists.** Camera paths, projectile arcs, road/track geometry,
animation curves in every DCC tool and every engine's curve editor, UI
motion paths.

**The mechanism.**

**Cubic Bezier** — four control points, the curve passes through `P0` and
`P3` and is pulled toward `P1` and `P2`:
```
   B(t) = (1-t)³P0 + 3(1-t)²t P1 + 3(1-t)t² P2 + t³ P3

        P1 •--------• P2
          /          \          the curve is inside the convex hull
   P0 •--'            '--• P3   of the control points, always
```
The De Casteljau construction — repeated lerping — is both the numerically
stable evaluation method and the clearest explanation of what the curve
*is*: lerp along each edge, then lerp between those results, then again.
It also gives you curve subdivision for free, which is how you flatten a
curve into line segments for rendering.

**Cubic Hermite** — same curve, different parameterization: two endpoints
and two **tangents**. This is what animation curve editors expose, because
tangents are what an animator wants to control (`T09.S02`).

Bezier and Hermite are the same cubic in different bases; converting is a
small matrix. Know that, so you're not surprised when a glTF animation
gives you tangents and your curve code wants control points.

**Catmull-Rom** — a Hermite spline whose tangents are derived
automatically from neighbouring points (`tangent_i = (P_{i+1} - P_{i-1})/2`).
The property that matters: **it passes through all its control points**,
unlike a Bezier's middle two. That makes it the default for "I have a
list of waypoints, give me a smooth path through them" — camera rails,
patrol routes, spline-based level geometry.

**Continuity, and why it's the thing that bites.** Joining curves:
- **C0** — endpoints meet. Visible corner.
- **C1** — tangents match in direction *and* magnitude. Smooth.
- **G1** — tangents match in direction only. Looks smooth; velocity jumps.
- **C2** — curvature matches. Needed when the curve drives something with
  momentum, or for anything a camera follows, where a C1-only join produces
  a visible jerk.

**Arc-length parameterization.** `t` is **not** distance along the curve.
Moving at constant `t` per second means moving at varying speed — fast
where control points are far apart, slow where they bunch. For a camera
dolly or a vehicle on a spline this is immediately visible and wrong. The
fix is to build an arc-length lookup table (sample the curve densely,
accumulate segment lengths, then binary-search to map distance → `t`).
This is a known-and-solved problem that everyone rediscovers painfully.

**Where it bites.** The arc-length issue, and C1-vs-C2 at joins. Both look
like "the camera stutters" and neither is a bug in the camera code.

**Read.**
- `[FREYA]` "The Beauty of Bézier Curves" — https://www.youtube.com/watch?v=aVwxzDHniEw — the best single explanation that exists. Watch before reading anything else.
- `[3DMP]` ch. 13 — https://gamemath.com/book/curves.html — curves in a game-math context, including arc-length.
- `[IQ]` — https://iquilezles.org/articles/bezierbbox/ and the related curve articles, for practical evaluation tricks.

**Check yourself.**
1. Why does a Bezier not pass through `P1` and `P2`, and why does Catmull-Rom pass through all its points?
2. Explain De Casteljau's construction, and what else it gives you besides evaluation.
3. Why is `t` not arc length? Describe the visual symptom for a camera on a spline.
4. What is the difference between G1 and C1 continuity, and when does it matter?

### U04 — Floating point, in the specific ways games meet it
**What it is.** The consequences of `float` being a finite approximation,
concentrated on the cases that produce visible artifacts.

**Why it exists.** `T02.S01.U04` introduced epsilons. This unit is the
rest of it, gathered in one place because it explains a whole class of
bugs that otherwise look unrelated.

**The mechanism.** `float` is 32 bits: 1 sign, 8 exponent, 23 mantissa.
The critical consequence is that **precision is relative, not absolute**:

| magnitude | spacing between representable floats |
|---:|---:|
| 1.0 | ~1.2e-7 |
| 100 | ~7.6e-6 |
| 10,000 | ~9.8e-4 (≈1 mm if units are metres) |
| 1,000,000 | ~0.0625 (6 cm) |
| 16,777,216 | 2.0 — integers stop being exact here |

This table explains the **large world problem**: at 10 km from the origin,
your positional precision is about a millimetre, which is fine. At 1000 km
it is 6 cm, and a character standing still visibly vibrates because its
position quantizes differently each frame. The industry answers are
**origin rebasing** (periodically shift the world so the player is near
the origin) and **doubles for world coordinates, floats for local** —
not "use doubles everywhere", which costs bandwidth and doesn't fix the
GPU side.

The other consequences worth holding:

- **Catastrophic cancellation.** Subtracting two nearly-equal large numbers
  destroys precision. `(a*a - b*b)` is much worse than `(a+b)*(a-b)` when
  `a ≈ b`. This is why the geometric ray-sphere form in `T02.S04.U03` is
  preferred over the quadratic form.
- **Addition is not associative.** `(a+b)+c != a+(b+c)`. This is why
  parallel reduction is nondeterministic (`T01.S02.U05`, `T13.S05`) and
  why summing a large array in order gives a different result than summing
  it in blocks.
- **NaN propagates and compares false to everything**, including itself.
  `x != x` is the NaN test. A single NaN entering a position will,
  within a few frames, have propagated through physics into the transform
  hierarchy and made an entire scene disappear. Add a debug-build
  `assert(is_finite(v))` at the points where values enter your simulation;
  it converts a baffling three-hour hunt into an immediate stack trace.
- **`-ffast-math` breaks NaN/inf handling and reassociates arithmetic.**
  It can genuinely help performance, and it makes your program
  non-IEEE-conformant. Know that you enabled it; don't enable it and then
  debug a determinism problem.
- **Denormals** (values very near zero) are slow on some hardware — an
  audio filter decaying toward silence can hit a 100x slowdown. FTZ/DAZ
  flags exist for this; `T11.S02` will meet it.

**Where it bites.** The large-world case, and NaN propagation. Both look
like something else entirely until you know the signature.

**Read.**
- `[GOLDBERG]` §1 and the "Relative Error and Ulps" section — https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
- `[RTCD]` ch. 11 — "Numerical Robustness". The games-specific version, and the most directly useful thing on this list. Read it now; reread at `T05`.
- Bruce Dawson, "Comparing Floating Point Numbers" — https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/ — the definitive treatment of epsilon comparison.

**Check yourself.**
1. What is the float spacing at 100,000 units, and what does that mean for a world in metres?
2. Why is `(a+b)*(a-b)` better than `a*a - b*b` when `a ≈ b`?
3. How do you test for NaN, and why doesn't `x == NAN` work?
4. Name two things `-ffast-math` changes that could break a game.

### U05 — Random numbers
**What it is.** Pseudorandom generation, and the ways games need it to
differ from `rand()`.

**Why it exists.** Procedural generation, loot, AI variation, particle
systems, and — critically — determinism requirements from `T01.S02.U05`.

**The mechanism.** Requirements that rule out the obvious choices:

- **`rand()` is unacceptable.** Implementation-defined, poor quality, often
  15-bit, global mutable state, not reproducible across platforms.
- **`std::mt19937` is reproducible but heavy** — 2.5 KB of state, which is
  a lot to have per-entity, and the *distributions* in `<random>` are
  **not** specified to produce identical output across implementations even
  from the same engine. If you need cross-platform determinism, write your
  own distribution mapping.
- **Use a small, fast, explicit-state PRNG**: PCG32 (~8 bytes state,
  excellent quality — https://www.pcg-random.org/) or xoshiro256++. Both
  are ~10 lines, both let you own the state.

**Own the state explicitly.** A global RNG is a determinism bug waiting to
happen: any code path that consumes a different number of random values
(a debug draw, a differently-ordered iteration) desynchronizes everything
downstream. Instead:
```cpp
struct Rng { uint64_t state; uint64_t inc; };   // PCG32
// Separate streams for separate concerns:
Rng sim_rng;         // simulation — must be deterministic and replayed
Rng cosmetic_rng;    // particles, idle animations — never affects simulation
```
The sim/cosmetic split is the important idea, and it is what makes `A021`
(replays) and `A047` (rollback) possible without freezing every visual
flourish.

**Games want "fair" random, not uniform random.** Uniform randomness
produces streaks, and players experience streaks as broken. Common
techniques:
- **Shuffle bags**: fill a bag with the desired distribution, shuffle,
  draw without replacement, refill when empty. Guarantees the distribution
  over short windows. Used for Tetris piece order.
- **Pity timers**: increase drop probability each failure, reset on
  success. Standard in loot systems.
- **Pseudo-random distribution (PRD)**: the Warcraft III / Dota 2 approach
  to critical hits — a linearly increasing probability tuned so the *mean*
  matches the advertised rate but variance is much lower.

**Noise is not random.** Perlin/simplex/value noise are *deterministic
functions of position* that look random and are spatially continuous.
That continuity is the point — terrain, clouds, and procedural textures
need `noise(x)` and `noise(x+ε)` to be similar. `[IQ]` is the reference.

**Where it bites.** Reading from a shared global RNG in cosmetic code and
then wondering why replays desync. The split above prevents it structurally.

**Read.**
- PCG's site — https://www.pcg-random.org/ — including the "Other RNGs" critique page, which is a good education in what these things are judged on.
- `[IQ]` on noise — https://iquilezles.org/articles/ — the gradient noise and fbm articles.
- `[GAMEAIPRO]` — chapters on randomness and perceived fairness in games.

**Check yourself.**
1. Why is `std::mt19937` reproducible but `std::uniform_int_distribution` not?
2. Explain why a global RNG breaks replay determinism, with a concrete scenario.
3. What is a shuffle bag and what problem does it solve that uniform random doesn't?
4. Why is Perlin noise not a random number generator?

---

## Implementation

| assignment | size | target rung | what it forces you to understand |
|---|:---:|:---:|---|
| [A005](../assignments/A005-orbit-and-aim.md) | S | R2 | Framerate-independent smoothing and easing, felt directly by dragging things around |
| [A004](../assignments/A004-math-geometry.md) | M | R3 | Splines, slerp, and the numerical guards, in the same testbed as the geometry |

**Order and overlap.** `A005` is where U01's framerate-independence lesson
becomes physical: its spec requires you to build the naive
`lerp(a, b, 0.1f)` version *and* the correct one, run them side by side,
and cap the framerate to 20 FPS to watch them diverge. That five-minute
experience is worth more than the derivation.

The curve code from `A004` is reused in `A031` (animation curves are
Hermite), `A034` (path smoothing), and `A040` (the editor's curve widget).
The RNG from U05 is reused in `A020` (data-driven enemy variation),
`A021` (replays — where the sim/cosmetic split gets tested), and `A047`
(rollback — where it gets tested *hard*).

**If you are short on time.** U01 is non-negotiable — the framerate trap
is a bug you will otherwise ship. U02 and U05 are each 20 minutes. U03
(curves) can be deferred until `A034`, and U04 can be skimmed now and read
properly when `T05` starts producing jitter, which it will.

---

## Discussion Log
<!-- Appended per study session. See ../CONVENTIONS.md §7. -->

## Open Questions

## Connections
- `T02.S01.U04` — the epsilon discussion this subtopic completes.
- `T02.S03.U04` — slerp, which is this subtopic applied to rotations.
- `T03.S01` — the fixed timestep, which is the structural answer to U01's problem.
- `T07.S02` — easing and springs as the technical content of "juice".
- `T09.S02` — animation curves, which are U03's Hermite splines.
- `T13.S05`, `T14.S05` — where U04's non-associativity and U05's determinism matter most.
