# T14 — Networking & Multiplayer
**Phase:** D | **Depends on:** T05, T06 | **Borrows from:** T13.S05 (determinism under parallelism matters for `A047`)
**Status:** stub | **Assignments:** A045–A047

## Why this topic, here
Multiplayer is the hardest thing in this roadmap, and it is hard for a
reason that is worth stating precisely: **the network makes time
ambiguous.** Every other system in the engine can assume there is one
"now". Networked gameplay cannot, and every technique in this topic —
interpolation, prediction, reconciliation, lag compensation, rollback —
is a different bargain about whose "now" wins.

It is placed last in Phase D because it consumes everything before it:
you cannot retrofit determinism, you cannot replicate state you cannot
serialize, and you cannot prediction-correct a character controller you
do not fully control.

The strategic point: `S04` and `S05` are two *different families* of
answer (state sync vs deterministic lockstep), not a progression. Which
one a game needs is determined by its genre, and getting that choice
wrong is unrecoverable late.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | The Transport Layer for Games | Why UDP and not TCP (head-of-line blocking), packet structure, sequence numbers and acks, selective reliability, fragmentation and MTU, congestion and bandwidth estimation, and simulating loss/jitter locally | T06.S04 | 7 | A045 | stub |
| S02 | Architectures & Authority | Peer-to-peer vs client-server, listen servers vs dedicated, server authority and why it is non-negotiable, relays, NAT traversal and hole punching, and topology's effect on cheating | S01 | 5 | A046 | stub |
| S03 | State Replication | Snapshots vs event replication, delta compression against acked baselines, quantization and bit packing, relevance/interest management, priority accumulators, and computing an actual bandwidth budget | S02, T06.S04 | 7 | A046 | stub |
| S04 | Latency Hiding: Prediction & Reconciliation | Entity interpolation and the interpolation buffer, client-side prediction of the local player, server reconciliation and replaying pending inputs, error smoothing, and lag compensation (rewinding the server for hit detection) | S03, T05.S05 | 7 | A046 | stub |
| S05 | Deterministic Lockstep & Rollback | The determinism requirements (float, iteration order, RNG), input delay, lockstep's failure mode, rollback/GGPO's model, save-state cost, and desync detection and debugging | S04, T05.S04 | 6 | A047 | stub |
| S06 | Services, Cheating & Backends | Matchmaking and sessions, the trust model, what server authority does and does not prevent, wallhacks vs aimbots vs speedhacks, anti-cheat approaches and their honest limits, telemetry | S02 | 5 | — | stub |

## Exit criteria
- [ ] You can explain head-of-line blocking and why it disqualifies TCP for game state.
- [ ] Your protocol handles loss, reordering, and duplication, and you have tested it under a simulated 200 ms / 5% loss link.
- [ ] You can compute a bandwidth budget for N players at M Hz and show your quantization decisions in bits.
- [ ] You can explain the difference between interpolation (remote entities) and prediction (local player) and why they are not the same technique.
- [ ] You can walk through a reconciliation step: what the server sends, what the client replays, and what the player sees when it mispredicts.
- [ ] You can explain lag compensation's fairness trade-off — the "shot behind cover" problem — and defend a position on it.
- [ ] You can list four sources of nondeterminism in your own engine and say what `A047` did about each.
- [ ] You can pick between state sync and rollback for a stated game and defend it.

## Traps specific to this topic
- **Trusting the client.** Every "just send the position" design becomes a
  cheating problem. Authority decisions are architectural, not a later
  hardening pass.
- **Testing on localhost.** 0 ms and 0% loss hides every bug in the topic.
  `A045` requires a loss/latency simulator in the loop from day one.
- **Rollback without determinism.** Determinism is a whole-engine
  property. If `T05`'s solver iterates a hash map, `A047` cannot work.
- **Underestimating this topic's size.** `A046` and `A047` are both `L`
  and both usually run long. That is normal; log the slip and continue.

## Primary sources for the whole topic
- `[GAFFER]` — the *Networked Physics* and *Building a Game Network Protocol* series. This is the primary text for `S01`, `S03`–`S05`
- `[VALVENET]` — Source's networking model; the origin of lag compensation as practiced
- `[GGPO]` — rollback's reference implementation and its determinism requirements
- `[OVERWATCH-ECS]` — Ford's GDC talk, the second half is the best available public description of a modern netcode architecture
- `[GEA3]` ch. 16.9 — for the engine-integration view
- `[GDCVAULT]` — *Rocket League*'s and *For Honor*'s netcode talks, as two very different answers
