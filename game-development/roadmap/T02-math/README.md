# T02 — Math for Games
**Phase:** A | **Depends on:** T01 | **Borrows from:** —
**Status:** expanded | **Assignments:** A003–A005

## Why this topic, here
This is the highest-leverage topic in the entire roadmap and it is second
for a reason: nothing else can start until it is done. A renderer is
matrix math with a GPU attached. A physics engine is vector math with a
solver attached. Animation is quaternion math with an interpolation
schedule attached. Every hard bug in `T04`–`T09` is, more often than not,
a transform in the wrong space.

The goal is **not** to be able to follow the derivations. It is to have
the geometric intuition so immediate that when a character is inside out,
you know it's a determinant sign before you open the debugger.

You will write the library yourself (`A003`, `A004`), and then use it in
almost every subsequent assignment. That library is the single longest-
lived piece of code you write in this track — design it accordingly.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | [Vectors & Spaces](S01-vectors.md) | Vector ops, dot and cross products and what they *mean*, projection, bases, coordinate spaces | — | 6 | A003, A005 | expanded |
| S02 | [Matrices & Linear Transforms](S02-matrices.md) | Transform matrices, composition order, affine vs linear, the model-view-projection chain, inverses, the normal matrix | S01 | 6 | A003 | expanded |
| S03 | [Rotation Representations](S03-rotations.md) | Euler angles, axis-angle, rotation matrices, quaternions, slerp, and choosing between them | S02 | 5 | A004 | expanded |
| S04 | [Geometry & Intersection](S04-geometry.md) | Rays, planes, spheres, AABBs, OBBs, triangles: closest point, distance, and intersection tests | S01, S02 | 6 | A004 | expanded |
| S05 | [Interpolation, Curves & Numerical Care](S05-interpolation.md) | Lerp/nlerp/slerp, easing, Bezier and Catmull-Rom splines, floating-point behaviour and epsilons | S01, S03 | 5 | A004, A005 | expanded |

## Exit criteria
- [ ] Given two vectors you can say what the dot and cross products mean geometrically, without computing them.
- [ ] You can write, from memory, the matrix for a rotation about an arbitrary axis, and say what its determinant must be.
- [ ] You can explain what "column-major" and "row-vector" independently mean, and why `M * v` vs `v * M` is a convention choice, not a correctness one.
- [ ] You can derive why the normal matrix is the inverse transpose, and demonstrate a case where using the model matrix directly is visibly wrong.
- [ ] You can state three concrete failure modes of Euler angles and explain why quaternions fix two of them but not the third (interpretability).
- [ ] You can implement ray-AABB (slab method) and ray-triangle (Möller–Trumbore) from your notes.
- [ ] You can explain why `nlerp` is not `slerp`, and when the difference is invisible.
- [ ] Your math library has tests, and at least one of those tests catches a sign or order error you actually made.

## Traps specific to this topic
- **Learning the algebra without the geometry.** If you can compute a
  cross product but can't predict which way it points, you have not
  learned it. Every unit here leads with the picture.
- **Handedness and convention drift.** Pick one convention (this roadmap
  uses right-handed, column-major, column vectors, `M*v`) and write it at
  the top of your math header. Half the transform bugs in games are two
  files disagreeing about this.
- **Building a "general" math library.** You need `vec2/3/4`, `mat3/4`,
  `quat`, and about forty functions. Templating it over dimension and
  scalar type is a week you will never get back at this stage.
- **Skipping the numerical unit (`S05`).** It looks like the boring one.
  It is the one that explains why your collision detection jitters.

## Primary sources for the whole topic
- `[3DMP]` — free, and the best geometric-intuition text on this list. This is your spine for T02.
- `[FGED1]` — denser and more rigorous; use it when `[3DMP]` leaves you wanting the derivation.
- `[FREYA]` — watch the quaternion and spline videos before reading about them
- `[RTCD]` ch. 3–5 — for `S04`, and you will return to it for all of `T05`
