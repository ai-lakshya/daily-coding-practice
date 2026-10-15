# AGENTS.md

This repo drives a daily DSA + concept-learning practice loop. Any AI
agent working in this repo should:

1. Read context/state.md and context/profile.md FIRST, always.
2. Route the request to the matching file in agents/:
   - "daily practice" / "give me a problem" / "today's DSA"  -> agents/daily-dsa.md
   - "concept of the day" / "teach me something"             -> agents/daily-concept.md
   - "system design" / a topic the user just read up on      -> agents/system-design.md
   - "game dev" / a roadmap id (T02.S03, A006) / "I solved A0NN"
     / "what's next in game dev" / "expand T05"               -> agents/game-dev.md
   - "analyse" / "I couldn't solve <problem>" / "contest <slug>"
     / anything about coding-questions/                       -> agents/coding-questions.md
   - "how am I doing" / "show progress" / "my streak"        -> agents/log-progress.md
   - weekly review (auto if 7+ days since last reviews/*.md) -> agents/weekly-review.md

   Note: agents/game-dev.md owns the `game-development/` directory — a
   three-layer roadmap (topics -> subtopics -> theory + assignments) with
   its own cursor in `game-development/STATE.md` and its own conventions
   in `game-development/roadmap/CONVENTIONS.md`. It has two main modes:
   a *study* session on a roadmap node, and a *review* of an assignment
   the user coded in `game-development/code/`. Read
   `game-development/roadmap/CONVENTIONS.md` before acting on it.

   Note: agents/coding-questions.md owns the `coding-questions/` directory —
   questions the user solved on their own, outside any guided session, each
   paired with a written explanation. It is the mirror image of
   agents/daily-dsa.md: there the agent serves a problem and guides the
   solve, here the solve already happened and the agent writes the record
   afterwards. Never serve a problem from it. Read
   `coding-questions/CONVENTIONS.md` before acting on it.

   Note: agents/system-design.md is the *self-study* track — the user reads
   a topic from their own PDF first and brings it in. It is separate from
   the System-Design category inside agents/daily-concept.md's rotation,
   and it owns the system-design/ directory. When the two overlap, the
   system-design/ doc is the deeper record and concept-bank.md's sysd-*
   row should point at it.
3. Follow that file's steps exactly, including which context/ files to
   read and write back to. This repo has no memory outside its files —
   do not skip the write-back steps.
4. Never change context/profile.md's stated preferences without the user
   explicitly asking to change them.
5. CP comes first until Codeforces rating >= 1200 (context/cp-plan.md).
   Until then, coding-questions, system-design and daily-concept are
   PAUSED: if one is triggered, say so and point at context/cp-plan.md
   instead of running it. game-dev stays active (2 h/week). Never write
   solution code for a problem the user is attempting; give at most one
   hint, per the stuck protocol in cp-plan.md.

If you are Claude Code: .claude/skills/ exposes the same logic as
invokable Skills — they delegate to agents/, so either entry point works.
