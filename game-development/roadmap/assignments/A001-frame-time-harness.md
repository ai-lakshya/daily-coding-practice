# A001 — Frame-Time Harness & Percentile Reporting
**Topic:** T01.S04 (also T01.S02) | **Size:** XS (≤1h) | **Target rung:** R2
**Status:** todo
**Code:** `game-development/code/A001-frame-time-harness/`
**Reuses:** — | **Reused by:** A002, A006, A007, A016, A030, A042

## Goal
Be able to take a timing measurement that is actually true, and report it
in a form that describes what a player would feel. Afterwards you should
never again describe performance with a single average number, and you
should have a reusable histogram you drop into every later project.

## Concepts exercised
| unit | first met in | why it recurs here |
|---|---|---|
| T01.S02.U04 | here | Averages lie — this is where you prove it to yourself |
| T01.S04.U01 | here | `steady_clock` vs the alternatives |
| T01.S04.U02 | here | Timer overhead, warm-up, DCE, run-to-run variance |
| T01.S04.U03 | here | Bucketed histograms and nearest-rank percentiles |

## Prerequisites
A C++20 compiler. No window, no SDL, no graphics. This is a console
program.

## Specification

1. A `Timer` that wraps `std::chrono::steady_clock` and returns elapsed
   seconds as `double`.
2. A `Histogram` with **fixed-width buckets**, no allocation after
   construction:
   - constructed with `(bucket_width_ms, bucket_count)`,
   - `add(double ms)` is O(1) and clamps into an overflow bucket,
   - `percentile(double p)` using **nearest-rank** (state this in a
     comment; the method matters for comparability),
   - `mean()`, `max()`, `count()`.
3. A `report()` that prints, in this shape:
   ```
   samples: N   config: <build config>   machine: <cpu>
     mean   X.XX ms
     p50    X.XX ms
     p95    X.XX ms
     p99    X.XX ms
     p99.9  X.XX ms
     max    X.XX ms
     1% low XX.X FPS
   <ascii histogram>
   ```
4. A **synthetic workload** driver: a loop that simulates frames by
   sleeping/spinning for a distribution you control — mostly ~10 ms, with
   a configurable spike rate (e.g. 1 in 300 frames takes 45 ms).
5. **Measure and report the timer's own overhead**: time N empty
   `now()`-pairs, report the per-call cost, and state your histogram's
   resolution floor.
6. A comparison mode that prints, for the same run, the naive
   "average FPS" number *next to* the percentile report — so the gap is
   visible on one screen.

### Constraints
- No allocation inside the sampling loop. The histogram is preallocated.
- No `high_resolution_clock`. No `system_clock` for durations.
- Must build with `-Wall -Wextra -Wpedantic -Werror`.
- Report must state the build config; refuse to run (or print a loud
  warning) if built at `-O0`.

### Explicitly out of scope
Graphics, a real game loop, threading, a GUI. Those are `A002`/`A006`.

## Acceptance criteria
- [ ] Runs 3600 synthetic frames and prints the full report.
- [ ] With a 1-in-300 45 ms spike injected, the mean barely moves and
      p99.9 and max clearly show it — and you can point at the two numbers
      and say which one a player feels.
- [ ] Timer overhead is measured and printed, not assumed.
- [ ] Zero allocations during sampling (verify: run under `ltrace`/ASan or
      instrument a global `operator new` counter and assert it doesn't
      change during the loop).
- [ ] Percentiles agree with a brute-force sort of the same samples to
      within one bucket width (write this as a test).

## Deliverables
- `code/A001-frame-time-harness/` — source + `CMakeLists.txt`
- `code/A001-frame-time-harness/NOTES.md` — your measured timer overhead,
  your machine, and one paragraph on what surprised you

## Design notes / hints
- Nearest-rank percentile on a bucketed histogram: walk buckets
  accumulating counts until you pass `p/100 * n`; report that bucket's
  upper edge. Your accuracy is one bucket width — which is why the
  bucket width is a parameter and why you state it in the report.
- The "1% low FPS" convention: take the slowest 1% of frames, average
  their frame *times*, then convert that to FPS. Note that this is not
  the same as `1/p99`, and think about why.
- To spin without sleeping (sleep has millisecond-scale granularity and
  its own jitter), busy-wait on the clock. That is fine here; it is a
  deliberately synthetic workload.

## Cut list
1. The ASCII histogram rendering (keep the numbers).
2. The comparison mode — but then you lose the main "aha".
3. The overflow-bucket handling (assert instead of clamping).

## Rubric

| rung | passes when |
|---|---|
| R0 | Builds clean with `-Werror`, runs, all acceptance criteria demonstrably met |
| R1 | Percentiles correct at the boundaries: p0, p100, a single sample, all samples in one bucket, and samples above the last bucket. No UB under UBSan. |
| R2 | The histogram is a **reusable component** — a header you can drop into `A002` without editing. No `printf` inside the data structure; reporting is separate from collection. Bucket width and count are parameters, not constants. |
| R3 | *(not targeted, but if you want it)* Sampling costs less than 100 ns per frame, measured. |
| R4 | See extension below |

### R4 extension
Add a second histogram implementation using **logarithmic buckets** (each
bucket covers a constant *ratio* rather than a constant width, as
HdrHistogram does). Compare accuracy at p50 and p99.9 against the linear
version for the same sample set, at the same total bucket count. Write
down which you would ship and why.

## Common pitfalls
- Timing with `system_clock` and getting a negative duration when NTP steps.
- Storing all samples in a `std::vector` that reallocates mid-loop —
  which allocates, in the loop you are measuring.
- Computing percentiles by sorting every frame instead of at report time.
- Building at `-O0` and reporting the numbers as if they meant something.
- Reporting FPS averages "for comparison" and then quietly believing them.

## Review Log
<!-- One dated block per rung. See ../CONVENTIONS.md §4. -->

## Recall schedule
| due | rung | done | result |
|---|---|---|---|
