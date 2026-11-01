# Independent Game Implementation

This directory is reserved for a new, independently designed fighting-game implementation. It is intentionally separate from the evidence-based reconstruction in `../src/` and from the ROM/Ghidra workspace in `../working/`.

## Planned technology

- C++20 for the platform-neutral simulation and application code.
- CMake for cross-platform configuration and build generation.
- SDL3 behind platform-facing graphics, audio, input, and controller adapters.
- CTest with small dependency-free test executables for the deterministic core.

The local machine has GCC C++ 13.3 and CMake 3.28. Only the SDL2 runtime is installed; SDL development headers and SDL3 are absent. The current graphics path is a dependency-free software renderer and virtual window surface; no dependencies were installed.

## Structure

- `src/core/`: engine orchestration, fixed-step loop, state transitions, event/time primitives.
- `src/game/`: match-level coordination and rules.
- `src/player/`: player slots, commands, and runtime state.
- `src/character/`: independently specified character definitions and move data.
- `src/combat/`: hit resolution and combat rules.
- `src/graphics/`: RGBA framebuffer, layered software renderer, virtual viewport/window surface, PPM resource loader, and frame pacer. A native SDL3 window adapter remains pending.
- `src/animation/`: frame-duration clips, movement-state transitions, looping, and direct clip playback.
- `src/audio/`: PCM16 WAV resources, mixer, event/music routing, null output and WAV-file output. Live SDL3 device output remains pending.
- `src/input/`: normalized keyboard/gamepad/joystick events, per-player bindings, input buffering and motion-sequence recognition. Live SDL3 polling remains pending.
- `include/`: public interfaces shared across modules.
- `assets/`: new or properly licensed assets only. Original ROM files are analysis inputs and must not be copied here.
- `tests/`: deterministic tests for core rules and adapters.
- `build/`: local generated build output; do not commit generated files.

## Current Core

The headless core includes a fixed-step clock, session/state flow, two generic training players, movement, pause/restart, round bookkeeping, RGBA rendering, sprite resources, tick-based animation, PCM mixing, gameplay-event-driven audio and normalized two-player input. A basic new collision/combat path connects attack buttons to prototype hitboxes, health and round outcomes. Character statistics, hitboxes, damage, attack priority, art/audio placeholders, key maps and motion windows are new project decisions, not verified SF2 data. Block/knockback, a native window, live audio output and live SDL input polling remain unimplemented.

Build and run the core tests from the workspace root:

```sh
cmake -S implementation -B implementation/build -DCMAKE_BUILD_TYPE=Debug
cmake --build implementation/build --parallel 2
ctest --test-dir implementation/build --output-on-failure
```

Generator-neutral Debug/Release presets are available in `implementation/CMakePresets.json`.

The core, software renderer, audio mixer and normalized input manager have no SDL dependency. A frontend must translate SDL3 keyboard/gamepad/joystick events to `InputEvent`, dispatch `FrameInput` once per fixed simulation step, present the output surface in a native SDL3 window, and connect a live audio adapter. The audio core can write mixed PCM16 output to WAV files.

See the final Phase 20 report set in `../reports/FINAL_PROJECT_REPORT.md` and the reconstruction, implementation, test, architecture, build, limitations, and remaining-work reports for the complete documented state.
