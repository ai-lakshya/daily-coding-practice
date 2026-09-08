# T16 — Shipping: Scope, Production & Release
**Phase:** E | **Depends on:** T07, T12 | **Borrows from:** —
**Status:** stub | **Assignments:** A050–A051

## Why this topic, here
The last topic, and the one that determines whether any of the previous
fifteen produced anything a person can play. The skill it teaches is not
technical: it is **finishing**, which is a distinct discipline with its
own techniques, and which is where the large majority of game projects
die.

It is placed last because its assignments (`A050`, `A051`) require
everything else to exist, but its *content* — particularly `S01`'s
scoping — is worth reading early. If you read one subtopic ahead of
schedule, read `S01` before starting `A023`.

`S05` (career) is included because the honest answer to "how do I become
a game developer" involves a portfolio and an interview loop, and both
have specific, learnable shapes.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | Scoping & Production | Vertical slice vs prototype vs demo, milestone structure, estimation and why yours is wrong, the cut list as a design tool, risk registers, and the mechanics of actually finishing | T07.S03 | 6 | A050 | stub |
| S02 | Build & Release Engineering | CI for games, reproducible builds, packaging and installers, versioning and build ids, symbol servers and crash reporting, telemetry, and patching/DLC structure | T12.S01 | 6 | A051 | stub |
| S03 | Polish, QA & Certification | Bug triage and severity, the bug-fix curve near ship, platform requirements (TRC/TCR/lotcheck) at an overview level, accessibility as a design requirement, localization plumbing | S01 | 5 | A051 | stub |
| S04 | Launch & After | Store pages and what converts, capsule/trailer basics, wishlists and the pre-launch window, community, launch-day operations, patches, and writing a real postmortem | S02, S03 | 5 | A051 | stub |
| S05 | Career: Portfolio, Interviews & the Technical Test | What studios actually assess, how to structure a portfolio around this roadmap's artifacts, the take-home test's shape, the technical interview's shape, and reading job specs accurately | — | 5 | — | stub |

## Exit criteria
- [ ] You have a written production plan for `A050` with milestones, and you can point at what you cut and when.
- [ ] Your game builds from a clean checkout via one command, in CI, and produces a versioned artifact.
- [ ] A crash in a shipped build produces a symbolicated report you can read.
- [ ] You have run a triage pass and can defend the bugs you chose not to fix.
- [ ] Your game is publicly playable and at least one stranger has finished it.
- [ ] You have written a postmortem naming three things that went right and three that went wrong, specifically.
- [ ] Your portfolio presents Phase A–D artifacts as evidence of specific capabilities, not as a list of repositories.

## Traps specific to this topic
- **The last 10%.** It is 50% of the time. Plan for it explicitly in
  `A050` rather than discovering it in `A051`.
- **Scope creep disguised as polish.** New content is not polish. The cut
  list exists to make this distinction enforceable.
- **Shipping to nobody.** A game released with no store page, no trailer,
  and no announcement has not really been shipped, and you learn nothing
  from `S04`. The `A051` rubric requires the launch, not just the build.
- **A portfolio that is a GitHub profile.** Repositories are evidence, not
  a portfolio. `S05` covers the difference.

## Primary sources for the whole topic
- `[LENSES]` — the production and playtesting lenses
- `[GEA3]` ch. 2 (tools of the trade) and its production notes
- `[GDCVAULT]` — postmortem talks; the "Classic Game Postmortem" series, and indie postmortems with real sales numbers
- Steamworks documentation — https://partner.steamgames.com/doc/home — for `S02`/`S04`
- itch.io developer docs — https://itch.io/docs/creators/ — the lower-friction first release path
