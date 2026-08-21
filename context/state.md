# State
- last_session_date: 2026-08-21
- streak_days: 1
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
