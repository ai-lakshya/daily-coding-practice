# A002 — Cross-Platform CMake + SDL3 Window Skeleton
**Topic:** T01.S03 | **Size:** S (2–4h) | **Target rung:** R2
**Status:** todo
**Code:** `game-development/code/A002-window-skeleton/`
**Reuses:** A001 (the histogram, as an on-screen overlay) | **Reused by:** A003, A005, A006, A011, and every assignment after

## Goal
A project skeleton you trust and will copy forty times: builds clean on
two platforms, has sanitizer and release presets, fetches a dependency,
and opens a window with a frame-time overlay. Afterwards, starting a new
assignment costs five minutes rather than an evening.

**Hard cap: four hours.** Toolchain work expands to fill available time
and produces the feeling of progress without any. When the cap is hit,
ship what you have and move to `A003`.

## Concepts exercised
| unit | first met in | why it recurs here |
|---|---|---|
| T01.S03.U01 | here | CMake targets, `PUBLIC`/`PRIVATE`, generator expressions |
| T01.S03.U02 | here | Debug / RelWithDebInfo / ASan configurations |
| T01.S03.U03 | here | Warnings-as-errors and sanitizers, wired from day one |
| T01.S03.U05 | here | The take-vs-write dependency policy, applied |
| T01.S04.U03 | A001 | The histogram becomes a live overlay |

## Prerequisites
`A001` complete — its histogram header gets copied in.

## Specification

1. **CMake project** (3.21+), C++20, `CMAKE_EXPORT_COMPILE_COMMANDS ON`.
   - A `core` static library target and a `game` executable target.
   - Warnings-as-errors attached to **your** targets only, via generator
     expressions covering GCC/Clang and MSVC.
   - `CMakePresets.json` with at least: `debug`, `relwithdebinfo`,
     `release`, `asan`.
2. **SDL3 via `FetchContent`**, pinned to a specific tag. The repo must
   build from a clean clone with no manual dependency steps.
3. **A window** that opens, is resizable, and closes on the window-close
   event and on Escape.
4. **A minimal loop**: poll events, clear to a colour, present. Vsync
   **off** by default (with a toggle), for the reason in `T01.S02.U03`.
5. **The `A001` histogram, live**: sample frame time every frame, and
   render a text overlay showing mean / p95 / p99 / max, updating ~4x per
   second (not every frame — it's unreadable). SDL3's debug text rendering
   or a simple bitmap font is fine; do not pull in a font library.
6. **An `ASSERT` / `VERIFY` macro pair** per `T01.S03.U03`, in `core`.
7. **A `README.md`** in the assignment directory with the exact build
   commands for Linux and Windows. Test the Linux ones by deleting your
   build directory and following your own instructions literally.

### Constraints
- No global `CMAKE_CXX_FLAGS`. Flags attach to targets.
- The `asan` preset must actually run cleanly — if SDL3 trips ASan, use a
  suppressions file and note it, don't disable the preset.
- Third-party targets must not be compiled with your `-Werror`.

### Explicitly out of scope
OpenGL (that's `A011` — use SDL3's 2D renderer here), audio, gamepad
input, a real game loop with fixed timestep (that's `A006`), ImGui.

## Acceptance criteria
- [ ] `git clone && cmake --preset relwithdebinfo && cmake --build --preset relwithdebinfo` works from scratch.
- [ ] The `asan` preset builds and runs the window with no ASan reports.
- [ ] Deliberately introduce an unused variable; the build fails. Remove it; it passes.
- [ ] The window opens, resizes without artifacts, and closes cleanly (no crash-on-exit, checked under ASan with leak detection on).
- [ ] The overlay shows plausible frame times that change when you toggle vsync.
- [ ] `compile_commands.json` exists and your editor's clangd resolves SDL3 headers.

## Deliverables
- `code/A002-window-skeleton/` — the skeleton, which you will copy forward
- `code/A002-window-skeleton/NOTES.md` — what took longest, and the one CMake concept you had to look up twice

## Design notes / hints
- Structure the skeleton so copying it is trivial: one `core/` directory
  that is genuinely reusable, one `game/` that is the throwaway per-
  assignment part. Resist putting anything assignment-specific in `core`.
- `FetchContent_MakeAvailable(SDL3)` with `SDL_SHARED OFF` / `SDL_STATIC ON`
  keeps deployment simple. Pin the tag; `main` will break you eventually.
- The overlay updating 4x/second means accumulating into the histogram
  every frame but *formatting the string* only on a timer. Formatting per
  frame allocates and costs more than the thing you're measuring.

## Cut list
1. The Windows build instructions (test them later).
2. The on-screen overlay — fall back to printing the report on exit.
3. The `release` preset (keep `debug`, `relwithdebinfo`, `asan`).
4. Resizability.

## Rubric

| rung | passes when |
|---|---|
| R0 | Builds clean under all presets, runs, acceptance criteria met |
| R1 | No leaks or ASan reports on a clean exit. Window resize during a frame doesn't crash. Closing via the window manager and via Escape both take the same shutdown path. |
| R2 | The CMake is *target-based* — no global flags, correct `PUBLIC`/`PRIVATE` on `core`'s include directories, third-party built without your warning flags. The `core`/`game` split is clean enough that `A003` can add a math library to `core` without touching `game`. |

### R4 extension
*(optional here; this assignment targets R2)* Add a GitHub Actions
workflow that builds all presets on `ubuntu-latest` and `windows-latest`
and runs the ASan preset. If it passes on Windows without you having a
Windows machine, your CMake is genuinely portable.

## Common pitfalls
- Setting `-Werror` globally and having SDL3's own warnings fail your build.
- Forgetting `SDL_Quit` / not destroying the window, then dismissing the
  ASan leak report because "it's just exit".
- Testing the build instructions by memory rather than by deleting the
  build directory and following them.
- Spending three hours on a font renderer for the overlay. Use SDL3's
  built-in debug text; this is not the assignment.

## Review Log
<!-- One dated block per rung. See ../CONVENTIONS.md §4. -->

## Recall schedule
| due | rung | done | result |
|---|---|---|---|
