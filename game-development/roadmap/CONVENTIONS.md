# Conventions

The rules that make this track machine-readable. An agent picking up a
session reads this file plus `INDEX.md` and knows how to act.

---

## 1. Addressing

| level | id form | example | lives in |
|---|---|---|---|
| Topic (Layer 1) | `TNN` | `T02` | `INDEX.md` row + `TNN-<slug>/` dir |
| Subtopic (Layer 2) | `TNN.SNN` | `T02.S03` | `TNN-<slug>/README.md` row + `SNN-<slug>.md` file |
| Theory unit (Layer 3) | `TNN.SNN.UNN` | `T02.S03.U04` | `## U04 —` heading inside the subtopic file |
| Assignment | `ANNN` | `A017` | `ASSIGNMENTS.md` row + `assignments/ANNN-<slug>.md` |

- Ids are **never reused and never renumbered.** Inserting a subtopic
  between `S03` and `S04` means adding `S03a` — not shifting everything.
  The same goes for units and assignments.
- Slugs are kebab-case, derived from the title, stable once created.
- Directory names are `TNN-<slug>`; subtopic files are `SNN-<slug>.md`
  inside the topic directory (the directory already carries the `TNN`).

---

## 2. Statuses

Used in every registry table (`INDEX.md`, topic `README.md`,
`ASSIGNMENTS.md`).

**Roadmap nodes** (topics, subtopics, units):

| status | meaning |
|---|---|
| `stub` | Listed with scope + sources, but the Layer-3 doc is not written yet |
| `expanded` | Layer-3 doc written, ready to study |
| `studying` | Currently in progress — has at least one session logged |
| `covered` | Theory studied and discussed; assignments may still be open |
| `solid` | Covered **and** every attached assignment reached R4, and a recall check passed |

`stub -> expanded` happens on demand. Do not pre-write 90 theory docs;
they go stale and none of them get read. Expand a topic when you reach it.

**Assignments:**

| status | meaning |
|---|---|
| `todo` | Not started |
| `wip` | Started, code exists in `code/ANNN-*/` |
| `R0`..`R4` | Highest review rung passed (see §4) |
| `done` | R4 passed |
| `revisit` | Failed a later recall check or was superseded — reopen it |

---

## 3. Assignment sizing

| tag | budget | shape |
|---|---|---|
| `XS` | ≤ 1 hour | One idea, one file, no build system changes |
| `S` | 2–4 hours | A single evening; one new subsystem, isolated |
| `M` | 1–3 days | A subsystem plus its integration into the existing codebase |
| `L` | 1–2 weeks | A real system with a design doc that matters, and tuning |
| `XL` | 3+ weeks | A shippable slice: multiple systems, content, polish |

Sizes are **budgets, not estimates.** If an `M` runs to a week, that is a
signal to log in `STATE.md` and cut scope, not to grind. Assignment docs
carry a `Cut list` section for exactly this.

---

## 4. The review ladder

Every assignment is reviewed in rungs. You do not need to climb all of
them for every assignment — the assignment doc names the **target rung**
(`XS`/`S` usually target R2, `M` targets R3, `L`/`XL` target R4) — but the
rungs always run in order, and a failed rung stops the climb.

| rung | name | who | what it checks |
|---|---|---|---|
| **R0** | Self-check | you | Builds clean with warnings-as-errors, runs, tests pass, every acceptance criterion in the design doc is demonstrably met. You assert this before asking for review. |
| **R1** | Correctness | agent | Does it do what the spec says, including the edge cases the spec names? Degenerate inputs, zero/NaN, empty containers, frame-rate extremes, resize, alt-tab. Bugs found here are logged as findings with a repro. |
| **R2** | Design | agent | Is the shape right? Coupling, ownership, data layout, naming, API surface, what a future assignment will have to fight. This rung is where the concept is actually taught — a working-but-wrong-shaped solution passes R1 and fails R2. |
| **R3** | Performance & robustness | agent + you | Measured, not guessed. Meets the budget the design doc states, under the load the design doc states. Requires numbers from your own profiling, not an assertion. |
| **R4** | Extension | you | The design doc names one extension that cannot be done without genuinely understanding the concept. You implement it. This is the rung that converts "I made it work" into "I know how it works". |
| **R5** | Recall | agent | Weeks later, unprompted by the code: explain the mechanism, re-derive the key equation, name the failure mode. Scheduled by spaced repetition, not by you. |

R5 is not part of the initial climb; it is scheduled after `done` at
+14d, +45d, +120d and recorded in the assignment doc's review log.

**Review findings** are recorded in the assignment doc under
`## Review Log` as a dated block per rung, with each finding as
`[severity] file:line — what and why`. Severity is `blocker` / `major` /
`minor` / `note`. A `blocker` or `major` means the rung is not passed.

---

## 5. Theory unit structure

Inside a Layer-3 subtopic file, `## Theory` contains numbered units. Each
unit is one sitting (~20–45 min of reading + discussion) and has:

- **What it is** — 2–4 lines, no history lesson.
- **Why it exists / what breaks without it** — the problem before the solution.
- **The mechanism** — the actual detail. Equations written out. ASCII
  diagrams where they earn their place.
- **Where it bites** — the practical failure modes and gotchas.
- **Read** — 1–4 concrete sources with the *specific* chapter, section, or
  URL. Never "read Real-Time Rendering" — always "RTR4 §5.3, pp. 116–124".
- **Check yourself** — 2–4 questions that cannot be answered by
  paraphrasing. Used for the R5 recall check later.

Sources are always resolvable to an entry in `SOURCES.md`, cited by its
short key (e.g. `[GEA3]`, `[RTCD]`, `[gafferon-timestep]`).

---

## 6. Overlap is a design requirement

An assignment that exercises exactly one concept is a bad assignment.
Every assignment doc carries:

- `Reuses:` — earlier assignments whose code it builds on.
- `Reused by:` — later assignments that will depend on this one.
- `Concepts exercised:` — unit ids (`T02.S01.U03`), including **prior**
  units, not just the current topic's.

When adding a new assignment, it must name at least one prior assignment
it reuses, unless it is in `T01`. If it cannot, it is probably in the
wrong place in the ladder.

---

## 7. Session logging

Every session writes back. This repo has no memory outside its files.

**Study session** appends to the subtopic doc under `## Discussion Log`:
```
### Session N — YYYY-MM-DD — units covered: T02.S03.U01–U03
**My take going in:** (their summary, near-verbatim)
**Corrections:** (what was wrong, quoting them)
**Q:** ... **A:** ... (each real question, with enough answer to be useful
                       in two months without the transcript)
**Left open:** ...
```

**Review session** appends to the assignment doc under `## Review Log`
(see §4) and updates the `ASSIGNMENTS.md` row.

**Both** additionally:
- update `STATE.md` (cursor, in-flight, due recalls),
- update the status column in `INDEX.md` and the topic `README.md`,
- append one row to `context/progress.md` with `type: gamedev` and the
  node or assignment id.

---

## 8. Extending the roadmap

New subtopic: add a row to the topic `README.md` (with `Depends on`),
create the file from `templates/_TEMPLATE-subtopic.md`, do not renumber.

New topic: add a row to `INDEX.md` with its phase and dependencies,
create the directory and `README.md` from `templates/_TEMPLATE-topic.md`.

New assignment: append to `ASSIGNMENTS.md` (next free `ANNN`), create the
design doc from `templates/_TEMPLATE-assignment.md`, and add the
reciprocal `Reused by:` line to whatever it reuses.
