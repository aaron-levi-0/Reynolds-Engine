# Changelog

All notable changes to Reynolds-Engine are recorded here. The format is loosely based on
[Keep a Changelog](https://keepachangelog.com/); the project uses simple `0.x` version tags.

## [Unreleased]

### Notes
- A 2D AABB physics module (`physics.h` / `physics.c`) exists as a prototype patch; it is not
  yet merged into `main`.

## [0.2.5]

### Changed
- **Shader module rebuilt around opaque `Shader` handles.** `LoadShader(path)` returns a
  `struct Shader*` holding the GL program **and its own uniform-location cache** (the old
  file-scope global cache keyed by name alone would return wrong locations once a second
  shader existed). `setMat4` / `setIntArray` now take the shader explicitly and upload via
  DSA (`glProgramUniform*`), so uniforms can be set without touching bind state.
  `BindShader` skips redundant binds via a cached program ID.
- **Renderer no longer loads its own shader.** `SetShaderPath` → `SetShader(renderer,
  shader)`: the game loads the batch shader and sets the `u_textures` sampler array once at
  startup (`testbed.c`). The renderer keeps a CPU-side `view_projection` snapshot
  (`setViewProjection`), and `FlushBatch` binds the renderer's shader and uploads
  `u_ProjectionView` immediately before drawing — each flush re-asserts its own state, the
  prerequisite for multi-shader frames (map shader, screen-space UI pass). Resolves the
  `entry.c` "move setMat4 to render layer" TO-DO.
- **Fragment shader reduced to a single path**: `colour = v_colour * texture(...)`. The
  `index == 0` branch is gone; `DrawQuad` writes a white tint into every vertex, so flat
  colour = white texture × tint. (Per-quad tinting of textures is now one parameter away.)
- Renderer internals tidied: `unsigned int` → `uint32_t`; vertex writes factored into
  `setPosition` / `setTexture` / `setColour` helpers.

### Known issues
- `FreeShader` releases the uniform cache with a bare `free()` — leaks every entry and its
  `strdup`'d key. Should call `free_cache()` (`utils/hash.c`), which already exists.
- `CreateShader` still doesn't check `GL_LINK_STATUS`, and `LoadShader` doesn't guard a
  failed `parseFile` (NULL sources reach `glShaderSource`).
- The fragment shader indexes `u_textures[]` with a per-fragment (non-dynamically-uniform)
  value — GLSL only guarantees this for dynamically uniform indices; works on NVIDIA,
  undefined on AMD/Intel. A `switch` over the slot index is the portable fix.
  
## [0.2.4]

### Changed
- **Event system rebuilt on a value-type tagged union.** `Event` is now one struct — `type`, `handled`, and a `union` payload (key / button / mouse / resize / moved) in `event_include.h` — replacing the base-struct + per-category-object design. Dispatch stays synchronous (ADR-0001); the tagged union is the migration step toward buffering. See ADR-0007.

- **Keycodes**. Keycodes are changed from GLFW's to engine's. 

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