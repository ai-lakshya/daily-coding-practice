# T13 — Performance Engineering
**Phase:** D | **Depends on:** T03, T05, T08 | **Borrows from:** —
**Status:** stub | **Assignments:** A042–A044

## Why this topic, here
Performance is a first-class *feature* in games in a way it is not in
most software: a game that misses frame is a game that plays worse, and
there is no "it's fine, the request took 200 ms instead of 100 ms".

This topic is placed in Phase D, not Phase A, on purpose. Optimizing
before you have real systems and real workloads teaches you to optimize
imaginary problems. By now you have a renderer, a physics solver, and an
animation system that are genuinely slow in genuinely interesting ways —
which makes this the first point where the discipline can be practiced
honestly rather than performed.

The rule the whole topic enforces: **every claim needs a number.** `A042`,
`A043`, and `A044` all fail their rubric on an unmeasured assertion,
however plausible.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | The Machine Model | Cache hierarchy and line size, latency numbers you should know, prefetching, TLB, branch prediction, out-of-order execution, and how much of your intuition about "operation count" is wrong | — | 6 | A042 | stub |
| S02 | Profiling in Practice | Sampling vs instrumentation, building a frame profiler, flame graphs, `perf`/VTune/Tracy, GPU timers and frame capture (RenderDoc/Nsight), and profiling methodology: hypothesis before measurement | S01, T01.S04 | 6 | A042 | stub |
| S03 | Data-Oriented Design | AoS vs SoA in practice, hot/cold splitting, removing pointer chasing, batching by type, the existence-based-processing idea, and where DOD's claims are overstated | S01, T03.S04 | 6 | A043 | stub |
| S04 | SIMD | The model (SSE/AVX2/AVX-512, NEON), intrinsics vs autovectorization, alignment and layout preconditions, horizontal ops as a smell, and measuring whether it helped | S03 | 5 | A043 | stub |
| S05 | Multithreading & Job Systems | Task graphs and dependencies, work stealing, fibers vs threads, false sharing, atomics and memory ordering, lock-free queues, frame pipelining, and determinism under parallelism | S01, `[CCIA]` | 7 | A044 | stub |
| S06 | Memory & Budgets | Fragmentation, streaming and residency, per-system budgets and tracking, leak detection, and the console-style discipline of a fixed memory plan | T03.S03 | 5 | A044 | stub |

## Exit criteria
- [ ] You can state approximate cycle costs for an L1 hit, an L2 hit, a main-memory miss, and a mispredicted branch, and use them in an estimate.
- [ ] You have a frame profiler in your own engine with named scopes and a visual timeline.
- [ ] You found three real hotspots by measurement, and at least one of them was not where you guessed.
- [ ] You can present a before/after with a stated workload, a stated machine, and a variance estimate — not a single run.
- [ ] You can convert a struct-of-objects loop to SoA and report the actual speedup, including the case where it was ~zero.
- [ ] You can explain false sharing and demonstrate it with a measurement.
- [ ] Your job system runs physics broadphase and animation in parallel with a correct dependency graph and no data races under TSan.
- [ ] You can produce a memory budget table for your engine with real numbers per subsystem.

## Traps specific to this topic
- **Optimizing without profiling.** The most reliable way to spend a week
  making a program 0.4% faster. The rubric blocks it.
- **Benchmarking a debug build**, or with a different CPU governor, or
  once. All three produce numbers that are worse than none.
- **SIMD-ing the wrong loop.** SIMD requires the data layout from `S03`.
  Applied to AoS data it usually loses to the scalar version.
- **Threading everything.** Amdahl plus synchronization overhead means a
  badly-chosen parallelization is slower *and* nondeterministic. `A044`
  requires you to show the serial baseline it beat.
- **Losing determinism silently.** Parallel float accumulation reorders
  additions. If `A047` (rollback netcode) is in your future, this matters.

## Primary sources for the whole topic
- `[MIKEACTON]` — CppCon 2014. Watch it, then read the counterarguments; both are useful
- `[DOD]` — Fabian, free. The book-length version of `S03`
- `[GEA3]` ch. 4 (parallelism & concurrent programming), ch. 5.2 (memory) — for `S05`–`S06`
- `[CCIA]` — Williams. ch. 5 (memory model), 7 (lock-free), 8 (designing concurrent code) for `S05`
- `[AGNERFOG]` — the microarchitecture and optimizing-C++ manuals, as reference for `S01`/`S04`
- `[INTELINTRIN]` — the intrinsics guide, for `S04`
- `[NAUGHTYDOG-JOB]` — Gyrling's fiber-based job system talk, the canonical `S05` case study
