# 0005 — Audio via a thin wrapper over miniaudio's high-level engine

- Status: Accepted
- Date: 2026-06-26

## Context

The engine needs audio. **miniaudio** is already vendored — single-header, no external
dependencies, cross-platform — which matches the project's stb-style taste in libraries. It
offers a low-level device/callback API and a high-level `ma_engine` API (sound effects, music,
groups, spatialization).

## Decision

Expose a small `audio.{h,c}` module wrapping miniaudio's **high-level `ma_engine`**, not the
low-level device API. The surface is intentionally tiny: init/shutdown, fire-and-forget
one-shots (`ma_engine_play_sound`), a single looping music track (`ma_sound` + streaming), and
master volume. A single static engine instance mirrors the engine's other singleton subsystems.
Every call is a no-op (with a log) when uninitialised, so callers don't guard each one.

## Consequences

- Minimal code, and it gets sound working immediately for a 2D game (SFX + music).
- The single-engine, single-music-slot, by-path design is deliberately limited. Managed sound
  handles (load once / play many), sound groups, and spatial audio all exist in miniaudio and
  can be added behind the same module later without changing callers.
- Streaming music (`MA_SOUND_FLAG_STREAM`) avoids decoding whole tracks into memory; short SFX
  use the fire-and-forget path, which decodes on demand.
- The miniaudio implementation TU is already vendored (`engine/vendor/miniaudio.c`). On Linux
  it links against `-lpthread -lm -ldl`; the Windows `.bat` build is unaffected.