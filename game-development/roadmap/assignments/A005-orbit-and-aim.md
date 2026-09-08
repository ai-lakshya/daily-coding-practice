# A005 — "Orbit & Aim" Visual Math Toy
**Topic:** T02.S01, T02.S05 | **Size:** S (2–4h) | **Target rung:** R2
**Status:** todo
**Code:** `game-development/code/A005-orbit-and-aim/`
**Reuses:** A002 (skeleton), A003 (math) | **Reused by:** A006 (its smoothing), A024 (its easing), A033 (steering is this, generalized)

## Goal
Convert the vector math from `A003` from something you can compute into
something you can *predict*. And meet the framerate-dependence trap
(`T02.S05.U01`) in a form where you can feel it, not just derive it.

This is deliberately the smallest and most fun assignment in Phase A. Do
it in one sitting.

## Concepts exercised
| unit | first met in | why it recurs here |
|---|---|---|
| T02.S01.U02 | A003 | Dot product, watched live as you drag a vector |
| T02.S01.U03 | A003 | The 2D cross product's sign as a left/right test |
| T02.S05.U01 | here | **The framerate-dependent lerp trap, demonstrated** |
| T02.S05.U02 | here | Easing curves, felt rather than plotted |
| T02.S05.U05 | here | A seeded PRNG for the wander behaviour |

## Prerequisites
`A002`'s skeleton and `A003`'s math library.

## Specification

A 2D scene. A **turret** at the centre, a **target** you drag with the
mouse, and a few obstacles.

1. **Live vector readout.** Draw and label, updating every frame:
   - the vector from turret to target,
   - `dot(turret_forward, normalize(to_target))`, with the value printed,
   - `cross2(turret_forward, to_target)` with its **sign** shown as a
     left/right indicator,
   - the angle between, in degrees,
   - the projection of `to_target` onto `turret_forward`, drawn as an
     actual line segment.
2. **A view cone.** A 60° cone drawn from the turret. The target changes
   colour when inside it. Implemented as the `dot > cos(30°)` comparison
   from `T02.S01.U02` — **not** with `acos`.
3. **Aiming, four ways**, switchable at runtime (cvar or number keys), all
   turning the turret toward the target:
   - **(a) Instant** — snap. The baseline.
   - **(b) Naive lerp** — `angle = lerp(angle, target_angle, 0.1f)` every
     frame. The trap.
   - **(c) Frame-rate independent** — `lerp(angle, target, 1 - exp2(-dt/half_life))`.
   - **(d) Constant angular speed** — turn at N degrees/second, clamped.
   Each must take the **shortest angular path** (the 2D analogue of
   `T02.S03.U04`'s sign flip — wrapping through ±180 is the same bug).
4. **The framerate demonstration.** A key that caps the framerate to 20
   FPS (busy-wait or sleep to pad the frame). With (b) selected, the
   turret visibly turns more slowly. With (c), it does not. **This is the
   deliverable that matters most in this assignment.**
5. **Reflection.** A projectile fired from the turret that bounces off the
   obstacles' edges using `reflect(v, n)` from `A003`.
6. **An easing comparison.** Five markers moving from left to right over 2
   seconds using linear, ease-in, ease-out, `smoothstep`, and a back/
   overshoot curve — running simultaneously so the difference is visible.
7. **Wander.** One obstacle that moves with a seeded random walk, using
   your own PRNG (`T02.S05.U05`), not `rand()`.

### Constraints
- Everything drawn with `A004`'s `DebugDraw` if you have it, or SDL's 2D
  renderer. No new rendering work.
- All vector math from `A003`. No new math functions except 2D-specific
  helpers.
- No `acos` in the view-cone test.

### Explicitly out of scope
A game. Collision response beyond a reflection. A fixed timestep (that's
`A006` — this assignment deliberately uses a variable one so that (b) can
misbehave).

## Acceptance criteria
- [ ] Dragging the target updates all five readouts live and they agree with what you'd predict by looking at the geometry.
- [ ] The cross-product sign correctly indicates left/right as you sweep the target around the turret, including behind it.
- [ ] Toggling the 20 FPS cap makes (b) visibly slower and leaves (c) unchanged. **Write down the observed convergence difference in `NOTES.md`.**
- [ ] All four aiming modes take the short way around when the target crosses behind the turret.
- [ ] The five easing markers reach the end simultaneously but travel differently.
- [ ] The wander is reproducible: same seed, same path.

## Deliverables
- `code/A005-orbit-and-aim/`
- `NOTES.md` — the 20 FPS observation with numbers, and one sentence on which aiming mode you'd use for a real turret and why

## Design notes / hints
- The short-angular-path problem: `target - current` can be ±350° when
  you meant ∓10°. Wrap the difference into `[-180, 180]` before using it.
  This is exactly the quaternion double-cover problem in 1D, and noticing
  that connection is worth more than the code.
- For the framerate cap, busy-wait rather than `sleep` — sleep granularity
  will make the demo noisy.
- Mode (d) (constant angular speed) is what most real games use for
  turrets, because it is predictable for the player. Modes (b)/(c) feel
  "smooth" and make the turn time depend on the angle, which is harder to
  read. Worth noticing.

## Cut list
1. The projectile and reflection.
2. The wander obstacle.
3. Aiming modes (a) and (d) — keep (b) and (c), which are the point.
4. **Never cut: the 20 FPS demonstration.**

## Rubric

| rung | passes when |
|---|---|
| R0 | Builds clean, runs, acceptance criteria met |
| R1 | The angle wrapping is correct in all quadrants, including exactly ±180°. No NaN when the target is exactly on the turret (zero-length `to_target`). |
| R2 | The smoothing code is written once as a reusable helper (`damp(current, target, half_life, dt)`), not inlined four times. The easing functions are a small reusable set. Both get copied into `A006` and `A024` without edits. |

### R4 extension
*(optional; this targets R2)* Replace mode (c) with a **critically damped
spring** (`T02.S05.U02`) and compare: move the target rapidly back and
forth and observe how the spring handles a moving target versus the
exponential damp. Write down which you'd use for a camera and why.

## Common pitfalls
- Using `acos` for the view cone and then wondering about the NaN when
  `dot` returns `1.0000001`.
- Forgetting the angle wrap, so the turret spins the long way around
  every time the target crosses behind it.
- Making the frame cap with `SDL_Delay(50)` and getting jittery, unclear
  results.
- Writing the smoothing four times inline, then having to fix the same
  bug four times.

## Review Log
<!-- One dated block per rung. See ../CONVENTIONS.md §4. -->

## Recall schedule
| due | rung | done | result |
|---|---|---|---|
