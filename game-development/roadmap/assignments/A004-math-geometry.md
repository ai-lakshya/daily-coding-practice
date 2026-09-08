# A004 — Quaternions, Transforms & Intersection Tests
**Topic:** T02.S03–S04 (also T02.S05) | **Size:** M (1–3 days) | **Target rung:** R3
**Status:** todo
**Code:** `game-development/code/A004-math-geometry/`
**Reuses:** A003 (the math library — extend it, don't fork it), A002 (skeleton) | **Reused by:** A010, A015, A016, A017, A030, A031, A032, A034

## Goal
Complete the math library with rotations and geometric queries, and —
equally important — build the **debug renderer** that everything from
`T05` onward is impossible without.

## Concepts exercised
| unit | first met in | why it recurs here |
|---|---|---|
| T02.S03.U01–U05 | here | All four rotation representations, and the conversions |
| T02.S04.U01–U06 | here | Primitives, closest points, raycasts, overlaps, barycentric |
| T02.S05.U03 | here | Splines, in the testbed |
| T02.S01.U02–U03 | A003 | Every test here is dot and cross products |
| T02.S02.U05 | A003 | The rigid inverse, used to transform rays into object space |

## Prerequisites
`A003`. This *extends* that library — same files, same conventions.

## Specification

### Part 1 — Rotations
1. `quat` type, with the component order stated in a comment
   (`T02.S03.U03`'s trap).
2. `quat_from_axis_angle`, `quat_from_euler(convention stated)`,
   `quat_from_matrix` (Shepperd's branching method — must be correct at
   180°), and the inverses of each.
3. `quat * quat` (composition), `conjugate`, `inverse`, `rotate(q, vec3)`
   using the expanded form, `normalize`.
4. `slerp` **with the `dot < 0` sign flip and the near-parallel `nlerp`
   fallback**, and `nlerp` separately.
5. A `Transform` struct (position + rotation + scale) with composition,
   inverse, and `to_matrix`. Decide and document whether you allow
   non-uniform scale (`T02.S02.U02`) — this is a design decision the
   review will ask about.

### Part 2 — Geometry
6. Primitive types per `T02.S04.U01`: `Ray`, `Segment`, `Plane`, `Sphere`,
   `AABB`, `Capsule`, `Triangle`.
7. Closest-point: `closest_point_on_segment`, `_on_aabb`, `_on_plane`,
   `_on_triangle`, `closest_points_segment_segment`.
8. Raycasts returning the `RayHit` from `T02.S04.U06`: `ray_plane`,
   `ray_sphere`, `ray_aabb` (slab method), `ray_triangle`
   (Möller–Trumbore, returning barycentric `u,v`).
9. Overlaps: `aabb_aabb`, `sphere_sphere`, `sphere_aabb`, `plane_aabb`
   (the signed-distance/extent version).
10. `barycentric(p, a, b, c)` and `point_in_triangle`.
11. The three-tier API from `T02.S04.U06` — boolean, `RayHit`, and a
    `Contact` stub (the contact tier can return normal+depth for
    sphere-sphere and sphere-AABB only; the rest is `A015`).

### Part 3 — The debug renderer and testbed
12. **`DebugDraw`** — a reusable module in `core/`:
    `line`, `ray`, `box`, `aabb`, `sphere`, `capsule`, `triangle`,
    `point`, `text_3d`. One-frame lifetime, batched, colour per call.
    Built on `A002`'s SDL renderer for now (2D projection of 3D is fine —
    it gets replaced properly in `A011`).
13. **An interactive testbed**: primitives you can select and drag with
    the mouse, showing live: the hit point and normal of a raycast, the
    closest point between two selected primitives, and the overlap state.
14. **A rotation comparison view**: three objects rotating from
    orientation A to B over 3 seconds — one via Euler interpolation, one
    via matrix (re-orthonormalized), one via slerp — side by side, with
    orientation A and B chosen to make gimbal lock visible.

### Constraints
- Extend `A003`'s library in place; do not fork it.
- `DebugDraw` must be a genuinely reusable module — no testbed specifics
  inside it.
- No third-party geometry library.

### Explicitly out of scope
SAT, GJK, swept tests, contact manifolds — all `A015`. OBB tests. BVH or
any acceleration structure (`A016`).

## Acceptance criteria
- [ ] Tests for every function, including: `quat_from_matrix` at exactly 180° about each axis; `slerp` between `q` and `-q` takes the short path; `ray_aabb` with a ray exactly parallel to a face and with a ray originating inside; `closest_point_on_triangle` for a point in each of the seven regions; degenerate (zero-length, zero-area) inputs for everything.
- [ ] The rotation comparison view **visibly shows gimbal lock** — you can point at the Euler object and see it swing wrong. Record a GIF or describe the exact orientations in `NOTES.md`.
- [ ] Removing the `dot < 0` check in `slerp` produces a visibly wrong (long-way-round) rotation in the testbed. Confirm this by actually removing it, then put it back.
- [ ] The testbed draws hit points and normals for raycasts against all four primitive types.
- [ ] `DebugDraw` is used by the testbed via its public API only.

## Deliverables
- Extended `core/math/` with `quat.h`, `transform.h`, `geometry.h`
- `core/debug_draw/` — the reusable renderer
- `code/A004-math-geometry/` — the testbed
- `NOTES.md` — the quaternion component order you chose, the non-uniform-scale decision and its justification, and what the gimbal-lock demo actually looked like

## Design notes / hints
- Do the debug renderer **first**, before the geometry. You cannot debug
  `closest_point_on_triangle`'s seven regions without seeing them, and
  trying to will cost you more time than building the renderer.
- Take `closest_point_on_triangle` and `closest_points_segment_segment`
  from `[RTCD]` §5.1.5 and §5.1.9 rather than deriving them. Deriving them
  is a day and teaches less than the rest of this assignment. **Type them
  in, don't paste** — and add the degenerate guards yourself.
- For the gimbal-lock demo: interpolate from `(yaw=0, pitch=85, roll=0)`
  to `(yaw=90, pitch=85, roll=0)`. Near-vertical pitch is where Euler
  interpolation misbehaves most visibly.
- `ray_aabb`'s `1/0` case (`T02.S04.U03`): write the test that has a ray
  exactly along an axis with its origin on a slab boundary, and see what
  your implementation does. Then decide.

## Cut list
1. `Capsule` and `closest_points_segment_segment` (defer to `A015`).
2. `quat_from_euler` / `euler_from_quat` (defer to `A040`'s editor) —
   but then you lose the gimbal-lock demo, which is the best part.
3. The interactive dragging; make it a fixed scene with animated primitives.
4. `text_3d` in `DebugDraw`.

## Rubric

| rung | passes when |
|---|---|
| R0 | Builds clean, tests pass, testbed runs, acceptance criteria met |
| R1 | Every degenerate input in the acceptance list has a tested, defined behaviour. `quat_from_matrix` is correct at 180° (this is the one that fails). No NaN escapes any function. |
| R2 | `DebugDraw` is reusable — you could drop it into `A002`'s skeleton and use it with no edits. The three API tiers are actually separate; the boolean overlap tests don't compute contacts. `Transform`'s scale policy is documented and enforced. |
| R3 | Ray-AABB benchmarked: report ns/test for 1M random rays, with the methodology from `T01.S04.U02`. Compare the branchless slab variant against the branching one and report which won on your machine — this is your first "the obvious optimization didn't help" data point. |
| R4 | See extension below |

### R4 extension
Implement `ray_obb` **without writing a new algorithm**: transform the ray
into the OBB's local space with `inverse_rigid` and call your existing
`ray_aabb`. Then verify the hit normal is transformed back correctly
(hint: it is a normal, so `T02.S02.U06` applies — and for a rigid
transform, what does that reduce to?). This extension is small and it
checks whether the space-transformation material actually landed.

## Common pitfalls
- Skipping the `dot < 0` sign flip because it works on the pairs you tested.
- `quat_from_matrix` using the naive trace-only formula, which fails near 180°.
- Building the testbed as a monolith with `DebugDraw` logic inline, then
  having to extract it under pressure in `A015`.
- Non-unit ray directions, making `t` meaningless (assert on it).
- Deriving `closest_point_on_triangle` from scratch and losing a day.

## Review Log
<!-- One dated block per rung. See ../CONVENTIONS.md §4. -->

## Recall schedule
| due | rung | done | result |
|---|---|---|---|
