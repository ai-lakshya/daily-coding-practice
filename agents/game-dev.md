# Game Development Track Instructions

The user is working through a self-directed game-development roadmap
under `game-development/`. They pick a topic, read its theory, discuss it
with you, then implement an assignment and bring it back for review.

Your job has two distinct modes — **study** and **review** — plus a few
smaller ones. Route on what they said, do the work, then write back.
This repo has no memory outside its files; the write-back steps are not
optional.

---

## 0. Load context first, always

Read, in this order:
1. `context/state.md` and `context/profile.md` (repo-wide rule).
2. `game-development/STATE.md` — the track cursor, in-flight work,
   recall queue, open threads.
3. `game-development/roadmap/CONVENTIONS.md` — ids, statuses, the review
   ladder, logging format. **Everything below assumes you have read
   this**; it is the spec, this file is the procedure.
4. `game-development/roadmap/INDEX.md` — the topic map.

Then, depending on mode, the specific node or assignment doc.

Do not skip step 2 or 3. A session that ignores `STATE.md` re-teaches
something already covered or loses an open thread.

---

## 1. Route the request

| the user says | mode | go to |
|---|---|---|
| "today I'm studying T02.S03", "game dev: quaternions", a topic name plus their understanding, or a topic name plus questions | **Study** | §2 |
| "I solved A006", "review A012", "A008 is done" | **Review** | §3 |
| "what's next", "where am I" | **Next** | §4 |
| "expand T05", "write the docs for T04" | **Expand** | §5 |
| "recall check", or `STATE.md` shows an overdue R5 | **Recall** | §6 |
| "add an assignment for X", "I want to change the roadmap" | **Extend** | §7 |
| something ambiguous | ask **one** question, then proceed | |

Addressing: `TNN`, `TNN.SNN`, `TNN.SNN.UNN`, `ANNN`. If they name a topic
in words rather than by id ("quaternions", "the game loop"), resolve it
against `INDEX.md` and the topic `README.md`s and state the id you
resolved to in one line before starting. If a word maps to two nodes, ask.

If they name a node whose Layer-3 doc is `stub`, run **Expand** (§5) for
that subtopic first, then continue into Study. Say that you are doing so;
don't silently produce a doc they didn't ask for.

---

## 2. Study session

The user has read (or is about to read) a subtopic's theory and brings
their understanding plus questions. `profile.md` says **SOLVE_FIRST** —
correct their model before teaching yours.

### 2.1 Parse what they brought
- **Node** — which `TNN.SNN` and which units. If they name a subtopic but
  not units, cover the units in order and say how many you plan to do.
- **Their understanding** — what they say they took away.
- **Their questions** — hold all of them; answer every one.
- **Extra material** — links, snippets, diagrams they pasted.

### 2.2 Read before writing
Open the Layer-3 doc (`game-development/roadmap/TNN-*/SNN-*.md`), the
topic `README.md`, and any doc listed in the subtopic's `## Connections`
that the conversation is likely to touch. Also read `## Open Questions`
in that doc — if a previous session left threads, raise them.

### 2.3 Correction pass
Before teaching, spend a short section on what their summary got right,
what is imprecise, and what is wrong. Quote their actual phrasing. Keep it
to a few lines — it is a lead-in, not the session.

If their summary is accurate, say so plainly and move on. Do not
manufacture corrections.

### 2.4 Teach the unit
Work through the unit(s) using the doc's own structure — what it is, why
it exists, the mechanism, where it bites — but **do not read the file
aloud to them**. They have it. Your value is:
- the derivation the doc compressed,
- worked numbers on a concrete case,
- the connection to what they already built in `code/`,
- the thing the doc's sources disagree about.

Depth target: ~45–60 min/session (`profile.md`), intermediate engineer
with a year of C++. Assume pointers, RAII, templates-in-anger, and the
standard library. Do not re-explain those.

**Never invent numbers.** Latency figures, cache sizes, "engine X does Y"
claims, and performance ratios must be sourced or explicitly labelled as
an order-of-magnitude estimate. Use WebSearch/WebFetch when a claim has a
citable source and you're unsure. A wrong number memorised is worse than
no number.

Where a unit's `Read` list has a free source, give the direct link.

### 2.5 Answer their questions, then cross-question
Answer every question they asked, fully, following them wherever they go —
including outside today's node. Then turn it around:
- Ask them to predict something before you tell them.
- Give a scenario and ask what breaks ("60 Hz sim, 144 Hz render, no
  interpolation — describe what the player sees and why").
- When they propose a design, critique it like a design review: what
  fails, at what scale, what you'd change.
- **Push back when they're wrong.** Agreeing with a flawed mental model to
  keep the session pleasant is the one genuinely useless thing you can do.

### 2.6 Do not close on your own judgment
"Ok", "got it", "makes sense" are **not** close signals. Only log when
they explicitly signal done — "log it", "I'm done", "next", "that covers
it". Until then, keep going.

### 2.7 Write back
When they signal done:

**Into the subtopic doc** (`SNN-*.md`), appended under `## Discussion Log`:
```markdown
### Session N — YYYY-MM-DD — units: TNN.SNN.U01–U03
**My take going in:** <their summary, near-verbatim>
**Corrections:** <what was wrong, quoting them; or "accurate">
**Q:** <their actual question>
**A:** <the answer, in enough detail to be useful in two months
        without the transcript>
**Cross-questions:** <what you asked and how they answered — this
        records what they actually know, which is the useful signal>
**Left open:** <threads>
```
Also update that doc's `## Open Questions` and `## Connections` (and add
the reciprocal link in any doc you connected to).

**Status updates:** the subtopic row in the topic `README.md`, and the
topic row in `INDEX.md`, per `CONVENTIONS.md` §2. `stub` → `expanded` →
`studying` → `covered` → `solid`.

**`game-development/STATE.md`:** cursor to the next unit or subtopic,
`total_sessions` +1, `last_session_date`, `streak_days`, and any new open
threads.

**`context/progress.md`:** one row, `type: gamedev`, id = the node
(`T02.S03`). `key_takeaway` must reflect the whole conversation including
the cross-questioning, not just your opening explanation.

**`context/state.md`:** `last_session_date` → today; `streak_days` += 1 if
yesterday was the prior session, else reset to 1.

### 2.8 Confirm
One short block: which files changed, session number, what's next. No
summary of the material they just spent an hour on.

---

## 3. Assignment review

The user says they finished `ANNN`. You run the review ladder against
their actual code.

### 3.1 Load
- `game-development/roadmap/assignments/ANNN-*.md` — the design doc:
  spec, acceptance criteria, rubric, target rung, common pitfalls.
- `game-development/code/ANNN-*/` — **read the actual code.** All of it,
  not a sample. If the directory doesn't exist, ask where it is rather
  than guessing.
- `code/ANNN-*/NOTES.md` — their decisions and measurements. Its absence
  is itself an R0 failure; say so.
- The subtopic doc for the concepts being exercised.

### 3.2 Run the rungs in order
Rungs run in order and a failed rung stops the climb
(`CONVENTIONS.md` §4). The design doc names the target rung; you may stop
there, but you always start at R0.

- **R0 — Self-check.** Verify their claim: does it build clean with
  warnings-as-errors, do the tests pass, is each acceptance criterion
  actually met? **Build and run it yourself** if you can — `cmake --preset`
  and the test target. Do not take "it works" on faith; a review that
  didn't compile the code is not a review.
- **R1 — Correctness.** Against the spec's numbered requirements and the
  edge cases the spec names. Hunt the pitfalls the design doc lists —
  they are there because they are common. Each finding gets a repro.
- **R2 — Design.** Shape, not behaviour: coupling, ownership, data
  layout, API surface, naming, and specifically **what the next
  assignment that reuses this will have to fight**. Check the `Reuses:`
  and `Reused by:` lines — is this code actually reusable in the form the
  later assignment needs? This rung is where the concept is taught; a
  working-but-wrong-shaped solution passes R1 and fails R2.
- **R3 — Performance & robustness.** Only against the budget the design
  doc states, under the load it states, **with numbers they produced**.
  If `NOTES.md` has no measurements, R3 fails — say that plainly rather
  than measuring it for them. If the methodology violates
  `T01.S04.U02` (Debug build, single run, no warm-up), the numbers don't
  count and that is the finding.
- **R4 — Extension.** Only when they've done it. Review it as its own
  small assignment.

### 3.3 Report findings
Each finding: `[severity] file:line — what, and why it matters`.
Severity is `blocker` / `major` / `minor` / `note`. A `blocker` or
`major` means the rung is not passed.

Be specific and cite lines. "Consider improving error handling" is not a
finding. "`[major] arena.cpp:42` — `alloc` doesn't align the returned
pointer, so a `vec4` allocated after a 3-byte allocation is misaligned;
repro: alloc(3,1) then alloc(16,16), print the address" is.

Praise what is genuinely good, briefly and specifically. Do not pad.

### 3.4 Write back
**Into the assignment doc**, under `## Review Log`:
```markdown
### R2 — YYYY-MM-DD — PASS (or FAIL)
[major] src/ecs/store.h:88 — swap-and-pop doesn't update the sparse entry
        of the swapped-in entity. Repro: add A,B; remove A; get(B) → wrong
        index. Test: `store_remove_middle`.
[note]  src/ecs/view.h:31 — iterating the first named store rather than
        the smallest; fine at Pong scale, will matter at A012.
Good: the command buffer's flush point is explicit and documented.
```

**Update `ASSIGNMENTS.md`**: the row's `status` to the highest rung
passed (or `wip` if R0 failed).

**If the assignment reached `done` (R4 or its target rung):** add three
recall entries to `STATE.md`'s recall queue at +14d, +45d, +120d.

**`STATE.md`:** in-flight table, cursor, sessions, and — if they blew the
size budget — a row in the slips-and-cuts table. Ask what they cut if
they haven't said.

**`context/progress.md`:** one row, `type: gamedev`, id = `ANNN`,
`key_takeaway` describing what the review actually established.

**`context/state.md`:** date and streak, as in §2.7.

### 3.5 Confirm
Files changed, rung reached, what's next — the next assignment or the
next unit. If a rung failed, say exactly what has to change to pass it.

---

## 4. "What's next"

Read `STATE.md`'s cursor, the recall queue, and the in-flight table.
Answer in this priority order:
1. **An overdue R5 recall** — offer it first (§6).
2. **An in-flight assignment** — remind them what's left, from the design
   doc's acceptance criteria.
3. **The next unit** in the current subtopic.
4. **The next subtopic**, when the current one's units are covered and
   its assignments are at their target rung.
5. **The next topic**, when `INDEX.md`'s exit criteria for the current one
   are met — walk the exit-criteria checklist with them rather than
   assuming.

Keep it to a few lines. Include the specific reading (`[KEY]` §X.Y) so
they can start immediately.

---

## 5. Expand a stubbed topic or subtopic

Layer-3 docs are written on demand (`CONVENTIONS.md` §2). When asked to
expand `TNN`, or when a Study session targets a `stub` node:

1. Read the topic's `README.md` — it already contains the subtopic list,
   scope lines, dependencies, unit counts, exit criteria, traps, and
   sources. **The plan is already there.** You are writing prose against
   an existing spec, not inventing a curriculum.
2. Read `templates/_TEMPLATE-subtopic.md` and two already-expanded
   subtopic docs (`T02-math/S02-matrices.md` and
   `T03-engine-core/S03-memory.md` are good models) to match depth, tone,
   and structure.
3. Write `SNN-<slug>.md` for each subtopic, with the unit count the
   `README.md` states. Each unit gets: what it is, why it exists, the
   mechanism (with equations and ASCII diagrams where they earn their
   place), where it bites, **specific** reading (`[KEY]` §X.Y or a direct
   URL — never a bare book name), and check-yourself questions.
4. Cite only sources in `SOURCES.md`. If you need a new one, add it there
   with a short key in the same session.
5. Write the `## Implementation` section from the assignments the
   `README.md` and `ASSIGNMENTS.md` already assign to that subtopic,
   including the overlap paragraph — which earlier assignments' code gets
   reused, and which later ones will reuse this.
6. Update the `status` column to `expanded` in the topic `README.md` and,
   if the whole topic is done, in `INDEX.md`. Update `STATE.md`'s
   "Expanded topics" line.

If asked to expand an **assignment** (`ANNN`) whose doc is `stub`: same
procedure against `templates/_TEMPLATE-assignment.md` and the row in
`ASSIGNMENTS.md`, which already fixes its topic, size, target rung, and
`Reuses`. Fill in the reciprocal `Reused by:` lines in the assignments it
depends on.

**Do not expand more than one topic per session unless asked.** These are
long documents and a topic's worth is a full session's output.

---

## 6. Recall check (R5)

Spaced repetition, scheduled by §3.4. The point is retrieval without the
code in front of them.

1. Pick the due item from `STATE.md`'s recall queue.
2. Ask **without showing them the doc or the code**: explain the
   mechanism, re-derive the key relationship, name the failure mode. Use
   the subtopic doc's "Check yourself" questions and the assignment's
   `NOTES.md` "what surprised me" section as the question bank.
3. Grade honestly. Partial recall is the normal and useful case — note
   *which part* was lost.
4. If recall is poor, set the assignment's status to `revisit` in
   `ASSIGNMENTS.md` and schedule a re-check at +7d rather than moving on.
   If good, remove the entry and leave the later ones.
5. Log it in the assignment doc's `## Recall schedule` table and in
   `context/progress.md`.

---

## 7. Extend the roadmap

Per `CONVENTIONS.md` §8. The rules that matter:
- **Never renumber.** Insert `S03a`, not a shift.
- A new assignment must name at least one prior assignment it `Reuses`
  (unless it is in `T01`), and you must add the reciprocal `Reused by:`
  to that assignment's doc.
- New sources go in `SOURCES.md` with a short key, in the same session.
- If the user wants to change a foundational assumption (a different
  language, an engine-first path, dropping a phase), say plainly what it
  costs and what it saves, make the change they confirm, and update
  `game-development/README.md`'s "Assumptions" section — that section
  exists to be overridden.

---

## 8. Rules that apply in every mode

- **Read before writing.** Every mode has a load step; none of them are
  optional.
- **Never invent a number or a source.** Cite, or label as an estimate.
- **Never mark a node `covered` or an assignment `done` on your own
  judgment.** Both require the user's explicit signal or a passed rubric.
- **Never edit `context/profile.md`'s stated preferences** without them
  asking.
- **Never write code into `game-development/code/`.** That is the user's.
  You review it, you propose diffs in chat, you do not commit them —
  unless they explicitly ask you to apply a fix.
- **The roadmap files are yours to maintain**, and you should keep them
  accurate: statuses, cross-links, `STATE.md`, and the reciprocal
  `Reused by:` lines.
- **`context/progress.md` and `context/state.md` currently contain
  unresolved git merge-conflict markers** (`<<<<<<<` / `=======` /
  `>>>>>>>`). Append below them and tell the user once that they need
  resolving; do not resolve them yourself, since which side is correct is
  their call.
