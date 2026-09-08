# T10 — Game AI
**Phase:** C | **Depends on:** T02, T05, T06 | **Borrows from:** —
**Status:** stub | **Assignments:** A033–A036

## Why this topic, here
Game AI is not AI. It shares almost nothing with machine learning and
everything with **systems design under a real-time budget**: the goal is
not an agent that plays well, it is an agent that is *legible* — one whose
behaviour the player can read, predict, and outwit.

It comes after `T05` and `T06` because agents need something to move
through and something to be driven by, and it comes before `T12`–`T14`
because behaviour trees and pathfinding are among the best possible
motivating problems for tooling and performance work.

The single most important idea in this topic is `S05`: an AI that is
smarter is often a worse game.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | Movement & Steering | Seek, flee, arrive, pursue/evade, wander, obstacle avoidance, flocking, and local avoidance (RVO/ORCA) — plus how steering interacts with your character controller | T05.S01, T05.S05 | 6 | A033 | stub |
| S02 | Pathfinding | Grids and graphs, Dijkstra → A*, heuristics and admissibility, tie-breaking, JPS, navmesh generation and why it beats grids, funnel/string-pull smoothing, hierarchical and time-sliced pathfinding | S01, T02.S04 | 7 | A034 | stub |
| S03 | Decision Making | FSM and HFSM, behaviour trees (nodes, blackboards, and what "reactive" costs), utility AI, GOAP and HTN — with an honest comparison of when each is the right tool | T06.S01 | 7 | A035, A036 | stub |
| S04 | Perception, Knowledge & Memory | Sensors and stimuli, vision cones and line-of-sight, hearing, sensory memory and forgetting, blackboards, influence maps, and shared/squad knowledge | S03, T05.S02 | 6 | A035 | stub |
| S05 | AI for Feel, Not Intelligence | Telegraphing, reaction-time budgets, deliberate imperfection, director AI and dynamic difficulty, aggro/attack tokens, and why players hate AI that is actually good | S03, S04, T07.S01 | 5 | A035, A036 | stub |

## Exit criteria
- [ ] You can explain why arrive needs a slowing radius and what oscillation looks like without one.
- [ ] You can state A*'s admissibility condition and give a heuristic that breaks it, with the consequence.
- [ ] You can explain why octile distance beats Euclidean on an 8-connected grid, and why tie-breaking matters visually.
- [ ] You can explain what a navmesh buys over a grid in memory, path quality, and query cost.
- [ ] You can implement a behaviour tree with a blackboard and explain what a decorator's abort/interrupt semantics do to the tree's evaluation.
- [ ] You can compare your `A035` behaviour tree and `A036` utility agent on the same NPC and argue for one, on specific grounds.
- [ ] You can name three techniques for making an NPC *read* as intelligent that involve no additional decision-making capability at all.

## Traps specific to this topic
- **Behaviour trees as the answer to everything.** They are excellent for
  authored, inspectable behaviour and poor for anything needing genuine
  planning or smooth trade-offs. `A036` exists so you experience the
  difference rather than believe a blog post about it.
- **A* on the grid forever.** Grid A* is where everyone stops. Navmesh +
  funnel smoothing is where paths stop looking like a Roomba's.
- **Perfect information.** An agent that knows the player's exact position
  always is both easy and awful. Model the sensing.
- **No debug view.** Same rule as physics: if you cannot see the path, the
  vision cone, and the current BT node, you are not debugging, you are
  guessing.

## Primary sources for the whole topic
- `[AIFG]` — Millington, 3rd ed. The reference; ch. 3 (movement), 4 (pathfinding), 5 (decision making), 11 (execution management)
- `[GAMEAIPRO]` — free chapters, industry practice. Particularly the behaviour tree, utility, and navmesh chapters
- `[REDBLOB]` — "Introduction to A*" and "Implementation of A*". The best pathfinding explanations that exist
- `[GDCVAULT]` — the annual "AI Summit" talks; *Left 4 Dead*'s director AI, *Halo*'s and *F.E.A.R.*'s AI postmortems for `S05`
