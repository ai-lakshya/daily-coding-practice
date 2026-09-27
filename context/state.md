# State
- last_session_date: 2026-09-27
- streak_days: 1
  <!-- 2026-09-27: previous session 2026-09-24, not consecutive, so the streak resets to 1. -->
  <!-- 2026-09-24: no session on 2026-09-23, so the streak resets to 1. -->
  <!-- 2026-09-22. This file was stale: it still read 2026-09-16 / 2 while
       INDEX.md records cq-005 and cq-007 documented on 2026-09-21. Taking
       2026-09-21 as the real previous session (not consecutive with 09-16,
       so it reset to 1), today is consecutive with it -> 2. Also note
       progress.md has no rows for cq-005 or cq-007; backfill is pending. -->
- selection_mode: random-combined
  <!-- daily-dsa picks uniformly at random from all status:todo rows across
       leetcode-150 + atcoder-dp + cses combined (no per-source rotation),
       so difficulty and topic vary randomly session to session. Spaced-
       repetition review still takes priority over a fresh random pick:
       any status:solved row in problem-bank.md whose next_review date is
       due (<= today) is served first, computed directly from
       problem-bank.md each session rather than cached here — this avoids
       a separate list ever falling out of sync with the actual due dates. -->
- category_rotation_order: [DSA-theory, C++, JS/TS, System-Design, OOP, OS, AI-Automation]
- category_cursor: 1
