# A003 — Vector & Matrix Math Library with Tests
**Topic:** T02.S01–S02 | **Size:** S (2–4h) | **Target rung:** R3
**Status:** todo
**Code:** `game-development/code/A003-math-core/`
**Reuses:** A002 (the skeleton; this becomes a `core/math` module) | **Reused by:** every assignment from A004 to A051

## Goal
The longest-lived code you will write in this track. Afterwards you own a
math library whose conventions you chose deliberately, whose edge cases
you have tested, and which you will still be editing in two years.

## Concepts exercised
| unit | first met in | why it recurs here |
|---|---|---|
| T02.S01.U02–U04 | here | Dot, cross, normalize, and their numerical traps |
| T02.S01.U05 | here | The `dst_from_src` naming convention, adopted permanently |
| T02.S01.U06 | here | The API surface, scoped deliberately |
| T02.S02.U01–U03 | here | Basis columns, composition order, memory layout |
| T02.S02.U05 | here | The rigid-body inverse, and `look_at` built from it |
| T02.S02.U06 | here | The normal matrix |
| T01.S03.U03 | A002 | Asserts on the degenerate cases |

## Prerequisites
`A002`'s skeleton. The library goes in `core/`, header-only, with a test
executable target.

## Specification

1. **Types**: `vec2`, `vec3`, `vec4`, `mat3`, `mat4`. Plain structs,
   `float`, no templates over dimension.
2. **Convention header comment**, stated explicitly at the top of the
   file, covering: handedness, up axis, forward axis, vector convention
   (column), memory layout (column-major), angle units. This comment is
   graded — it is the artifact that stops future-you from contradicting
   past-you.
3. **Vector ops** per `T02.S01.U06`'s list. Include both `length` and
   `length_squared`, and `normalize` plus `normalize_or(v, fallback)`.
4. **Matrix ops**:
   - `mat4 * mat4`, `mat4 * vec4`
   - `transform_point(m, vec3)` (w=1) and `transform_direction(m, vec3)` (w=0) as *separate named functions* — this is the U01 point/vector distinction made structural
   - `translate`, `rotate_x/y/z`, `rotate_axis_angle`, `scale`
   - `transpose`, `determinant`, `inverse` (general), **`inverse_rigid`** (transpose shortcut, asserts orthonormality in debug)
   - `look_at(eye, target, up)` — implemented via the transpose shortcut, **not** by calling `inverse`
   - `perspective(fovy, aspect, near, far)`, `orthographic(...)`
   - `normal_matrix(mat4)` returning `mat3`
5. **Tests** (doctest or Catch2, fetched via CMake). At minimum:
   - Identity properties, associativity of composition
   - `inverse(m) * m == identity` within epsilon, for a `T*R*S` matrix
   - `inverse_rigid` agrees with `inverse` for a rigid matrix
   - **The layout test**: build `translate({100,0,0})`, memcpy to a
     `float[16]`, assert `f[12] == 100` — pinning your convention
     (`T02.S02.U03`)
   - **The order test**: assert `T*R != R*T` for a specific case, and that
     `T*R` matches a hand-computed expected result
   - `look_at` produces a view matrix that maps the target to the -Z axis
   - `normal_matrix` under non-uniform scale: a normal on a 45° plane
     stays perpendicular to a tangent after transform; and demonstrate
     that `mat3(model)` does **not**
   - Degenerate: `normalize` of zero, `acos` clamping, `inverse` of a
     singular matrix
6. **Debug-build asserts** on the preconditions: unit-length inputs where
   assumed, non-degenerate inputs where required.

### Constraints
- No GLM, no Eigen, no third-party math. That is the entire assignment.
- Header-only with `inline`; the functions must inline in release.
- No SIMD, no alignment attributes (`T02.S01.U06` — this is revisited at `T13.S04`).
- `-Wall -Wextra -Werror` clean.

### Explicitly out of scope
Quaternions and intersection tests (`A004`). Swizzles beyond `xy()`/`xyz()`.
Expression templates. Anything `double`.

## Acceptance criteria
- [ ] All tests pass, including every degenerate case listed above.
- [ ] The convention comment exists and matches what the code actually does (verify by the layout test, not by reading).
- [ ] **At least one test caught a real error you made.** Write down which, in `NOTES.md`. If nothing did, your tests are too weak — add the order test and the normal-matrix test, which catch the two most common errors.
- [ ] `look_at` contains no call to `inverse`.
- [ ] Release build: confirm `dot` inlines (check the disassembly or the optimization remarks).

## Deliverables
- `code/A003-math-core/core/math/` — the library
- `code/A003-math-core/tests/` — the test target
- `NOTES.md` — the conventions you chose *and why*, plus the error your tests caught

## Design notes / hints
- Write the convention comment **first**, before any code. Every ambiguity
  you resolve later will be resolved against it.
- The normal-matrix test is the one people skip and it is the one that
  catches the subtlest bug. Set it up as: a plane at 45°, a tangent vector
  in it, a normal; apply `scale(2,1,1)`; assert `dot(N*n, M*t) ≈ 0` and
  assert `dot(mat3(M)*n, M*t) != 0`.
- For `inverse_rigid`'s orthonormality assert: check that the three basis
  columns are unit and mutually perpendicular to within `1e-4`.
- Keep the file count low. `vec.h`, `mat.h`, `math_common.h` is plenty.

## Cut list
1. `orthographic`, `determinant`, general `inverse` (keep `inverse_rigid`).
2. `mat3` as a separate type (return `mat4` from `normal_matrix` and use its upper-left).
3. `refract`, `smoothstep`, `reject`.
4. **Never cut: the tests.** They are the R3 rung.

## Rubric

| rung | passes when |
|---|---|
| R0 | Builds clean, all tests pass, acceptance criteria met |
| R1 | Every degenerate case has a defined, tested behaviour — no UB, no NaN escaping a function silently. UBSan clean under the test suite. |
| R2 | The API reads like the math (`dot(a,b)`, not `a.dot(b)`). Point/direction transforms are separate named functions. The convention comment is accurate and complete. No function does two things (`normalize` doesn't also assert-and-log-and-fallback silently — the policy is one thing, stated). |
| R3 | `dot`, `length_squared`, and `mat4*vec4` inline in release — verified by looking at the generated code, not assumed. Report the size of `mat4` and confirm it is 64 bytes with no padding. |
| R4 | See extension below |

### R4 extension
Write a **property-based test**: generate random `T*R*S` matrices and
random points, and assert that
`transform_point(inverse(M), transform_point(M, p)) ≈ p` over 10,000
random cases, with a tolerance you justify from the float analysis in
`T02.S01.U04`. Then find the tolerance at which it starts failing and
explain *why* it fails there — what magnitudes, what cancellation. This
converts "I tested it" into "I know its numerical limits".

## Common pitfalls
- Getting `T*R` vs `R*T` backwards and not noticing because your test
  scene has everything at the origin. (Hence the order test.)
- Implementing `look_at` by building `world_from_view` and calling
  `inverse` — correct, slower, and it means you didn't learn `T02.S02.U05`.
- A `normalize` that silently returns garbage for the zero vector.
- Forgetting to clamp before `acos` in `angle_between`.
- Row-major storage with column-vector math, or the reverse, discovered
  three assignments later when nothing renders.

## Review Log
<!-- One dated block per rung. See ../CONVENTIONS.md §4. -->

## Recall schedule
| due | rung | done | result |
|---|---|---|---|
