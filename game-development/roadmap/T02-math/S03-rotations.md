# T02.S03 — Rotation Representations
**Topic:** [T02 — Math for Games](README.md) | **Depends on:** T02.S02
**Status:** expanded | **Assignments:** A004 | **Est. theory time:** 2 sessions

> The question this subtopic answers: *there are four ways to represent a
> rotation and they are all standard — which one do I store, which do I
> interpolate, which do I show a designer, and how do I convert without
> introducing a bug?*
>
> There is no single best representation. The skill is knowing the
> trade-offs well enough to convert deliberately.

---

## Theory

### U01 — Euler angles: what they buy and what they cost
**What it is.** Three angles applied as three successive rotations about
three axes. "Pitch 30, yaw 45, roll 0."

**Why it exists.** Humans can read them. A designer can type "yaw 90" and
know what will happen; nobody can type a quaternion. That is the entire
argument for Euler angles, and it is a good one — it is why every engine's
inspector shows them.

**The mechanism.** An Euler triple is meaningless without stating the
**order** and whether rotations are about fixed (extrinsic) or moving
(intrinsic) axes. There are 12 valid orderings and both variants, so 24
conventions, all called "Euler angles".

Common in games: **YXZ intrinsic** (yaw about Y, then pitch about the new
X, then roll about the new Z), because it matches how a camera or
character is naturally described. Unity uses ZXY; Unreal uses a
pitch/yaw/roll triple with its own axis conventions. **Write your
convention down in your header.**

The three problems, in increasing order of severity:

1. **Order dependence.** `yaw 90 then pitch 90` ≠ `pitch 90 then yaw 90`.
   Not a bug — rotations genuinely don't commute — but it means the triple
   is only interpretable with its convention attached.

2. **Gimbal lock.** When the middle rotation reaches ±90°, the first and
   third axes align and you lose a degree of freedom. In YXZ, pitching to
   exactly straight up makes yaw and roll do the same thing. This is not a
   numerical problem; it is a topological one — no 3-parameter
   representation of rotation can avoid it.

   ```
     pitch = 0:    yaw spins you around the world Y  (horizon spins)
     pitch = 90:   yaw and roll are now the same axis
                   -> one dof gone, and interpolating through here
                      makes the object swing wildly
   ```

3. **Interpolation is wrong.** Lerping two Euler triples does not produce
   the rotation "halfway between". It produces a path that wobbles, and
   near gimbal lock it flails. This is the reason animation systems do not
   store Euler.

**Where it bites.** A first-person camera clamped to ±89° pitch works fine
and hides the whole problem — which is why many people never meet gimbal
lock and then are baffled by it in `A031`. Free-flying cameras, aircraft,
and any bone in an animated skeleton meet it immediately.

**Read.**
- `[3DMP]` ch. 8.3 — https://gamemath.com/book/orient.html — Euler angles, their aliases, and gimbal lock.
- `[FGED1]` §2.6 — the matrix forms per ordering.

**Check yourself.**
1. Why is "pitch 30, yaw 45" ambiguous without more information? Name the two things you must state.
2. Explain gimbal lock in terms of degrees of freedom, not in terms of gimbals.
3. Why does clamping camera pitch to ±89° make the problem disappear for a FPS camera?

### U02 — Axis-angle and rotation matrices
**What it is.** Two more representations. Axis-angle: a unit axis `n` and
an angle `theta` (4 numbers, or 3 as a scaled "rotation vector"). Matrix:
the 3×3 from `T02.S02`.

**Why they exist.** Axis-angle is how rotations are physically specified
("spin about this axis by this much") and is the natural output of many
derivations. Matrices are what the GPU consumes.

**The mechanism.** Rodrigues' rotation formula rotates `v` about unit axis
`n` by `theta`:

```
   v' = v cos(theta) + (n × v) sin(theta) + n (n · v)(1 - cos(theta))
        \___________/   \_______________/   \_________________________/
         the component    the perpendicular   the component along n,
         perpendicular    swing               which is unchanged
         to n, rotated
```

The decomposition is the insight: split `v` into a part along `n` (fixed)
and a part perpendicular (rotated in the plane). Both other terms follow
from `T02.S01`'s dot and cross.

The matrix form (the "rotation matrix from axis-angle") drops out of
Rodrigues by collecting terms, and its trace is `1 + 2cos(theta)` — a
convenient way to extract the angle back out.

**Comparing all four representations:**

| | numbers | interpolates? | composes? | free of gimbal lock? | human-readable? |
|---|---:|---|---|---|---|
| Euler | 3 | badly | via conversion | no | **yes** |
| Axis-angle | 4 | badly (about different axes) | via conversion | yes | somewhat |
| Matrix 3×3 | 9 | no (must re-orthonormalize) | **yes, cheap** | yes | no |
| Quaternion | 4 | **yes (slerp)** | **yes, cheapest** | yes | no |

That table is the whole subtopic. Matrices for the GPU, quaternions for
storage and interpolation, Euler for the UI, axis-angle where the physics
hands it to you.

**Where it bites.** Matrix drift. Repeatedly multiplying rotation matrices
accumulates floating-point error until the columns are no longer
orthonormal — the object starts to shear and scale imperceptibly, then
perceptibly. Quaternions drift too, but renormalizing a quaternion is one
divide, whereas re-orthonormalizing a matrix is Gram-Schmidt.

**Read.**
- `[3DMP]` ch. 8.2 and 8.4 — matrix and axis-angle forms.
- `[FGED1]` §2.6.2 — Rodrigues' formula derived.

**Check yourself.**
1. Derive Rodrigues' formula by decomposing `v` into components parallel and perpendicular to `n`.
2. Why is composing two rotations cheaper with quaternions than with matrices? Count the operations.
3. What does matrix drift look like on screen, and how would you detect it programmatically?

### U03 — Quaternions: the operational understanding
**What it is.** `q = (w, x, y, z)` = `w + xi + yj + zk`, a unit-length
4-vector encoding a 3D rotation. Four numbers, no gimbal lock, cheap
composition, correct interpolation.

**Why it exists.** Because the set of 3D rotations is topologically a
3-sphere with antipodal points identified, and *no* 3-parameter chart
covers it without singularities. Four parameters with one constraint does.
That is the real answer to "why quaternions"; the practical answer is the
table in U02.

**The mechanism.** You do not need the algebra of `i, j, k` to use them
correctly. You need this:

**Construction from axis-angle** — this is the definition to memorise,
because everything else follows from it:
```
   q = ( cos(theta/2),  n * sin(theta/2) )
         ^ w component   ^ xyz, the "vector part"
```
The **half-angle** is the one thing that surprises everyone. It falls out
of the sandwich product below: a quaternion rotation applies `q` twice
(once as `q`, once as `q*`), so each contributes half the angle.

**Rotating a vector** — the sandwich product:
```
   v' = q * (0, v) * q⁻¹        (q⁻¹ = conjugate, for unit q)
```
In practice you use the expanded form, which avoids constructing the
intermediate quaternions:
```cpp
vec3 rotate(quat q, vec3 v) {
    vec3 u{q.x, q.y, q.z};
    vec3 t = 2.0f * cross(u, v);
    return v + q.w * t + cross(u, t);
}
```

**Composition**: `q_total = q_second * q_first` — same right-to-left order
as matrices, and cheaper: 16 mul + 12 add, vs 27 mul + 18 add for 3×3.

**Inverse**: for a unit quaternion, the conjugate `(w, -x, -y, -z)`. Free.

**Identity**: `(1, 0, 0, 0)`. Note: `w=1`, not `w=0` — a zero quaternion is
not a rotation and normalizing it is the same NaN trap as `T02.S01.U04`.

**The double cover — the single most important practical fact.**
`q` and `-q` represent the **same rotation**. Every rotation has exactly
two quaternion representations. Consequences:

- You cannot compare quaternions for equality without checking both signs.
- **Interpolating between `q` and a `-q'` takes the long way around** —
  a 359° rotation instead of a 1° one. This is the single most common
  quaternion bug in animation, and it looks like a rigging error.
- The fix, everywhere you blend: if `dot(q0, q1) < 0`, negate `q1` first.
  This is one line and it belongs inside your `slerp`/`nlerp`, not at the
  call sites.

**Where it bites.** Component order. Some libraries store `(w,x,y,z)`,
others `(x,y,z,w)`. GLM uses `(w,x,y,z)` in its constructor but
`(x,y,z,w)` in memory. Mixing them silently swaps the real part with a
vector component and produces rotations that are wrong in a way that looks
like a coordinate-system problem. Pick one, write it in the header, and
assert on `length ≈ 1` in debug builds.

**Read.**
- `[3DMP]` ch. 8.5 — https://gamemath.com/book/orient.html#quaternions — the operational treatment; this is the one to actually study.
- `[FREYA]` "Quaternions explained" and 3Blue1Brown/Ben Eater's interactive quaternion explainer — https://eater.net/quaternions — watch/play with one of these before reading, so the half-angle stops being arbitrary.
- `[FGED1]` ch. 2.7 — if you want the derivation properly.

**Check yourself.**
1. Why does the axis-angle construction use `theta/2`?
2. What are `q` and `-q`, and give the concrete visual symptom of not handling it in a blend.
3. Write the identity quaternion. Why is `(0,0,0,0)` not a valid rotation?
4. Compare op counts: composing 100 rotations as quaternions vs as 3×3 matrices.

### U04 — Interpolating rotations: slerp, nlerp, and drift
**What it is.** Producing intermediate rotations between two orientations.
The whole reason animation stores quaternions.

**Why it exists.** Blending two animation clips, smoothing a camera toward
a target, and easing a turret onto a target are all "give me 30% of the
way from A to B, as a rotation".

**The mechanism.**

**`nlerp`** — linearly interpolate the four components, then normalize:
```cpp
quat nlerp(quat a, quat b, float t) {
    if (dot(a, b) < 0) b = -b;          // shortest path — never omit this
    return normalize(a * (1-t) + b * t);
}
```
- Cheap, commutative, associative (so blending 3+ clips works predictably).
- **Not constant angular velocity** — it moves faster in the middle of the
  arc than at the ends. The chord is being traversed linearly, then
  projected onto the sphere.

**`slerp`** — interpolate along the great-circle arc:
```
   slerp(a, b, t) = ( sin((1-t)*omega) * a  +  sin(t*omega) * b ) / sin(omega)
   where cos(omega) = dot(a, b)
```
- Constant angular velocity. Correct.
- Costs an `acos`, two `sin`s, a divide.
- **Numerically unstable when `a` and `b` are nearly equal** (`sin(omega)`
  → 0). Every real implementation falls back to `nlerp` below a threshold
  (`dot > 0.9995` is the usual constant), which is fine because at that
  angle the two are visually identical anyway.

**Which to use.** The honest answer: for the small angular deltas typical
of animation blending at 30–60 Hz, **the difference is invisible** and
`nlerp` is faster and blends associatively — which is why many animation
systems use `nlerp` internally for pose blending. Use `slerp` when the
arc is large and the velocity is visible: a camera swinging 120° over a
second, a slow deliberate turret rotation.

The error is bounded: nlerp's maximum angular deviation from slerp is
under 3° for a 90° arc, and it goes to zero at both endpoints and at
`t=0.5`. `[3DMP]` has the plot.

**Drift and renormalization.** Repeated quaternion multiplication
accumulates error and `|q|` wanders from 1. Renormalize:
- after every N compositions in a chain, or
- once per frame per animated bone, or
- whenever `|dot(q,q) - 1| > 1e-4`.

One `rsqrt` and four multiplies. Cheap insurance; do it rather than
reasoning about whether you need to.

**Where it bites.** Skipping the `dot < 0` sign flip. It works for most
pairs and then a character's arm swings the wrong way around for one
transition. Because it is data-dependent, it survives testing.

**Read.**
- `[3DMP]` ch. 8.5.6–8.5.9 — slerp derived, with the nlerp comparison plot.
- Jonathan Blow, "Understanding Slerp, Then Not Using It" — https://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/ — the argument for nlerp, from someone who did the analysis.
- `[GEA3]` §12.4 — how a real animation system blends, and what it stores.

**Check yourself.**
1. Why does nlerp not have constant angular velocity? Draw the chord vs the arc.
2. Where is nlerp's error maximal, and where is it exactly zero?
3. Why does slerp need a fallback near `dot ≈ 1`, and what is the fallback?
4. Give a case where the nlerp/slerp difference would actually be visible in a game.

### U05 — Conversions, and choosing what to store
**What it is.** The practical policy: which representation lives where in
your engine, and the conversions between them.

**Why it exists.** Because you will need all four, and the conversions are
where sign errors hide.

**The mechanism.** The policy this roadmap uses:

```
   Designer / inspector  ---> Euler (with convention documented)
                                |  convert on edit
                                v
   Storage & simulation  ---> Quaternion   <--- animation clips store these
                                |               (T09.S02)
                                |  convert once per object per frame
                                v
   GPU upload            ---> 3x3 / 4x4 matrix
```

Rules that follow:
- **Never store Euler.** Convert on the way in from the UI, discard.
  (Caveat: engines that *do* keep Euler in the inspector — Unity, Unreal —
  keep a shadow copy precisely so that typing `90` and reading it back
  gives `90` rather than a re-derived alias like `-270`. If you build an
  editor in `A040`, you will meet this.)
- **Never interpolate matrices.** Convert to quaternion, slerp, convert
  back.
- **Convert quaternion → matrix once per object per frame**, not per
  vertex, not per call site.

**Quaternion → matrix** (unit `q`, column-major, our convention):
```
   | 1-2(y²+z²)   2(xy-wz)    2(xz+wy)  |
   |  2(xy+wz)   1-2(x²+z²)   2(yz-wx)  |
   |  2(xz-wy)    2(yz+wx)   1-2(x²+y²) |
```

**Matrix → quaternion** requires Shepperd's method: compute the trace, and
branch on which of `w, x, y, z` is largest to avoid dividing by something
near zero. Naive extraction (always solving for `w` from the trace) fails
for rotations near 180°, where `w ≈ 0`. This is a known trap — take the
branching version from `[3DMP]` §8.7.5 rather than deriving it, and *test
it at 180°*.

**Euler → quaternion** is three axis-angle quaternions multiplied in your
chosen order. **Quaternion → Euler** needs a `clamp` before `asin` (the
same `acos` trap from `T02.S01.U02`) and has to special-case gimbal lock,
where it must pick one of infinitely many valid answers.

**Where it bites.** Matrix→quaternion at 180°, and Euler round-tripping.
`euler_from_quat(quat_from_euler(e))` will not give you `e` back in
general — it gives an equivalent orientation with possibly different
numbers. That is correct behaviour, and it confuses everyone the first
time. It is also why editors keep the shadow copy.

**Read.**
- `[3DMP]` ch. 8.7 — all the conversions, with the numerically careful versions.
- Mike Day (Insomniac), "Converting a Rotation Matrix to a Quaternion" — https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf — a short, robust branchless variant.

**Check yourself.**
1. Why does naive matrix→quaternion fail near 180°?
2. Why doesn't `euler_from_quat(quat_from_euler(e)) == e` in general? Is that a bug?
3. Where in a frame should quaternion→matrix conversion happen, and how many times?

---

## Implementation

| assignment | size | target rung | what it forces you to understand |
|---|:---:|:---:|---|
| [A004](../assignments/A004-math-geometry.md) | M | R3 | Quaternions with the double-cover handled, slerp with its fallback, all four conversions, and a visual testbed where gimbal lock is *demonstrated*, not described |

**Order and overlap.** `A004` covers `S03` and `S04` together. The
requirement that matters most here is the **visual testbed**: three
side-by-side objects rotating via Euler, matrix, and quaternion
interpolation, driven by the same input. Gimbal lock and the long-way-round
bug are things you have to *see* once; after that you recognise them
instantly in `A031` when a character's shoulder does something impossible.

The quaternion code from `A004` is used again in: `A005` (2D is a
degenerate case, and worth noticing), `A010`/`A011` (object orientation),
`A017` (angular impulse), `A030`/`A031` (this is where quaternions earn
their keep — every bone, every frame), `A032` (IK), and `A047`
(determinism requires your quaternion ops be reproducible).

**If you are short on time.** Quaternion construct/compose/rotate/inverse/
slerp plus quat↔matrix is the minimum. Euler conversions can wait until
`A040`'s editor needs them — but then do them properly, including the
gimbal-lock case.

---

## Discussion Log
<!-- Appended per study session. See ../CONVENTIONS.md §7. -->

## Open Questions

## Connections
- `T02.S01.U04` — the normalization and NaN traps, which apply identically to quaternions.
- `T02.S02.U05` — why `Rᵀ = R⁻¹`, the matrix analogue of the quaternion conjugate.
- `T02.S05` — interpolation in general; slerp is one instance of it.
- `T09.S01`, `T09.S03` — where this subtopic is cashed in, heavily.
- `T05.S04` — angular velocity and torque, which are axis-angle by nature.
