# T02.S01 — Vectors & Spaces
**Topic:** [T02 — Math for Games](README.md) | **Depends on:** —
**Status:** expanded | **Assignments:** A003, A005 | **Est. theory time:** 2 sessions

> The question this subtopic answers: *what do the vector operations
> actually mean geometrically, so that I reach for the right one without
> deriving it?* Almost every game-math bug is a right operation in the
> wrong space or a wrong operation with a plausible-looking result.

**Convention used throughout this roadmap** (write it in your math
header): **right-handed, Y-up, column vectors, column-major storage,
`M * v`, angles in radians, `-Z` forward.** Every one of those is a
choice; the only wrong move is having two files that disagree.

---

## Theory

### U01 — Points, vectors, and why the distinction matters
**What it is.** A vector is a displacement (direction + magnitude). A
point is a location. They are stored identically and behave differently.

**Why it exists.** Because the operations that are legal differ:

```
  point  - point  = vector      (displacement between two locations)
  point  + vector = point       (move a location)
  vector + vector = vector      (compose displacements)
  point  + point  = nonsense    (what is "Paris plus London"?)
```

And under an affine transform they transform differently: a point gets
translated, a vector does not. This is exactly what the homogeneous `w`
component encodes (`T02.S02.U04`): `w=1` for points, `w=0` for vectors.

**The mechanism.** Most game math libraries — including the one you will
write — use a single `vec3` type for both, and rely on the programmer to
know which is which. `[FGED1]` argues for distinct types; most shipped
engines don't. Either is fine; **knowing which one a variable holds is
not optional**, and naming it (`world_pos` vs `to_target`) is how you
keep it straight.

The bug this prevents: transforming a direction vector with a matrix that
has translation in it. Your "forward" vector gets displaced by the
object's position, giving a direction that changes as the object moves.
Classic, and it looks *almost* right.

**Where it bites.** Normals specifically. A normal is neither a point nor
an ordinary vector — it's a covector, and it transforms by the inverse
transpose (`T02.S02.U06`). Getting this wrong under non-uniform scale is
the standard first lighting bug.

**Read.**
- `[3DMP]` ch. 2.1–2.4 — https://gamemath.com/book/vectors.html — points vs vectors, done geometrically.
- `[FGED1]` ch. 1.1–1.3 — the more formal treatment, including why the distinction is structural.

**Check yourself.**
1. Give a concrete gameplay bug caused by treating a direction as a point.
2. What is `w` for a point, for a direction, and what does the transform do differently to each?

### U02 — The dot product: the projection operator
**What it is.** `a · b = ax*bx + ay*by + az*bz = |a||b|cos(theta)`.
A scalar. Geometrically: **how much of `a` points along `b`**.

**Why it exists.** It is the answer to "how aligned are these?", "how far
along this axis is that?", and "which side of this plane am I on?" — three
of the most common questions in a game.

**The mechanism.** Read the sign first; the magnitude second.

```
  a·b > 0     same general direction    (angle < 90 deg)
  a·b = 0     perpendicular
  a·b < 0     opposing                  (angle > 90 deg)
```

With `b` normalized, `a·b` is the **signed length of a's projection onto
b** — this is the single most useful reading and the one to internalise:

```
        a
       /|
      / |            a·b_hat  = this length along b
     /  |                       (negative if pointing the other way)
    +---+------> b_hat
    |<->|
     a·b_hat
```

The workhorse uses, all of which are the same idea:

| task | expression |
|---|---|
| Angle between | `acos(dot(normalize(a), normalize(b)))` |
| Is target in front of me? | `dot(forward, target - pos) > 0` |
| Within a 60° view cone? | `dot(forward, normalize(to_target)) > cos(radians(30))` |
| Signed distance to a plane | `dot(n, p) - d` (with `n` unit) |
| Project `a` onto `b` | `b * (dot(a,b) / dot(b,b))` |
| Reflect `v` about unit normal `n` | `v - 2*dot(v,n)*n` |
| Squared length | `dot(v, v)` |
| Lambert diffuse term | `max(0, dot(n, l))` — `T08.S02` |

**Where it bites.**
- **Forgetting to normalize** before `acos`. `dot` of unnormalized vectors
  scales with both magnitudes and `acos` of anything outside `[-1,1]`
  returns NaN — which then propagates silently through your whole frame.
  Always `clamp(d, -1, 1)` before `acos`; floating point *will* give you
  `1.0000001`.
- **Comparing `acos` results when you could compare `cos` values.** The
  view-cone row above avoids a transcendental per check by comparing
  against `cos(half_angle)` directly. Do this; it is free and it is also
  more numerically stable.
- **Using `length()` where `dot(v,v)` suffices.** Comparing distances?
  Compare squared distances and skip the `sqrt`. This idiom is everywhere
  in `T05` and `T10`.

**Read.**
- `[3DMP]` ch. 2.11 — https://gamemath.com/book/vectors.html#dot_product
- `[FREYA]` "The Beauty of Bézier Curves" is the famous one, but her vector
  math explainers are the right first exposure if the geometry is not
  immediate for you yet.

**Check yourself.**
1. Derive the reflection formula `v - 2(v·n)n` from the projection reading of the dot product. Draw it.
2. Why is `dot(fwd, dir) > cos(theta)` preferable to `acos(dot(fwd,dir)) < theta`? Give two reasons.
3. `dot(a,b)` is 0. Name three geometrically different situations that produce this.

### U03 — The cross product: area, and the perpendicular
**What it is.** `a × b` — a **vector** perpendicular to both `a` and `b`,
with magnitude `|a||b|sin(theta)` = the area of the parallelogram they span.
Only defined in 3D (2D has a scalar analogue; see below).

**Why it exists.** It answers "give me a perpendicular", "which way does
this triangle face", and "is this turn left or right".

**The mechanism.** Direction by the right-hand rule (in our right-handed
convention): point fingers along `a`, curl toward `b`, thumb is `a × b`.

```
  a × b = ( ay*bz - az*by,
            az*bx - ax*bz,
            ax*by - ay*bx )
```

Properties that matter more than the formula:
- **Anticommutative**: `a × b = -(b × a)`. Order is the difference between
  a surface facing you and facing away. This is where most winding-order
  bugs live.
- **Zero when parallel** (including antiparallel). So `cross` is a
  parallelism test, and normalizing the result of a near-parallel cross
  is a division by nearly zero — a real numerical hazard when building
  a basis from two nearly-aligned vectors (`U05`).
- `|a × b|` is twice the area of triangle `(0, a, b)`.

The workhorse uses:

| task | expression |
|---|---|
| Triangle normal | `normalize(cross(b-a, c-a))` — sign depends on winding |
| Build a perpendicular basis | `U05` |
| Torque / angular momentum | `cross(r, F)` — `T05.S04` |
| Barycentric / point-in-triangle | signs of three cross products — `T04.S01` |
| Triangle area | `0.5 * length(cross(b-a, c-a))` |

**The 2D "cross product".** In 2D, `cross(a,b) = ax*by - ay*bx` is a
scalar — the z-component of the 3D cross of the embedded vectors. It is
the signed area of the parallelogram, and its sign answers **"is `b` to
the left or right of `a`?"** That question is the backbone of 2D
collision (`T05.S02`), convex hulls, and steering (`T10.S01`). It is worth
having as `cross2(a,b)` in your library, and worth being comfortable that
it returns a scalar.

**Where it bites.** Winding order. A triangle's normal flips with vertex
order, and half the "my model is invisible from this side" bugs are a
backface-culling interaction with winding you didn't intend. Pick
counter-clockwise-is-front (the OpenGL default) and be consistent from
`A009` onward.

**Read.**
- `[3DMP]` ch. 2.12 — https://gamemath.com/book/vectors.html#cross_product
- `[RTCD]` §3.1.7 — the "triple products" section; useful for the geometric readings you'll want in `T05`.

**Check yourself.**
1. Without computing: which way does `X × Y` point in a right-handed, Y-up system? What about `Y × X`?
2. Why is the 2D cross a scalar, and what question does its sign answer?
3. You build a normal from `cross(b-a, c-a)` and it points into the surface. What are the two possible causes?

### U04 — Length, normalization, and the numerical traps
**What it is.** `|v| = sqrt(dot(v,v))`; `normalize(v) = v / |v|`.

**Why it exists.** Unit vectors are the currency of direction. Half the
formulas in graphics assume unit inputs and misbehave silently otherwise.

**The mechanism / where it bites.** Three traps, all of which you will hit:

1. **Normalizing a zero (or near-zero) vector.** Division by ~0 gives inf
   or NaN, which then contaminates everything downstream — and NaN
   compares false to everything including itself, so guards like
   `if (x > 0)` silently take the wrong branch. Your `normalize` must
   decide a policy and stick to it:
   ```cpp
   // policy: return zero and let the caller notice, with an assert in dev builds
   vec3 normalize(vec3 v) {
       float len2 = dot(v, v);
       ASSERT(len2 > 1e-12f);            // catch it where it happens
       if (len2 < 1e-12f) return {0,0,0};
       return v * (1.0f / std::sqrt(len2));
   }
   ```
   The alternative policy (`normalize_or(v, fallback)`) is often better at
   call sites that legitimately can get a zero vector — e.g. "direction to
   target" when you are standing on the target.

2. **Drift.** Repeatedly transforming a unit vector accumulates error and
   it stops being unit. Renormalize periodically at the point where it
   matters, not every operation (the `sqrt` isn't free). The same problem
   for quaternions is worse and is covered in `T02.S03.U04`.

3. **Comparing lengths.** `length(a) < length(b)` costs two `sqrt`s and
   answers the same question as `dot(a,a) < dot(b,b)`. Provide
   `length_squared()` in your library and *use* it — this is not
   micro-optimization, it is the idiomatic form, and in `T05.S03` it is in
   the innermost loop.

**Epsilons.** `if (a == b)` on floats is a bug. But a fixed absolute
epsilon (`fabs(a-b) < 1e-6`) is also wrong for large values, because float
spacing scales with magnitude. The practical rule for games:

- Use an **absolute** epsilon when comparing against zero (you know the
  scale: it's zero).
- Use a **relative** epsilon (`fabs(a-b) <= eps * max(fabs(a),fabs(b))`)
  when comparing two arbitrary values.
- Keep your world units sane — metres, with the playable space within
  ~10^4 of origin — so that absolute epsilons remain valid. This is why
  large open worlds need origin rebasing: at 10^6 metres, `float` spacing
  is ~0.06 m and physics visibly jitters.

**Read.**
- `[3DMP]` ch. 2.5–2.8.
- `[GOLDBERG]` §1–2 — https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html — read §1 and the "Relative Error and Ulps" part. Skim the rest.
- `[RTCD]` ch. 11 — "Numerical Robustness". This chapter is why your collision code won't jitter; read it now and again at `T05`.

**Check yourself.**
1. Why does a NaN in one component of a position propagate to the whole scene within a few frames?
2. Why does a fixed epsilon of `1e-6` fail for values around `1e6`? What is the float spacing there?
3. When is `length()` genuinely required rather than `length_squared()`?

### U05 — Coordinate spaces and building a basis
**What it is.** A space is an origin plus three basis vectors. "Position"
is meaningless without saying *in what space*.

**Why it exists.** A game holds the same point in half a dozen spaces at
once, and confusing them is the most common source of transform bugs —
more common than getting any individual formula wrong.

**The mechanism.** The spaces you will meet, in the order data flows:

```
  object/model space -> world space -> view/camera space
       -> clip space -> NDC -> screen space
```

plus tangent space (`T08.S03`), bone/joint space (`T09.S01`), and each
parent's space in a transform hierarchy.

**Naming convention that eliminates a whole bug class.** Encode the space
in the identifier:

```cpp
vec3 pos_world;                 // not "pos"
vec3 dir_local;
mat4 world_from_local;          // reads: converts local -> world
mat4 view_from_world;
vec3 pos_view = view_from_world * world_from_local * vec4(pos_local, 1);
//                    ^ spaces cancel like units: view<-world, world<-local
```

The `dst_from_src` matrix naming is worth adopting immediately. It makes
composition order self-checking: adjacent names must match, exactly like
dimensional analysis. Most matrix-order bugs become visible while typing.

**Building an orthonormal basis** from a single direction — needed for
cameras (`T04.S02`), tangent frames (`T08.S03`), and constraint frames
(`T05.S04`):

```cpp
// Gram-Schmidt against a reference up. Fails when forward is parallel to up.
vec3 f = normalize(forward);
vec3 r = cross(f, world_up);
if (dot(r, r) < 1e-8f) {            // gimbal-ish degenerate case: looking
    r = cross(f, vec3{0,0,1});      // straight up or down
}
r = normalize(r);
vec3 u = cross(r, f);               // already unit: r and f are orthonormal
```

That degenerate branch is not a rare edge case — it is "the camera looks
straight up", which every player does within thirty seconds. The
branchless alternative (Duff et al.'s method, in `[FGED1]`) is worth
knowing for tangent-frame generation where you need it per-vertex.

**Where it bites.** Applying a transform in the wrong direction, i.e. using
`world_from_local` where you needed `local_from_world`. Symptom: things
move the right amount in the wrong direction, or twice as far. The naming
convention above is the cheapest possible defence.

**Read.**
- `[3DMP]` ch. 3 — https://gamemath.com/book/multiplespaces.html — the whole chapter; it is the best treatment of this specific confusion.
- `[FGED1]` §1.7 — orthogonalization and basis construction.

**Check yourself.**
1. Write the composition to take a vertex from bone space to clip space, using `dst_from_src` naming. Which matrices do you need?
2. Why does the basis-building code need a degenerate case, and what does the player do to trigger it?
3. What does `local_from_world` equal, given `world_from_local`, and when is the cheap form of that inverse valid? (See `T02.S02.U05`.)

### U06 — What actually goes in your vector library
**What it is.** The concrete API surface for `A003`. Deliberately small.

**Why it exists.** Because "write a math library" is unbounded and the
useful version is about 40 functions. Scope control is part of the lesson.

**The mechanism.**

```cpp
struct vec2 { float x, y; };
struct vec3 { float x, y, z; };
struct vec4 { float x, y, z, w; };

// arithmetic:      + - * / (vec-vec componentwise, vec-scalar)
//                  += -= *= /=, unary -
// products:        dot, cross (vec3), cross2 (vec2 -> float)
// magnitude:       length, length_squared, distance, distance_squared,
//                  normalize, normalize_or(v, fallback), is_normalized(eps)
// geometric:       project(a,b), reject(a,b), reflect(v,n),
//                  refract(v,n,eta), angle_between(a,b)
// componentwise:   min, max, abs, clamp, saturate, floor, ceil, round,
//                  lerp, step, smoothstep
// queries:         any_nan, is_finite, near_equal(a,b,eps)
// swizzle/convert: xy(), xyz(), to_vec4(v, w)
```

Design decisions to make deliberately, because `A003`'s R2 review asks
about them:

- **Plain struct, not templated over dimension.** Three concrete types
  with duplicated code beats one clever one at this stage. You can read
  the assembly, the debugger shows real values, compile times stay low.
- **`float`, not `double`.** GPUs are float, memory bandwidth is the
  constraint, and double buys you almost nothing until you're doing
  large-world coordinates (where the answer is origin rebasing, not
  doubles).
- **Free functions, not methods**, for the binary operations. `dot(a,b)`
  reads like the math; `a.dot(b)` reads like Java and picks an arbitrary
  asymmetry.
- **No SIMD yet.** `vec3` as three floats is 12 bytes and packs densely.
  A 16-byte SIMD `vec3` wastes 25% of your memory bandwidth for a win you
  cannot yet measure. `T13.S04` revisits this with data.
- **Header-only, with `inline`.** These must inline; a non-inlined `dot`
  in a physics inner loop is a catastrophe.

**Where it bites.** Building the general library. Templates over scalar
type and dimension, expression templates to avoid temporaries, custom
operators for swizzles — each is a day, none of them make a game.

**Read.**
- Read one real one: https://github.com/g-truc/glm (large, general — see what you're choosing not to do) and DirectXMath's design notes (SIMD-first, see the trade-off).
- `[FGED1]` ch. 1 — Lengyel's own library design, which is close to what you want.

**Check yourself.**
1. Argue for and against templating `vec<N,T>` over three concrete types.
2. Why is a 16-byte-aligned `vec3` a poor default despite being faster per-operation?

---

## Implementation

| assignment | size | target rung | what it forces you to understand |
|---|:---:|:---:|---|
| [A003](../assignments/A003-math-core.md) | S | R3 | The API from U06, with tests that catch sign and normalization errors |
| [A005](../assignments/A005-orbit-and-aim.md) | S | R2 | That you can *predict* what dot and cross will give you, by seeing them driven live |

**Order and overlap.** `A003` then `A005`. `A005` is deliberately visual
and deliberately trivial to implement — its value is that you watch the
dot product change as you drag a vector, and the geometric intuition from
U02/U03 stops being something you looked up.

Both feed forward permanently: `A003`'s library is used by every
assignment from `A004` to `A051`. Design its API as if you will be
editing it in two years, because you will be.

**If you are short on time.** `A003` minus `refract`, `smoothstep`, and
the `vec2` cross can be done in two hours. Do not skip the tests —
`A003`'s whole R3 rung is "your tests caught a real error you made", and
they will.

---

## Discussion Log
<!-- Appended per study session. See ../CONVENTIONS.md §7. -->

## Open Questions

## Connections
- `T02.S02` — matrices, which are just "what happens to the basis vectors".
- `T02.S04` — where dot and cross become intersection tests.
- `T04.S01` — barycentric coordinates, which are cross products in disguise.
- `T05.S02` — SAT, which is entirely projections onto axes: dot products all the way down.
