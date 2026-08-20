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
- **System design** is a separate self-study track: you read a topic from
  your own PDF first, then bring in what you understood. The agent writes
  its own independent explanation into `system-design/<sub-topic>/`, then
  you cross-question it, and the discussion gets appended to that same doc.
  Topics are living notes — a second session on a topic extends its doc
  rather than starting a new one. `system-design/INDEX.md` maps it all.
- Every session's outcome is logged to `context/progress.md`, per-problem
  write-ups go in `docs/<source>/`, and per-concept notes go in `concepts/`.
- `context/state.md` tracks streak and concept rotation position — it's
  the only cached "memory" this repo has. What's due for spaced-repetition
  review is *not* cached there; it's computed fresh each session directly
  from `next_review` dates in `problem-bank.md`, so it can't go stale.

## Starting a session
Say things like:
- "give me today's DSA problem" / "daily practice"
- "concept of the day" / "teach me something"
- "system design: <topic> — here's what I read today ..." (or `/system-design`)
- "how am I doing" / "show progress" / "my streak"
- "weekly review"

Any AI agent working here should start by reading `AGENTS.md` — it routes
each request to the matching file in `agents/`.

## Tools
`tools/` has a couple of small, dependency-free TypeScript scripts
(`npm install` inside `tools/` first):
- `npm run next-up` — read-only preview of what the next session would serve.
- `npm run streak` — recomputes the current streak from `progress.md`.
