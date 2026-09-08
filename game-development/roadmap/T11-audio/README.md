# T11 — Audio
**Phase:** C | **Depends on:** T03 | **Borrows from:** T05 (raycasts for occlusion)
**Status:** stub | **Assignments:** A037–A038

## Why this topic, here
Audio is the most under-studied and highest-return-per-hour system in
game development. Perceived responsiveness, impact, and weight are
substantially carried by sound — mute a good action game and it becomes
noticeably worse to play, in a way that is hard to attribute.

It is small (4 subtopics) and it is placed in Phase C so it exists in
time for `A023` if you take it slightly early — which the `T07` header
recommends you do.

It is also an excellent systems-programming exercise: the audio callback
is a **hard real-time deadline on another thread** where blocking,
allocating, or locking is a bug. It teaches lock-free thinking earlier and
more concretely than `T13.S05` does.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | Digital Audio Fundamentals | PCM, sample rate and bit depth, Nyquist and aliasing, buffers and the callback model, latency vs underrun trade-off, resampling, and what you must never do on the audio thread | T03.S01 | 6 | A037 | stub |
| S02 | Mixing & the Audio Graph | Voices and voice stealing, gain in dB vs linear, panning laws, buses and submixes, DSP effects (filters, delay, compression), streaming vs fully-loaded assets | S01, T03.S03 | 6 | A037 | stub |
| S03 | Spatial Audio | Distance attenuation curves, stereo panning vs HRTF, doppler, occlusion vs obstruction, reverb zones and early reflections, and cheap approximations that work | S02, T05.S02 | 5 | A038 | stub |
| S04 | Adaptive & Interactive Music | Vertical layering vs horizontal re-sequencing, transitions on musical boundaries, stingers, parameter-driven mixing, and the middleware model (FMOD/Wwise) as a design | S02 | 5 | A038 | stub |

## Exit criteria
- [ ] You can explain the relationship between buffer size, latency, and underruns, and pick a buffer size with a stated justification.
- [ ] You can list what is forbidden inside an audio callback and explain each prohibition.
- [ ] You can implement a lock-free command queue from the game thread to the audio thread and explain its memory ordering.
- [ ] You can explain why gain must be smoothed and demonstrate the click you get without it.
- [ ] You can explain the difference between occlusion and obstruction and how each is filtered differently.
- [ ] You can implement vertical music layering with transitions quantized to a bar.
- [ ] You can describe what FMOD/Wwise provide that your mixer doesn't, and make a defensible build-vs-buy argument.

## Traps specific to this topic
- **Allocating or locking on the audio thread.** Produces intermittent
  clicks that you will chase for days and blame on your decoder.
- **Zipper noise.** Setting volume per-buffer instead of ramping per-sample.
  Extremely common, instantly audible once you know the word for it.
- **Ignoring the mix.** Twenty correctly-spatialized sounds all at 0 dB is
  mud. Buses and ducking are not advanced features; they are the baseline.
- **Treating audio as a shipping-week task.** It is a feel system. It
  belongs in the loop with `T07`.

## Primary sources for the whole topic
- `[GEA3]` ch. 14 — the audio chapter, for the runtime architecture view
- `[GAIMPL]` — Stevens & Raybould, for the implementation-design view
- `[SDLDOCS]` — SDL3 audio streams and the callback contract; this is what `A037` builds on
- `[WWISE]` — the free Wwise 101 course, as a model of how the industry structures adaptive audio
- `[CCIA]` ch. 5, 7 — memory ordering and lock-free queues, for the audio thread boundary
