# Changelog

All notable changes to Reynolds-Engine are recorded here. The format is loosely based on
[Keep a Changelog](https://keepachangelog.com/); the project uses simple `0.x` version tags.

## [Unreleased]

### Notes
- A 2D AABB physics module (`physics.h` / `physics.c`) exists as a prototype patch; it is not
  yet merged into `main`.

## [0.2.4]

### Changed
- **Event system rebuilt on a value-type tagged union.** `Event` is now one struct — `type`, `handled`, and a `union` payload (key / button / mouse / resize / moved) in `event_include.h` — replacing the base-struct + per-category-object design. Dispatch stays synchronous (ADR-0001); the tagged union is the migration step toward buffering. See ADR-0007.

### Removed
- Per-category event files `app_event.{c,h}`, `key_event.{c,h}`, `mouse_event.{c,h}`; their `create*Event` factories are consolidated into `events/event.c` and the per-category shared event objects are gone.

## [0.2.3]

### Added
- `testbed/src/debug_layer.{c,h}` — debug layer extracted to its own files (FPS readout and
  draw-call / quad counters on keypress).

### Fixed
- Suppressed the `stb_image` `unused-but-set-variable` warning (scoped pragma in the vendor stub).

## [0.2.2]

### Added
- `get_log_level()` to query the active log level (e.g. from a debug layer).
- Testbed **debug layer**: FPS readout and draw-call / quad counters on keypress.
- `EngineInit` validates its arguments (non-NULL title, positive dimensions) and logs start-up.

### Removed
- Stray scratch file `a.md`.

## [0.2.1]

### Added
- **Audio module** `audio.{h,c}` — a thin wrapper over miniaudio (init/shutdown, one-shot SFX,
  looping music, master volume). See ADR-0005.
- `ARCHITECTURE.md`, `docs/adr/` (records 0001–0005), and this `CHANGELOG.md`.
- Audio tracks under `testbed/res/audio/`.

## [0.1.2]

### Added
- Vendored **miniaudio** (audio) and **stb_truetype** (text) into `engine/vendor` — dependencies
  staged, not yet wrapped in engine modules.
- Testbed **UI layer** (`testbed/src/ui_layer.c`) and supporting display changes.
- `progress.md` — per-system status board.

### Changed / Fixed
- Event-handling logic fixes and general error-handling improvements.
- Cached shader uniform lookups; reworked the `timestep` delta/cap logic; guarded the
  `subTexture` UV lookup against a missing registration.

### Known issues
- Camera zoom jitter on the menu / options screens of the Minesweeper testbed.

## [0.1.1]

### Changed
- Reworked window-close flow (deferred layer-stack destruction; added `EngineShutdown`).
- Moved `SHADER_PATH` into the `Renderer` handle; header/include reorganisation; added a
  `REN_STATIC` build option.

### Fixed
- Corrected an inverted layer-stack assertion, a layer-stack leak, and added empty-stack guards.

## [0.1.0] – [0.0.1]

Early iterations that established the engine foundation: window + GL context, the batch quad
renderer, sub-textures / sprite sheets, the orthographic camera and controller, the layer
stack, the event and input systems, shaders, levelled logging, the delta-time clock, custom
`vector` / `hash` utilities, and the Minesweeper testbed.