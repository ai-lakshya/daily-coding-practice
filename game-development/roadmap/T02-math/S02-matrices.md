# T02.S02 — Matrices & Linear Transforms
**Topic:** [T02 — Math for Games](README.md) | **Depends on:** T02.S01
**Status:** expanded | **Assignments:** A003 | **Est. theory time:** 2–3 sessions

> The question this subtopic answers: *what is a matrix actually doing to
> space, and why does the order of multiplication keep betraying me?*
>
> The single idea that makes matrices stop being opaque: **the columns of
> a transform matrix are where the basis vectors end up.** If you hold
> that, everything else here is a consequence.

---

## Theory

### U01 — A matrix is where the basis vectors land
**What it is.** A linear transform is fully determined by what it does to
`X`, `Y`, and `Z`. A matrix is just those three results, stored as columns.

**Why it exists.** This reading turns matrix construction from
memorisation into drawing. You want a transform? Ask where the axes go,
write them down as columns, done.

**The mechanism.** With column vectors and `M * v`:

```
        | Xx  Yx  Zx |          M * X = first column
   M =  | Xy  Yy  Zy |          M * Y = second column
        | Xz  Yz  Zz |          M * Z = third column
          ^   ^   ^
          |   |   +--- where Z lands
          |   +------- where Y lands
          +----------- where X lands
```

Check it directly: `M * (1,0,0)` selects column 0. That's the whole proof.

Read transforms off immediately:

- **Scale by (2,3,1)**: X lands at (2,0,0), Y at (0,3,0), Z at (0,0,1) →
  diagonal matrix.
- **Rotate 90° about Z** (right-handed, so counter-clockwise looking down
  -Z): X → (0,1,0), Y → (-1,0,0), Z → (0,0,1):
  ```
   | 0 -1  0 |
   | 1  0  0 |
   | 0  0  1 |
  ```
  You just derived the rotation matrix by drawing two arrows. Do this
  rather than memorising the `cos/-sin/sin/cos` pattern, and you will
  never get the sign wrong again.

**Where it bites.** With **row** vectors (`v * M`), it's the *rows* that
hold the basis images, and matrices appear transposed relative to every
formula written the other way. DirectX documentation historically used row
vectors, OpenGL/GLSL column. Mixing sources without noticing produces
transforms that are transposed — which for a pure rotation means rotating
backwards, and looks *almost* plausible.

**Read.**
- `[3DMP]` ch. 4.2 — https://gamemath.com/book/matrixtransforms.html — this exact geometric interpretation, at length.
- 3Blue1Brown, "Essence of Linear Algebra" ep. 3–4 — https://www.youtube.com/playlist?list=PLZHQObOWTQDPD3MizzM2xVFitgF8hE_ab — if the basis-columns idea isn't yet automatic, this is 20 minutes very well spent.

**Check yourself.**
1. Write the matrix that rotates 90° about Y, by drawing where the axes go. Check the sign against the right-hand rule.
2. What does a matrix whose columns are not mutually perpendicular do to a cube?
3. Given a rotation matrix, how do you read the object's forward vector straight out of it?

### U02 — Composition, order, and why it's backwards
**What it is.** `(A * B) * v = A * (B * v)`. Matrix multiplication is
associative but **not commutative**, and with column vectors it composes
**right to left**.

**Why it exists.** Because `B` is applied to `v` first — it's the inner
parenthesis. So the matrix nearest the vector acts first.

**The mechanism.**

```
   v_world = T * R * S * v_local
             ^   ^   ^
             |   |   +-- 3rd: scale in local space (applied first)
             |   +------ 2nd: then rotate
             +---------- 1st: then translate
```

`T*R*S` is the standard order and it is the one you want, because it means
"scale the object about its own origin, orient it, then place it".
Swapping to `T*S*R` scales along world axes after rotating — which shears
your object when the scale is non-uniform. `R*T` translates first and then
rotates the translation, orbiting the object around the world origin
instead of rotating in place. Both of those are useful *sometimes*, which
is why they're bugs rather than errors: they do something coherent.

**The `dst_from_src` naming from `T02.S01.U05` makes this self-checking:**

```cpp
mat4 clip_from_local = clip_from_view * view_from_world * world_from_local;
//                              view <-  view      world <- world     local
//                      adjacent names match: the composition is right
```

**Hierarchies.** A transform hierarchy is just repeated composition:

```
  world_from_hand = world_from_root * root_from_torso
                  * torso_from_upperarm * upperarm_from_forearm
                  * forearm_from_hand
```

which is exactly the skeleton evaluation in `T09.S01`, and exactly the
scene-graph traversal in a renderer. Nothing new is needed; it's this unit
applied N deep.

**Where it bites.** Non-uniform scale in a hierarchy. A non-uniform scale
on a parent, combined with a child's rotation, produces shear that
propagates down. Some engines (Unreal) accept it; some (older Unity
advice) tell you not to. If your `A004` transform type stores TRS
separately rather than as a matrix, you can *forbid* it structurally,
which is often the right call — that decision is part of `A004`'s design
review.

**Read.**
- `[3DMP]` ch. 4.1.3 and ch. 5.7 — composition and the order question.
- `[FGED1]` §2.3 — transform composition done formally.

**Check yourself.**
1. Draw what `T*R` and `R*T` do to a square placed away from the origin. They differ; say how.
2. Why does non-uniform scale on a parent shear a rotated child? Draw the basis vectors.
3. You have `world_from_local` for a parent and `parent_from_child`. Write the child's world transform.

### U03 — Storage: row-major, column-major, and the two questions people conflate
**What it is.** Two independent conventions that share confusing names.

**Why it exists.** People say "column-major" to mean two different things,
and the resulting confusion is responsible for an enormous amount of lost
time.

**The mechanism.** The two questions:

1. **Vector convention**: are vectors columns (`M*v`) or rows (`v*M`)?
   This is *mathematical* and changes the matrices themselves (they're
   transposes of each other) and the composition order (right-to-left vs
   left-to-right).
2. **Memory layout**: are the 16 floats stored column-by-column or
   row-by-row? This is *storage only* and changes nothing mathematically —
   only how you index and how you upload to the GPU.

The four combinations all exist in the wild. What matters practically:

- **OpenGL/GLSL**: column vectors, column-major storage. `glUniformMatrix4fv`
  has a `transpose` parameter precisely because people show up with the
  other layout.
- **Direct3D/HLSL**: historically row vectors, row-major storage.
  (HLSL has `row_major`/`column_major` qualifiers and D3D's math library
  has changed over time; check per project.)
- **This roadmap**: column vectors, column-major storage, matching GL and
  GLSL so uploads are memcpy with `transpose = GL_FALSE`.

Concretely, for our convention, the translation lives in the last column,
which in column-major memory is elements `[12], [13], [14]`:

```
   memory index:            matrix (column vectors, column-major):
   [ 0  4  8 12 ]              | m0  m4  m8  m12 |     m12,m13,m14
   [ 1  5  9 13 ]              | m1  m5  m9  m13 |     = translation
   [ 2  6 10 14 ]              | m2  m6 m10  m14 |
   [ 3  7 11 15 ]              | m3  m7 m11  m15 |
```

**Where it bites.** A transposed transform is not a crash — it's a rotation
in the opposite direction, or translation that appears in the wrong
component. It looks like a logic bug and it isn't.

Diagnostic that saves hours: **translate an object by (100, 0, 0) and print
the raw 16 floats.** If `100` is at index 12, you're column-major with
translation in the last column. If it's at index 3, you're row-major (or
have a transposed matrix). One test, unambiguous answer.

**Read.**
- `[3DMP]` ch. 4.1.1 and Appendix — the clearest disambiguation of these two questions in print.
- `[OGLWIKI]` — "Data Type (GLSL)" matrix section, on what GL expects.

**Check yourself.**
1. State the two independent questions "column-major" can refer to.
2. Your rotation goes the wrong way. Which of the two conventions is likely wrong, and how do you confirm it in one test?
3. Why does `glUniformMatrix4fv` have a `transpose` argument at all?

### U04 — Homogeneous coordinates and affine transforms
**What it is.** Adding a 4th coordinate `w` so that translation becomes a
matrix multiply, and so that perspective projection is expressible at all.

**Why it exists.** Translation is not linear — it doesn't fix the origin —
so it cannot be a 3×3 matrix. Two options: carry translation separately
forever (some engines do), or lift into 4D where it *is* linear. The 4D
lift also gets you perspective for free, which is the real payoff.

**The mechanism.**

```
        | Xx Yx Zx Tx |     points:     (x, y, z, 1)
   M =  | Xy Yy Zy Ty |     directions: (x, y, z, 0)
        | Xz Yz Zz Tz |
        |  0  0  0  1 |     <- affine: bottom row is (0,0,0,1)
```

With `w = 0`, the translation column is multiplied by zero — so directions
are automatically not translated. That is the whole reason for the point/
vector `w` distinction from `T02.S01.U01`, and it means the *type system of
`w`* does the job that discipline would otherwise have to.

For **projection** matrices the bottom row is not `(0,0,0,1)`; it's
something like `(0,0,-1,0)`, which puts `-z` into `w`. The subsequent
divide by `w` (the "perspective divide", done by the GPU between vertex
and fragment stages) is what makes distant things smaller. `T04.S02`
develops this properly — the point here is only that the 4th row is where
perspective lives, and it is why you need 4×4 rather than a 3×4 affine.

**Where it bites.**
- Multiplying a direction as `(x,y,z,1)` — it gets translated, and your
  "forward" vector wanders as the object moves.
- Forgetting the divide when doing projection math on the CPU (e.g.
  world-to-screen for a HUD marker). The GPU does it for you in the
  pipeline; your CPU code must do it explicitly.
- Assuming `w == 1` after a projection. It isn't; that's the point.

**Read.**
- `[3DMP]` ch. 6.4 — https://gamemath.com/book/matrixmore.html — homogeneous coordinates and the 4D interpretation.
- `[FGED1]` ch. 2.4 and ch. 3 — the rigorous treatment, including why the bottom row is what it is.

**Check yourself.**
1. Why can't translation be a 3×3 matrix?
2. What is `w` after multiplying a point by a perspective matrix, and what is it geometrically?
3. Write world-to-screen for a HUD marker, including every step the GPU would otherwise do for you.

### U05 — Inverses, and the cheap ones
**What it is.** `M⁻¹` undoes `M`. You need it constantly — world-to-local
queries, the view matrix, bone binding, ray casts into object space.

**Why it exists.** Because half of all transform work is going the other
direction, and the general inverse is expensive enough that knowing the
special cases is worth real money.

**The mechanism.** Never call a general 4×4 inverse when you know the
structure:

| matrix is | inverse is | cost |
|---|---|---|
| Pure rotation `R` (orthonormal) | `Rᵀ` | free — it's a transpose |
| Rotation + translation `M = T·R` | `[Rᵀ | -Rᵀ·t]` | ~9 mul |
| `T·R·S` with **uniform** scale `s` | `[Rᵀ/s | -Rᵀ·t/s]` | ~10 mul |
| `T·R·S` non-uniform | build from `S⁻¹R⁻¹T⁻¹` (component-wise) | moderate |
| General affine | 3×3 cofactor inverse + translation | ~40 mul |
| Fully general 4×4 | Laplace/cofactor expansion | ~100+ mul |

The rigid-body case is the important one, because the **view matrix is
exactly this**. A camera at position `p` with orthonormal basis `(r,u,f)`
has `world_from_view = [r u -f | p]`, so:

```
   view_from_world = | rx ry rz  -dot(r,p) |
                     | ux uy uz  -dot(u,p) |
                     |-fx -fy -fz  dot(f,p)|
                     |  0  0  0      1     |
```

That is `gluLookAt` and it is a transpose plus three dot products — not a
matrix inverse. Writing this yourself in `A003` is the moment `Rᵀ = R⁻¹`
stops being a fact and becomes a tool.

**Determinant, read geometrically.** `det(M)` is the **signed volume
scale factor**:
- `det = 1` — rigid (rotation), volume and handedness preserved.
- `det = 8` — everything is 8× the volume (e.g. uniform scale of 2).
- `det = 0` — the transform collapses a dimension; **not invertible**.
  This is what a degenerate basis produces, and it is why the
  basis-building code in `T02.S01.U05` needs its guard.
- `det < 0` — handedness flipped (a mirror). Your triangle winding
  reverses, backface culling inverts, and normals point inward. This is
  the cause of the classic "the model is inside-out after I mirrored it"
  bug, and the fix is to also flip the winding order or the cull face.

**Where it bites.** Calling a general inverse per-object per-frame in a
hot loop. Also: inverting a matrix that has accumulated drift and is no
longer quite orthonormal, then using the transpose shortcut — which is
now subtly wrong. Re-orthonormalize, or store TRS separately and rebuild.

**Read.**
- `[3DMP]` ch. 6.1–6.3 and 6.6 — determinant, inverse, and orthogonal matrices.
- `[FGED1]` §1.7, §2.5 — the derivation of the rigid-body inverse.

**Check yourself.**
1. Why is `Rᵀ = R⁻¹` for a rotation? Prove it from "columns are orthonormal".
2. Derive the view matrix from `world_from_view = [r u -f | p]` without calling `inverse()`.
3. Your mirrored model renders inside out. Explain via the determinant, and give two fixes.

### U06 — The normal matrix
**What it is.** Normals transform by `(M⁻¹)ᵀ` — the inverse transpose of
the upper-left 3×3 — not by `M`.

**Why it exists.** A normal is defined by a perpendicularity constraint
(`n · t = 0` for any tangent `t` of the surface), not by being an arrow
attached to the surface. Transforming it like an arrow breaks the
constraint under non-uniform scale.

**The mechanism.** The picture is the proof. Take a 45° surface and scale
X by 2:

```
  before:  surface  /        normal  \
                   /                  \      (perpendicular)

  after scaling X by 2:
           surface   _____/            naive normal   \
                    /                                  \
                                       -> no longer perpendicular!
                                       the normal needed to become MORE
                                       vertical, but naive scaling made
                                       it more horizontal
```

Derivation: we need `n' · t' = 0` where `t' = M t`. Try `n' = N n`:
```
   n'·t' = (N n)·(M t) = nᵀ Nᵀ M t
```
For this to equal `nᵀ t = 0` for all `t`, we need `Nᵀ M = I`, so
`N = (M⁻¹)ᵀ`.

**When you can skip it:**
- **Pure rotation**: `(R⁻¹)ᵀ = (Rᵀ)ᵀ = R`. The normal matrix *is* `R`.
- **Rotation + uniform scale**: `(M⁻¹)ᵀ = R/s`, which after normalizing
  the result is just `R`. So for uniform scale, use `R` and normalize.
- **Rotation + translation**: translation doesn't affect the 3×3 part.

So: **the normal matrix only differs when there is non-uniform scale.**
Which is exactly why this bug is so annoying — everything works until an
artist scales something non-uniformly, and then lighting is wrong on one
object and nobody knows why.

Always **renormalize after transforming**, even with the right matrix —
scale changes the length.

**Where it bites.** The GLSL you'll write in `A025`:
```glsl
// wrong under non-uniform scale:
vec3 N = normalize(mat3(u_model) * a_normal);
// right:
vec3 N = normalize(u_normal_matrix * a_normal);   // computed CPU-side
```
Computing `transpose(inverse(mat3(model)))` *in the shader*, per vertex, is
a common tutorial pattern and is genuinely expensive — compute it once per
object on the CPU and upload it.

**Read.**
- `[RTR4]` §4.1.7 — "Normal Transform". One page, definitive.
- `[LEARNOGL]` — "Basic Lighting", the normal matrix section — https://learnopengl.com/Lighting/Basic-Lighting
- `[FGED1]` §2.5.2 — the derivation above, done properly.

**Check yourself.**
1. Derive `N = (M⁻¹)ᵀ` from the perpendicularity requirement.
2. For which transforms is the normal matrix equal to `mat3(M)` after normalization?
3. Why is computing the normal matrix in the vertex shader a bad idea?

---

## Implementation

| assignment | size | target rung | what it forces you to understand |
|---|:---:|:---:|---|
| [A003](../assignments/A003-math-core.md) | S | R3 | `mat3`/`mat4`, composition, the rigid-body inverse, the normal matrix — with tests that pin down your convention |

**Order and overlap.** `A003` covers `S01` and `S02` together, because a
vector library without matrices isn't usable. The specific things this
subtopic contributes to `A003`'s spec: `look_at` implemented via the
transpose shortcut (not a general inverse), `inverse_rigid()` as a
separate function from `inverse()`, and a test that asserts your memory
layout (the "translate by 100 and check index 12" test from U03).

This matrix code is then exercised at every level of the roadmap:
`A005` (2D transforms), `A010` (the MVP chain by hand), `A011` (the same
chain, uploaded to GL), `A025` (the normal matrix, for real), `A030`
(skinning matrices), `A040` (editor gizmos, which are inverse transforms
under a mouse ray).

**If you are short on time.** `mat4` with multiply, `look_at`,
`perspective`, `inverse_rigid`, and transform-point/transform-direction is
the minimum that unblocks `T03` and `T04`. `mat3`, general `inverse`, and
`determinant` can wait until `A004`.

---

## Discussion Log
<!-- Appended per study session. See ../CONVENTIONS.md §7. -->

## Open Questions

## Connections
- `T02.S01.U05` — the space naming that makes U02's ordering self-checking.
- `T02.S03` — rotations specifically, and why you often don't want them as matrices.
- `T04.S02` — the projection matrix, where U04's 4th row finally does its job.
- `T08.S03` — tangent space, another basis built the U05 way.
- `T09.S01` — skeleton hierarchies, which are U02's composition N levels deep.
