# Daily Coding Practice

A self-contained daily DSA + concept-learning practice loop, driven by an AI
agent (Claude Code, or any AI agent that reads `AGENTS.md`) working directly
in this repo's files — no external app or database.

## How it works
- **Problems** come from three combined sources seeded in
  `context/problem-bank.md`: LeetCode's Top Interview 150, AtCoder's
  Educational DP Contest (A-Z), and CSES's Introductory Problems. Each
  daily session picks one at random from whatever's still `status: todo`
  across all three (or serves a spaced-repetition review first, if one's
  due) — see `context/state.md`'s `selection_mode`.
- **Concepts** rotate through seven categories (C++, JS/TS, DSA-theory,
  System-Design, OOP, OS, AI-Automation), seeded in
  `context/concept-bank.md`.
- Every session's outcome is logged to `context/progress.md`, per-problem
  write-ups go in `docs/<source>/`, and per-concept notes go in `concepts/`.
- `context/state.md` tracks streak, concept rotation position, and
  anything due for spaced-repetition review — it's the only "memory" this
  repo has.

## Starting a session
Say things like:
- "give me today's DSA problem" / "daily practice"
- "concept of the day" / "teach me something"
- "how am I doing" / "show progress" / "my streak"
- "weekly review"

Any AI agent working here should start by reading `AGENTS.md` — it routes
each request to the matching file in `agents/`.

## Tools
`tools/` has a couple of small, dependency-free TypeScript scripts
(`npm install` inside `tools/` first):
- `npm run next-up` — read-only preview of what the next session would serve.
- `npm run streak` — recomputes the current streak from `progress.md`.
