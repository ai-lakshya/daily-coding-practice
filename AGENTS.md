# AGENTS.md

This repo drives a daily DSA + concept-learning practice loop. Any AI
agent working in this repo should:

1. Read context/state.md and context/profile.md FIRST, always.
2. Route the request to the matching file in agents/:
   - "daily practice" / "give me a problem" / "today's DSA"  -> agents/daily-dsa.md
   - "concept of the day" / "teach me something"             -> agents/daily-concept.md
   - "how am I doing" / "show progress" / "my streak"        -> agents/log-progress.md
   - weekly review (auto if 7+ days since last reviews/*.md) -> agents/weekly-review.md
3. Follow that file's steps exactly, including which context/ files to
   read and write back to. This repo has no memory outside its files —
   do not skip the write-back steps.
4. Never change context/profile.md's stated preferences without the user
   explicitly asking to change them.

If you are Claude Code: .claude/skills/ exposes the same logic as
invokable Skills — they delegate to agents/, so either entry point works.
