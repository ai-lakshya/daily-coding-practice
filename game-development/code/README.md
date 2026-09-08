# Code

Your code lives here. One directory per assignment, named
`ANNN-<slug>/` — matching the slug in the assignment's design doc so the
review skill can find it.

```
code/
├── A001-frame-time-harness/
│   ├── CMakeLists.txt
│   ├── src/
│   └── NOTES.md          <- required; the review reads this
├── A002-window-skeleton/
│   ├── core/             <- the reusable half, copied forward
│   ├── game/             <- the per-assignment half
│   └── NOTES.md
└── ...
```

## The `NOTES.md` contract

Every assignment directory needs one, and the review at R2 and R3 reads
it. It is not documentation for anyone else — it is the record of what
you decided and what surprised you, and it is what the R5 recall check
(weeks later) is run against.

Minimum:
```markdown
# ANNN — <title>

## Decisions
The choices the design doc left to you, and why you made them that way.

## Measurements
Any numbers the rubric asked for. Machine, build config, methodology.

## What surprised me
The thing that didn't work the way you expected. This is the most
valuable section and it is the one people skip.

## Cut
What you dropped from the spec, and why.
```

## Carrying code forward

Assignments deliberately build on each other (`Reuses:` in each design
doc). Two workable strategies:

- **Copy forward.** Each assignment directory is self-contained; copy
  `core/` from the previous one and evolve it. Simple, and every
  assignment stays independently buildable and reviewable. **Start here.**
- **Shared `core/`.** One `code/core/` that all assignments link against.
  Less duplication, but a change made in `A017` can break `A008`'s build,
  and the history of what `core` looked like at each assignment is lost.

The copy-forward approach wastes disk and preserves a record of how your
engine looked at each stage, which is worth more than the disk. Switch to
a shared core somewhere around `A020` if the duplication starts to hurt.

## Build

Each assignment builds standalone:
```bash
cd code/ANNN-slug
cmake --preset relwithdebinfo
cmake --build --preset relwithdebinfo
```

`A002` establishes the preset set (`debug`, `relwithdebinfo`, `release`,
`asan`); copy its `CMakePresets.json` forward.

## Not in git

Build outputs (`build/`, `out/`), compiled binaries, and downloaded
dependencies should be gitignored. Source, `CMakeLists.txt`,
`CMakePresets.json`, and `NOTES.md` are committed.
