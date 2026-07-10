# Progress

Live status of each engine system. See [`ARCHITECTURE.md`](ARCHITECTURE.md) for how they fit
together and [`docs/adr/`](docs/adr/) for the decisions behind them.

**Legend:** ✅ Done · 🟡 In progress · 📦 Vendored (dependency present, not yet wired) ·
🧪 Prototype (exists, not in `main`) · ⬜ Not started

| System | Status | Notes |
|---|---|---|
| Window + GL context | ✅ | GLFW; clean callback wiring (`window.c`) |
| Batch quad renderer | ✅ | Texture slots, sub-textures, white-texture trick (`renderer.c`) |
| Sprite sheets (sub-textures) | ✅ | UV math in `subTexture.c` |
| Orthographic camera + pan/zoom | ✅ | `OrthoCameraController.c` |
| Layer stack | ✅ | `layers.c` (layers stored by value — ADR-0003) |
| Texture loading | ✅ | stb_image |
| Shaders + uniform cache | ✅ | Opaque `Shader` handles (`LoadShader`/`SetShader`, 0.2.5); per-shader uniform cache; DSA uniform uploads (`glProgramUniform*`); renderer binds its shader + uploads `u_ProjectionView` at `FlushBatch` — ready for a second shader (map). Known: `FreeShader` cache leak, no link-status check, varying sampler index (see CHANGELOG 0.2.5) |
| Time / delta | ✅ | `timestep.c` |
| Logging | ✅ | `logging.c` (levels, colour, `get_log_level()`) |
| Event system + input | 🟡 | `Event` is now a value-type **tagged union** (ADR-0007); per-category shared objects removed. Layers still **poll** input (ADR-0006); `handled` consumption is input-only (ADR-0002). Buffering / event queue still to come |
| Text / font rendering | 📦 | stb_truetype vendored; no engine wrapper yet |
| Audio | 🟡 | miniaudio wrapped in `audio.{h,c}` — init/shutdown, SFX one-shots, looping music, master volume (ADR-0005) |
| Physics | 🧪  | 2D AABB rigid-body module exists as a patch; not merged to `main` |
| UI | 🟡 | Testbed UI layer (`testbed/src/ui_layer.c`); no engine-level UI yet |
| Debug | 🟡 | Testbed debug layer (FPS / draw-call counters on keypress, via polling) |
| Scene / entity management | ⬜ | Each game wires layers by hand |
| Sprite animation | ⬜ | Have sub-textures, but no frame timeline on top |
| Input action mapping | ⬜ | Uses raw `GLFW_KEY_*` (boundary leak — ADR-0004) |
| Cross-platform build | ⬜ | `CMakeLists.txt` incomplete; real build is the Windows `.bat` scripts |

## Near-term focus

1. Wire **text** (stb_truetype) into engine modules now that the
   library is vendored.
2. Finish `handled` event consumption (input-only) and retire the coarse
   `disable_layer_event` workaround where it applies.
3. Introduce engine key codes and translate GLFW → engine in `window.c` (ADR-0004), so input
   stops naming `GLFW_KEY_*`.
4. Make `CMakeLists.txt` actually link GLFW/GLEW/cglm and build on Linux.