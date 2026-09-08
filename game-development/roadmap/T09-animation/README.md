# T09 — Animation Systems
**Phase:** C | **Depends on:** T02, T08 | **Borrows from:** —
**Status:** stub | **Assignments:** A030–A032

## Why this topic, here
Animation is where `T02.S03` (quaternions) stops being an exercise and
becomes load-bearing, and it is the system players read most directly:
a character with good animation and mediocre AI reads as smarter than the
reverse.

It also has an unusual property — it is the subsystem most tightly
coupled to *both* rendering and gameplay simultaneously. That makes it an
excellent test of whether your `T03` core and your `T06` gameplay layer
were designed well. If integrating animation is painful, the pain is
usually diagnosing something upstream.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | Skeletons, Poses & Skinning | Joint hierarchies, local vs model vs bind space, the inverse bind matrix, linear blend skinning and the candy-wrapper artifact, dual quaternion skinning, CPU vs GPU skinning | T02.S03, T08.S02 | 6 | A030 | stub |
| S02 | Clips, Sampling & Compression | Keyframes and interpolation, sampling at a fixed rate vs continuous time, curve fitting, quantization, memory budgets for large clip libraries | S01, T02.S05 | 5 | A030 | stub |
| S03 | Blending, State Machines & Blend Trees | Crossfades, 1D/2D blend spaces, additive and layered animation, bone masks, transitions and sync points, root motion vs in-place | S02, T06.S01 | 6 | A031 | stub |
| S04 | Procedural Animation & IK | Two-bone analytic IK, CCD and FABRIK, foot placement on uneven ground, look-at with constraints, ragdoll and blend-to-ragdoll | S01, T05.S05 | 6 | A032 | stub |
| S05 | Animation-Driven Gameplay | Animation events and notifies, hit reactions and hitboxes from animation, motion matching (an overview), authority conflicts between animation and physics | S03, S04 | 5 | A031 | stub |

## Exit criteria
- [ ] You can explain the full transform chain from a clip's keyframe to a vertex's final position, naming every space it passes through.
- [ ] You can explain what the inverse bind matrix is and what breaks if you omit it.
- [ ] You can show the candy-wrapper artifact and explain why LBS produces it.
- [ ] You can state the memory cost of one second of animation for a 60-joint skeleton, and reduce it with a stated technique.
- [ ] You can implement a crossfade that does not pop, and explain what "does not pop" requires of the quaternions involved.
- [ ] You can explain root motion's trade-off against a code-driven controller in terms of who owns the character's velocity.
- [ ] Your two-bone IK handles the unreachable-target case without snapping.

## Traps specific to this topic
- **Lerping quaternions without checking the sign.** `q` and `-q` are the
  same rotation; blend the wrong pair and the character takes the long way
  around. It will look like a rigging bug and it is not.
- **Skinning in the wrong space.** The most common cause of "my character
  explodes into infinity". Debug by rendering the skeleton first — always.
- **Letting animation and physics both own the transform.** Decide who is
  authoritative per state and write it down; conflicts here produce bugs
  that appear only in transitions.
- **Building a blend-tree editor before you have two clips.** `A031` says
  state machine and blend tree in code first.

## Primary sources for the whole topic
- `[GEA3]` ch. 12 — the best single treatment of the runtime animation pipeline in print
- `[3DMP]` / `[FGED1]` — return to the quaternion chapters, now with a reason to care
- `[GLTF]` — the skinning and animation sections of the spec; you will implement against it in `A030`
- `[GDCVAULT]` — "Animation Bootcamp" sessions, particularly on motion matching and blending
- `[RTR4]` §4.6 (vertex blending), for the GPU-side view
