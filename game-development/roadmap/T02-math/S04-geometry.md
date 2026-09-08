# T02.S04 — Geometry & Intersection
**Topic:** [T02 — Math for Games](README.md) | **Depends on:** T02.S01, T02.S02
**Status:** expanded | **Assignments:** A004 | **Est. theory time:** 2–3 sessions

> The question this subtopic answers: *given two geometric things, do they
> touch, where, and how far apart are they?* This is the vocabulary of
> `T05` (physics), `T10` (AI perception and pathfinding), and every
> "click to select" interaction you will ever write.
>
> These are also, individually, the most-copied functions in games — which
> means it is worth writing them once, carefully, and testing them, rather
> than copying a version whose degenerate-case behaviour you don't know.

---

## Theory

### U01 — Representing the primitives
**What it is.** The handful of shapes games actually use, and the
representation choice for each.

**Why it exists.** Representation determines what the tests cost. Choosing
"plane as three points" instead of "plane as normal + distance" makes
every subsequent query more expensive.

**The mechanism.**

| primitive | representation | notes |
|---|---|---|
| **Ray** | origin `o`, unit direction `d`, optional `t_max` | Parametric: `p(t) = o + t*d`, `t >= 0` |
| **Segment** | `a`, `b` — or `o`, `d`, `t` in `[0,1]` | The `t`-bounded form shares code with rays |
| **Line** | `o`, `d`, `t` unbounded | Rarely what you want; usually you mean a segment |
| **Plane** | unit normal `n`, distance `d`, with `n·p = d` | The single best representation. `n·p - d` is the *signed distance*, free |
| **Sphere** | center `c`, radius `r` | Cheapest volume test there is |
| **AABB** | `min`, `max` (or `center`, `half_extents`) | `center/extents` is better for SAT and for transforming |
| **OBB** | `center`, 3 orthonormal axes, `half_extents` | An AABB in its own space; test by transforming into that space |
| **Capsule** | segment `a`–`b`, radius `r` | The character-collision workhorse — cheap, no corners to catch on |
| **Triangle** | `a`, `b`, `c` | Winding order defines the normal (`T02.S01.U03`) |
| **Convex polygon/hull** | ordered vertices (2D) / faces + verts (3D) | The input to SAT and GJK |
| **Frustum** | 6 planes, inward normals | Culling (`T04.S06`) |

The `n·p = d` plane form deserves emphasis: it makes "which side", "how
far", and "project onto the plane" all one dot product. Storing three
points instead means recomputing the normal every query.

**Where it bites.** Storing AABBs as `min`/`max` and then needing
`center`/`extents` in every SAT axis test, or vice versa. Pick based on
your dominant use: `min`/`max` for broadphase overlap tests and BVH
construction, `center`/`extents` for SAT and for transforming (a rotated
AABB's new extents are `|R| * extents` with `|R|` the componentwise
absolute value — a neat trick worth knowing, from `[RTCD]` §4.2.6).

**Read.**
- `[RTCD]` ch. 4 — bounding volumes, all of them, with the trade-offs.
- `[3DMP]` ch. 9 — https://gamemath.com/book/geomprims.html — primitive representations.

**Check yourself.**
1. Why is `n·p - d` the signed distance to a plane, given `n` is unit?
2. When would you store an AABB as `center`/`extents` rather than `min`/`max`?
3. Why are capsules preferred over boxes for character collision?

### U02 — Closest-point queries
**What it is.** Given a point and a primitive, the nearest point on that
primitive. The building block that most distance and intersection tests
reduce to.

**Why it exists.** Because "distance between A and B" is usually solved by
"closest point on A to B, then measure" — and because closest-point is
what you need for contact generation in `T05.S02` anyway.

**The mechanism.**

**Point → segment.** The pattern to internalise; almost everything else is
a variation:
```cpp
vec3 closest_point_on_segment(vec3 p, vec3 a, vec3 b) {
    vec3 ab = b - a;
    float t = dot(p - a, ab) / dot(ab, ab);   // project, normalized by |ab|²
    t = clamp(t, 0.0f, 1.0f);                 // the clamp IS the segment-ness
    return a + t * ab;
}
```
Two observations that generalise:
- The projection is `T02.S01.U02`'s dot product doing its one job.
- The **clamp is what distinguishes a segment from a line.** For a ray,
  clamp to `[0, inf)`; for a line, don't clamp. One function, three shapes.
- `dot(ab,ab)` is zero for a degenerate segment (`a == b`). Guard it.

**Point → AABB.** Componentwise clamp — that's the whole algorithm:
```cpp
vec3 closest = clamp(p, box.min, box.max);
```
And therefore **point-in-AABB distance** is `length(p - closest)`, and
**sphere-vs-AABB** is `length_squared(p - closest) <= r*r`. Two of the most
used tests in games, both one line, once you have this.

**Point → plane.** `p - n * (dot(n,p) - d)`. The signed distance again.

**Point → triangle.** The genuinely fiddly one: the closest point may lie
on the face, on any of three edges, or at any of three vertices — seven
regions (a Voronoi region decomposition). Don't derive it; take the
barycentric-region version from `[RTCD]` §5.1.5, which handles all seven
without branching on the region explicitly.

**Segment → segment.** The one everybody needs and nobody remembers.
Needed for capsule-vs-capsule, which is character-vs-character collision.
`[RTCD]` §5.1.9. Note the parallel-segments degenerate case; it is not
optional.

**Where it bites.** Degenerate inputs. Zero-length segments, zero-area
triangles, and duplicated vertices all appear in real mesh data and all
produce divide-by-zero in the naive forms. Every closest-point function in
your library should have a test for its degenerate case, and `A004`'s
rubric asks for exactly that.

**Read.**
- `[RTCD]` ch. 5.1 — closest-point computations, the whole section. This is the reference chapter for this unit and you will return to it in `T05`.
- `[3DMP]` ch. 9.4–9.5.

**Check yourself.**
1. Modify point-to-segment into point-to-ray by changing one line. Which line, and to what?
2. Derive sphere-vs-AABB from point-to-AABB. Why is no square root needed?
3. Why does point-to-triangle have seven cases? Sketch the regions.

### U03 — Ray casts: the workhorse queries
**What it is.** `ray vs {plane, sphere, AABB, triangle}`, returning the
parametric `t` of the hit.

**Why it exists.** Raycasts are how a game asks questions about the world:
line of sight (`T10.S04`), what did the bullet hit, what is under the
mouse cursor (`A040`), ground detection for a character (`T05.S05`),
shadow and AO tests. Along with AABB overlap, these are the most-executed
geometric functions in a game.

**The mechanism.**

**Ray-plane.** Substitute the ray into the plane equation and solve:
```
   n · (o + t*d) = D    ->    t = (D - n·o) / (n·d)
```
`n·d == 0` means the ray is parallel to the plane: no hit (or infinitely
many, if it lies in the plane). Guard it.

**Ray-sphere.** Substituting gives a quadratic in `t`. Solve, take the
smaller non-negative root. The geometric formulation is more numerically
stable and faster to reject:
```cpp
vec3  m = ray.o - sphere.c;
float b = dot(m, ray.d);          // ray.d must be unit
float c = dot(m, m) - r*r;
if (c > 0 && b > 0) return false; // origin outside AND pointing away
float disc = b*b - c;
if (disc < 0) return false;       // no real root: misses
float t = -b - sqrt(disc);        // near intersection
if (t < 0) t = 0;                 // origin inside the sphere
```
Note the two early-outs before the `sqrt`. Both are free and both cull the
common case.

**Ray-AABB: the slab method.** The elegant one, and worth knowing cold.
Treat the box as the intersection of three slabs (one per axis). For each
axis compute the `t` interval during which the ray is inside that slab;
intersect the three intervals; non-empty means a hit.
```cpp
float tmin = 0.0f, tmax = ray.t_max;
for (int i = 0; i < 3; ++i) {
    float inv = 1.0f / ray.d[i];            // see note below
    float t1 = (box.min[i] - ray.o[i]) * inv;
    float t2 = (box.max[i] - ray.o[i]) * inv;
    if (t1 > t2) std::swap(t1, t2);
    tmin = std::max(tmin, t1);
    tmax = std::min(tmax, t2);
    if (tmin > tmax) return false;
}
```
```
   axis X:        [------------]        slab interval on X
   axis Y:              [--------------]
   axis Z:          [----------]
   intersection:        [--]            <- non-empty: HIT
```
**The `d[i] == 0` case:** the ray is parallel to that axis's slab. `1/0`
gives ±inf, and the arithmetic — remarkably — still works out correctly
under IEEE 754, *except* when the origin lies exactly on a slab boundary,
which yields `0 * inf = NaN`. `[RTCD]` §5.3.3 and Tavian Barnes'
well-known writeup (https://tavianator.com/2011/ray_box.html and its
2022 follow-up) cover the robust variants. Know that this trap exists;
you will hit it in `A016`'s BVH.

**Ray-triangle: Möller–Trumbore.** The standard. It computes the
barycentric coordinates `(u,v)` and `t` simultaneously without
precomputing the plane, which makes it memory-efficient for meshes — you
store only the three vertices. The `u,v` output is exactly what you need
to interpolate UVs and normals at the hit point, which you will want in
`A010`.

**Where it bites.**
- **Non-unit ray direction.** Most of these formulas assume `|d| = 1`. If
  it isn't, `t` is in units of `|d|` rather than distance, and comparisons
  between hits from different rays are meaningless. Assert on it.
- **Self-intersection / shadow acne.** A ray starting exactly on a surface
  re-hits that surface at `t ≈ 0` due to floating point. Standard fix:
  offset the origin along the normal by an epsilon, or pass a `t_min`.
  This same problem reappears as shadow acne in `T08.S04` — it is the
  same bug in a different costume.

**Read.**
- `[RTCD]` ch. 5.3 — all four tests, with the robustness discussion.
- `[SCRATCH]` — "Ray-Tracing: Rendering a Triangle" — Möller–Trumbore derived: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/
- Tavian Barnes on ray-box — https://tavianator.com/2022/ray_box_boundary.html — the NaN edge case, done thoroughly.

**Check yourself.**
1. Explain the slab method in one sentence, without formulas.
2. What are the two pre-`sqrt` early-outs in ray-sphere, and what does each reject?
3. Why does a ray starting on a surface hit that surface, and name two fixes.
4. What do Möller–Trumbore's `u` and `v` outputs give you beyond a yes/no?

### U04 — Overlap tests between volumes
**What it is.** Boolean (and sometimes penetration-depth) tests between
pairs of volumes.

**Why it exists.** This is broadphase and narrowphase collision
(`T05.S02`, `T05.S03`), trigger volumes, and culling.

**The mechanism.** In increasing cost — and this ordering *is* the design
of a collision system, because you use the cheap ones to reject before
running the expensive ones:

**Sphere-sphere** — the cheapest useful test:
```cpp
float r = a.radius + b.radius;
return length_squared(a.c - b.c) <= r * r;      // no sqrt
```

**AABB-AABB** — separating axis on the three world axes, which is just
three interval overlaps:
```cpp
return a.min.x <= b.max.x && a.max.x >= b.min.x
    && a.min.y <= b.max.y && a.max.y >= b.min.y
    && a.min.z <= b.max.z && a.max.z >= b.min.z;
```
Note this branches early on the first separating axis, which is why AABB
tests are so fast in practice: most pairs are separated on the first axis
checked.

**Sphere-AABB** — from U02: `length_squared(c - clamp(c, min, max)) <= r²`.

**Plane-AABB / frustum-AABB** — project the box's extents onto the plane
normal to get its "radius" along that normal, then compare with the
center's signed distance:
```cpp
float r = dot(box.extents, abs(plane.n));       // the projected radius
float s = dot(plane.n, box.center) - plane.d;   // signed distance
// s < -r : entirely behind;  s > r : entirely in front;  else straddling
```
This is the core of frustum culling in `T04.S06` — six of these per box.
The `abs(plane.n)` trick is worth understanding: it computes the extent
projection without needing to test all 8 corners.

**OBB-OBB / convex-convex** — the separating axis theorem, which is
`T05.S02`'s material. The idea: two convex shapes are disjoint iff there
exists an axis on which their projections don't overlap, and for polyhedra
it suffices to test the face normals of both plus all pairwise
cross-products of edges (15 axes for two OBBs). Note this is `T02.S01`'s
dot product, applied 15 times.

**Where it bites.** Using an overlap test where you need a *swept* test.
A fast bullet moving 20 m in one 16 ms frame will pass entirely through a
0.2 m wall between frames, and every discrete overlap test returns "no
collision". This is tunnelling; it's `T05.S02`'s problem and it is worth
knowing the shape of it now so it isn't a surprise.

**Read.**
- `[RTCD]` ch. 4.2–4.4 (AABB, sphere, OBB tests) and ch. 5.2 (dynamic/swept tests).
- `[RTR4]` §22.10–22.14 — the same tests, with the rendering/culling emphasis.

**Check yourself.**
1. Why is `length_squared` comparison correct for sphere-sphere, and what does it save?
2. Derive the plane-AABB test. Why does `abs(n)` give the projected extent?
3. Give the numbers for a tunnelling case: a projectile speed, a wall thickness, and a frame rate where it fails.

### U05 — Barycentric coordinates
**What it is.** Expressing a point inside a triangle as a weighted
combination of its vertices: `p = u*a + v*b + w*c`, with `u+v+w = 1`.

**Why it exists.** It is the answer to "interpolate anything across a
triangle" — which is what a rasterizer does for every attribute of every
pixel (`T04.S01`), what a raycast needs to get the UV at a hit, and what
navmesh pathfinding uses to place a point on a polygon (`T10.S02`).

**The mechanism.** The weights are **area ratios**:

```
              c
             /|\
            / | \            u = area(p,b,c) / area(a,b,c)
           /  |  \           v = area(a,p,c) / area(a,b,c)
          /  .p   \          w = area(a,b,p) / area(a,b,c)
         /  /   \  \
        a-----------b        and u + v + w = 1 always
```

- All three in `[0,1]` ⟺ `p` is inside the triangle. That is the standard
  point-in-triangle test.
- Negative weight ⟺ `p` is on the far side of the opposite edge. The
  *sign pattern* tells you which region, which is how the seven-region
  closest-point-on-triangle from U02 works.
- Because areas come from cross products (`T02.S01.U03`), the whole thing
  is three cross products — and in 2D, three `cross2` scalars.

The efficient implementation (Christer Ericson's, `[RTCD]` §3.4) solves a
2×2 system built from dot products, and is what you should use in
`A009`'s rasterizer inner loop.

**The perspective-correction caveat.** Interpolating attributes with
barycentric weights computed in *screen space* is wrong under perspective
— it gives affine interpolation, and textures on a floor visibly warp and
swim. The fix is to interpolate `attribute/w` and `1/w` linearly, then
divide: this is perspective-correct interpolation, the GPU does it for
you, and in `A010` you will do it yourself and see exactly what it fixes.
`T04.S01` develops it.

**Where it bites.** Degenerate triangles (zero area) make the denominator
zero. Real meshes contain them. Guard, and decide what a degenerate
triangle should do in your renderer (usually: skip it).

**Read.**
- `[RTCD]` ch. 3.4 — barycentric coordinates, with the efficient formulation.
- `[SCRATCH]` — "Rasterization: a Practical Implementation", the barycentric and perspective-correct sections: https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/
- `[3DMP]` ch. 9.6.3.

**Check yourself.**
1. State the point-in-triangle test using barycentric coordinates.
2. What does a negative `v` tell you geometrically?
3. Why is screen-space barycentric interpolation wrong under perspective? What is the visual symptom?

### U06 — Building the query API
**What it is.** The design of the intersection module in `A004`, and the
conventions that make it usable in `T05`.

**Why it exists.** These functions get called from physics, AI, rendering,
and gameplay, each wanting slightly different information. Designing the
return types once saves a lot of adapter code.

**The mechanism.** Three tiers, because the callers genuinely differ:

```cpp
// Tier 1: boolean. Broadphase, triggers, culling. Cheapest.
bool intersects(const AABB& a, const AABB& b);
bool intersects(const Sphere& s, const AABB& b);

// Tier 2: parametric hit. Raycasts.
struct RayHit {
    bool  hit;
    float t;          // parametric distance along the ray
    vec3  point;      // o + t*d, cached because everyone wants it
    vec3  normal;     // surface normal at the hit, facing the ray
    // + primitive id / barycentric uv, when meshes are involved
};
RayHit raycast(const Ray& r, const Sphere& s);

// Tier 3: contact. Physics response needs penetration, not just overlap.
struct Contact {
    vec3  normal;     // direction to separate B from A (unit)
    float depth;      // penetration along that normal
    vec3  point;      // contact point in world space
};
bool  collide(const Sphere& a, const AABB& b, Contact* out);
```

Conventions to fix *now* and document in the header, because changing them
later touches everything:
- **Normal direction**: always points from B toward A (so A can be pushed
  out along it). Or the reverse — but pick, and state it.
- **Touching counts as intersecting** (`<=` not `<`). Consistency matters
  more than the choice.
- **Ray `t` is in world units**, which requires unit direction.
- **Out-params vs optional returns**: out-params via pointer let the caller
  skip the work when they only want the boolean. That is why Tier 1 exists
  separately at all.

**Where it bites.** Returning `Contact` from everything. Broadphase calls
these millions of times per frame and does not want a penetration depth
computed. The tiering is a performance decision, not an aesthetic one, and
`A016` will show you the cost if you skip it.

**Read.**
- `[RTCD]` ch. 5 introduction — Ericson's discussion of what these functions should return, informed by having built them for shipped games.
- Read Box2D's `b2Manifold` and Bullet's `btManifoldPoint` for two real answers to the Tier-3 design question.

**Check yourself.**
1. Why have a boolean-only tier when the contact version returns the same information?
2. What breaks if two callers disagree about which way the contact normal points?
3. Why must a `RayHit` normal face the ray, and what is the alternative convention?

---

## Implementation

| assignment | size | target rung | what it forces you to understand |
|---|:---:|:---:|---|
| [A004](../assignments/A004-math-geometry.md) | M | R3 | The full query set with degenerate cases handled, and a visual testbed where you drag primitives and watch the results |

**Order and overlap.** `A004` bundles `S03` (rotations) and `S04`
(geometry) because they share a testbed: a scene where you can move and
rotate primitives with the mouse and see hit points, normals, and closest
points drawn live.

That testbed is the real deliverable. It is the first version of the
**debug renderer** that `T05` is impossible without — `A015`'s spec
requires it, `A016` visualizes broadphase cells with it, `A034` draws
paths and navmesh polygons with it, and `A040` turns it into an editor.
Build it as a reusable module (`DebugDraw::line/box/sphere/text`), not as
scaffolding inside `A004`'s main.

Every function here is used again in `T05.S02` — the difference is that
there you'll need the *swept* versions, and you will be glad the static
ones were written carefully.

**If you are short on time.** Minimum viable set for unblocking `T03`/`T04`:
ray-plane, ray-sphere, ray-AABB (slab), AABB-AABB, sphere-sphere,
sphere-AABB, closest-point-on-segment, barycentric. Defer ray-triangle,
segment-segment, and OBB tests until `A009` and `A015` need them.

---

## Discussion Log
<!-- Appended per study session. See ../CONVENTIONS.md §7. -->

## Open Questions

## Connections
- `T02.S01.U02`, `U03` — every test here is dot and cross products in a trench coat.
- `T04.S01` — barycentric coordinates become the rasterizer's inner loop.
- `T04.S06` — the plane-AABB test becomes frustum culling.
- `T05.S02` — the swept and penetration-depth versions of everything here.
- `T10.S02`, `T10.S04` — pathfinding and line-of-sight, built on raycasts.
