# T03.S02 — Input
**Topic:** [T03 — The Game Loop & Engine Core](README.md) | **Depends on:** T03.S01
**Status:** expanded | **Assignments:** A006 | **Est. theory time:** 1–2 sessions

> The question this subtopic answers: *how does a keypress become a game
> action, with the least latency and without the gameplay code knowing
> what a keyboard is?*
>
> Input is a small subsystem with an outsized effect on feel. It is also
> the place where an unnecessary frame of latency is easiest to add and
> hardest to notice.

---

## Theory

### U01 — Events vs polling, and why you want both
**What it is.** Two models for getting input from the OS, with different
guarantees.

**Why it exists.** The OS delivers input as a queue of timestamped events.
Games mostly want to ask "is this held right now?". Neither model answers
the other's question well.

**The mechanism.**

**Event-driven** — you drain a queue:
```cpp
SDL_Event e;
while (SDL_PollEvent(&e)) { /* key down, key up, mouse motion, ... */ }
```
- Never misses a press, even one shorter than a frame.
- Gives you ordering and, on some platforms, sub-frame timestamps.
- Awkward for "is W held" — you must reconstruct that state yourself.

**Polling** — you ask for current state:
```cpp
const bool* keys = SDL_GetKeyboardState(nullptr);
if (keys[SDL_SCANCODE_W]) { /* ... */ }
```
- Natural for continuous input (movement, aiming).
- **Misses short presses.** A 5 ms tap between two 16 ms polls is gone.
  This matters: fighting-game and rhythm-game inputs are routinely
  shorter than a frame, and so are fast mouse clicks.

**The answer: drain the event queue, and build a polled state from it.**
This is what every engine does, and the structure is:

```cpp
struct InputState {
    std::bitset<N> down;        // held this frame
    std::bitset<N> pressed;     // went down this frame (edge)
    std::bitset<N> released;    // went up this frame (edge)
    vec2  mouse_pos, mouse_delta;
    float axes[MAX_AXES];
};
// each frame:
prev = curr;
curr.clear_edges();
drain_os_events_into(curr);     // sets `down`, records transitions
curr.pressed  = curr.down & ~prev.down;
curr.released = ~curr.down & prev.down;
```

The `pressed`/`released` edges come from comparing against the previous
frame — that's the whole trick, and it's why you keep two frames of state.

**The sub-frame press problem.** If a key goes down *and* up within one
frame, the naive edge computation shows nothing (`down` is false at both
sample points). Fix: track a separate "was pressed at any point this
frame" bit set directly by the event handler, not derived from the state
comparison. This matters more than it sounds — it is the difference
between a responsive game and one that "eats inputs".

**Where it bites.** Using only polling, and then debugging why quick taps
sometimes do nothing. It is intermittent, input-dependent, and looks like
a hardware problem.

**Read.**
- `[SDLDOCS]` — SDL3 events (`SDL_PollEvent`) and keyboard state. Read what SDL guarantees about event ordering and coalescing.
- `[GEA3]` ch. 8.7 — the human interface devices chapter, in full. It is short and it is the best treatment of this subsystem in print.

**Check yourself.**
1. Give a concrete case where polling misses an input that events catch.
2. How are `pressed` and `released` derived, and what state does that require?
3. How do you handle a key that goes down and up within a single frame?

### U02 — The action layer
**What it is.** A mapping from physical inputs to named game actions, so
gameplay code says `Action::Jump` and never says `SDLK_SPACE`.

**Why it exists.** Four things all become trivial once this layer exists,
and all are painful to retrofit:
- **Rebinding** — a baseline accessibility requirement, not a feature.
- **Multiple devices** — keyboard, gamepad, and touch mapping to the same
  actions.
- **Contexts** — the same key means different things in menus, in gameplay,
  and while driving a vehicle.
- **Replays and AI** — anything that can produce an action stream can drive
  the game. This is what makes `A021` (replay) and `A047` (rollback)
  possible, and it is why this layer is not optional in this roadmap.

**The mechanism.**

```cpp
enum class Action { MoveX, MoveY, Jump, Fire, Interact, Pause, /*...*/ };

struct ActionState {
    float value;        // analog: [-1,1] for axes, [0,1] for triggers
    bool  pressed;      // digital edge
    bool  released;
    bool  down;
};

struct Binding {                    // one physical source -> one action
    Action action;
    Source source;                  // key / mouse button / pad button / axis
    float  scale;                   // -1 for the negative half of an axis
};
```

Design points that matter:

- **Everything is analog.** Treat a digital key as an axis that is 0 or 1.
  Then `MoveX` is one action fed by both `A`/`D` (scale −1/+1) and the
  stick's X axis, and gameplay reads one float. Without this you write the
  keyboard and gamepad paths twice.
- **Contexts are a stack.** Push "menu" on top of "gameplay"; the top
  context consumes what it handles and optionally blocks the rest. This is
  the same structure as `T06.S05`'s screen stack and they should agree.
- **Bindings are data** (`T06.S02`), loaded from a config file. That is the
  point of the layer.

**Where it bites.** Deciding this is over-engineering for Pong. It is
~150 lines, and `A006`'s spec requires it specifically because `A021`,
`A035`, and `A047` all need to feed synthetic actions into the game, and
retrofitting that means touching every gameplay call site.

**Read.**
- `[GEA3]` §8.7.4–8.7.6 — abstraction, context, and the HID system's design.
- `[GPP]` "Command" — https://gameprogrammingpatterns.com/command.html — the pattern this layer is an instance of, and it explains the replay/undo payoff clearly.

**Check yourself.**
1. Name four capabilities the action layer enables that direct key checks don't.
2. Why represent digital buttons as analog axes?
3. How do you handle the same physical key meaning different things in a menu vs gameplay?

### U03 — Input and the fixed timestep
**What it is.** The specific interaction between `T03.S01`'s loop and
input sampling.

**Why it exists.** Because the loop samples input once per *render* frame
but may simulate zero to many times, and getting this wrong causes
dropped or doubled inputs.

**The mechanism.** The rule from `T03.S01.U02`: **sample once per render
frame; feed the same sampled state to every simulation step in that
frame.**

```cpp
poll_input();                       // once — builds this frame's ActionState
while (accumulator >= FIXED_DT) {
    simulate(FIXED_DT, actions);    // same `actions` each iteration
    accumulator -= FIXED_DT;
}
```

Two consequences to handle explicitly:

1. **Edges must not fire twice.** If `actions.pressed[Jump]` is true and
   the frame runs two simulation steps, the naive code jumps twice. The
   simulation must consume the edge: either the sim clears it after the
   first step, or (better) the sim reads a per-tick input snapshot where
   the edge is only set on the first tick of the frame.
2. **Zero-step frames drop edges.** At 144 Hz render / 60 Hz sim, most
   frames run no simulation step. A press sampled during such a frame must
   survive until the next simulation step or it is lost entirely — which
   is the "the game ate my input" bug in its most common form.

The robust solution to both: **latch edges into a pending buffer that the
simulation drains.**
```cpp
pending.pressed |= this_frame.pressed;     // accumulate across render frames
while (accumulator >= FIXED_DT) {
    simulate(FIXED_DT, make_tick_input(pending, held_state));
    pending.pressed.reset();               // consumed by the first step only
    accumulator -= FIXED_DT;
}
```

**Input buffering as a feel technique.** Beyond correctness, deliberately
holding an input for a few ticks makes games feel better:
- **Jump buffering**: a jump pressed up to ~100 ms before landing still
  fires on landing. Removes the "I pressed jump and nothing happened"
  experience entirely.
- **Coyote time**: a jump remains valid for ~100 ms *after* walking off a
  ledge.
- **Attack buffering / input queuing**: the next attack in a combo can be
  queued during the current animation.

These are `T05.S05` and `T07.S01` material, but they live structurally in
the input layer, so build the buffer here. Celeste's jump buffer and coyote
time are both ~5 frames and are a large part of why it feels the way it
does — see `[CELESTE]`.

**Where it bites.** The zero-step frame case is invisible at 60 Hz render
/ 60 Hz sim (where every frame steps once) and appears when someone plays
on a 144 Hz monitor. Test at mismatched rates.

**Read.**
- `[GAFFER]` "Fix Your Timestep!" — and think about where input fits into its loop; the article doesn't address it and that gap is the content of this unit.
- `[CELESTE]` — https://maddythorson.medium.com/celeste-and-towerfall-physics-d24bd2ae0fc5 — jump buffering and coyote time from the person who tuned them.

**Check yourself.**
1. Why does a press sometimes get lost at 144 Hz render / 60 Hz sim, and what is the fix?
2. Why does a naive `pressed` flag cause a double jump on a catch-up frame?
3. What are jump buffering and coyote time, and why do they belong in the input layer?

### U04 — Devices: gamepads, mice, and their specifics
**What it is.** The per-device details that generic input handling gets
wrong.

**Why it exists.** Each device has a physical reality that leaks through,
and ignoring it produces controls that feel bad in device-specific ways.

**The mechanism.**

**Analog sticks: dead zones.** A stick at rest does not read zero. Without
a dead zone, the character drifts. Three approaches, in increasing quality:

- **Axial** — zero each axis independently below a threshold. Simple, and
  it produces a cross-shaped dead zone, so pure diagonals are hard and the
  stick "snaps" to axes near the centre. Avoid.
- **Radial** — zero the whole vector if `length < threshold`. Correct shape,
  but the magnitude jumps from 0 to `threshold` as you cross the boundary.
- **Scaled radial** — remap the remaining range to [0,1]:
  ```cpp
  float len = length(stick);
  if (len < DEAD) return {0,0};
  return normalize(stick) * ((len - DEAD) / (1.0f - DEAD));
  ```
  Continuous, full range, no snapping. **This is the one to use.**

Also clamp `len` to 1 — sticks report slightly outside the unit circle at
the diagonals, and unclamped that means diagonal movement is ~1.4x faster
than cardinal, which players notice as "diagonals are faster".

**Response curves.** Linear stick-to-velocity is rarely what you want.
A cubic curve (`out = in³`, or a blend) gives fine control near centre
and full speed at the edge. For aiming, this is most of what "feels good"
means; it is `T07.S01` material and it belongs here mechanically.

**Mouse.** Two distinct uses that must not be confused:
- **Absolute position** — for UI. Comes from the OS, already accelerated
  and scaled by system settings.
- **Relative delta** — for camera look. Must use *raw* input
  (`SDL_SetWindowRelativeMouseMode`), which bypasses OS pointer
  acceleration. Using accelerated position deltas for camera look makes
  aim inconsistent, and it is the most common cause of "the mouse feels
  wrong in this game".

Do not apply your own acceleration to camera look unless it is a
toggleable option that defaults off. Players have muscle memory calibrated
to linear.

**Gamepad specifics.** Hot-plug (controllers connect and disconnect
mid-game — handle it, don't crash); multiple pads and player assignment;
button layout differences between Xbox/PlayStation/Switch layouts (the
same physical position has different names, and your UI prompts must
match the connected device); trigger axes that rest at different values on
different pads. SDL3's gamepad API abstracts most of this via its
controller database, which is a significant part of why it's worth using.

**Where it bites.** Axial dead zones and OS-accelerated mouse look. Both
produce a game that "feels off" in a way testers report vaguely and that
has a precise, cheap cause.

**Read.**
- Josh Sutphin, "Doing Thumbstick Dead Zones Right" — http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html — the three approaches with diagrams. Short and definitive.
- `[SDLDOCS]` — SDL3 gamepad API and relative mouse mode.
- `[GAMEFEEL]` ch. 5–7 — input response curves and their perceptual effect.

**Check yourself.**
1. Why is an axial dead zone wrong? What does the player experience?
2. Why must camera look use raw relative input rather than pointer position deltas?
3. What happens if you don't clamp stick magnitude to 1?

### U05 — Input latency: not adding frames
**What it is.** The parts of `T01.S02.U02`'s latency chain that are under
your control.

**Why it exists.** Because the self-inflicted frames are free to remove
and invisible in every metric.

**The mechanism.** The self-inflicted delays, in order of how often they
appear:

1. **Polling after updating.** If `simulate()` runs before `poll_input()`,
   every input is one frame late. Free to fix; happens surprisingly often
   when the loop is refactored.
2. **Buffering input for "smoothing".** Averaging mouse deltas over
   several frames to reduce jitter adds latency proportional to the window.
   Almost never worth it; if the mouse is jittery, the cause is usually a
   polling rate problem.
3. **Acting on input at the wrong phase.** Reading input in a system that
   runs late in the frame ordering, after the systems that consume its
   result, delays it by a frame.
4. **Rendering a frame behind.** If the renderer draws `previous_state`
   rather than the interpolated state (`T03.S01.U04`), you have added a
   full simulation tick.
5. **Deep present queues.** Driver-level; controllable via
   `SDL_GL_SetSwapInterval` and low-latency modes, but partly out of your
   hands.

**Measuring it.** The reliable method is a high-speed camera (240+ FPS
phone slow-mo works) pointed at the screen and the input device, counting
frames between the physical press and the visible response. This sounds
elaborate; it takes ten minutes and it is the only way to get a real
number, because everything else measures a subset of the chain.

A cheaper proxy for regression testing: log the tick at which an input was
consumed and the tick at which the resulting state was presented.

**Where it bites.** Nobody measures it, so it accumulates. Each frame
added is 16.7 ms, and three of them is the difference between "crisp" and
"floaty" — a difference players report as a feel problem with no
identifiable cause.

**Read.**
- `[GAMEFEEL]` ch. 3 — the perceptual thresholds: how much latency players detect, which is lower than most developers assume.
- `[T01.S02]` U02 — re-read the latency chain now that you're implementing part of it.

**Check yourself.**
1. List the self-inflicted sources of input latency in a game loop.
2. Why does polling after simulating cost exactly one frame?
3. How would you measure end-to-end input latency without special hardware?

---

## Implementation

| assignment | size | target rung | what it forces you to understand |
|---|:---:|:---:|---|
| [A006](../assignments/A006-fixed-timestep-pong.md) | M | R3 | The event→state→action pipeline, edge handling across variable step counts, and an input stream that can be recorded and replayed |

**Order and overlap.** `A006` implements `S01` and `S02` together. The
requirement that carries the most forward weight is that the simulation
takes its input as an explicit `TickInput` parameter rather than reading a
global — because that single interface decision is what makes `A021`
(replay), `A035` (AI-driven agents feeding the same action stream), and
`A047` (rollback replaying stored inputs) into features rather than
rewrites.

`A018` extends this with jump buffering and coyote time. `A040`'s editor
adds a context to the stack. `A045`/`A046` serialize `TickInput` over the
network — at which point its size in bits starts to matter, which is worth
knowing now (keep it small and quantizable).

**If you are short on time.** The event→polled-state conversion with
edges (U01) and the action layer (U02) are the core. Dead zones (U04)
matter the moment you connect a gamepad. U05's measurement can wait until
`A024`'s juice pass, where you will care a great deal.

---

## Discussion Log
<!-- Appended per study session. See ../CONVENTIONS.md §7. -->

## Open Questions

## Connections
- `T01.S02.U02` — the full latency chain this subtopic implements a section of.
- `T03.S01.U02` — the loop structure that constrains when input can be sampled.
- `T05.S05`, `T07.S01` — buffering and coyote time as feel techniques.
- `T06.S05` — the screen/context stack, which should share structure with the input context stack.
- `T14.S04` — where `TickInput` becomes a network packet.
