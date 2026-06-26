# 0004 — Quarantine GLFW behind a thin platform boundary

[Status: Accepted; Date: 2026-06-26]

## Context

A stated project goal is to reduce dependence on GLFW (and later support more platforms or
windowing back-ends). For that to ever be cheap, GLFW must not be entangled throughout the
engine.

## Decision

Keep all GLFW usage inside a thin **platform boundary**: window/context/callbacks
(`core/window.c`), input polling (`core/input.c`), and timing (`core/timestep.c`). The event
system and the renderer stay GLFW-free. Code above the boundary speaks the engine's own
vocabulary (`Event`, and — to be added — engine key/button enums), never GLFW types or
constants.

## Consequences

- Swapping GLFW for another backend (SDL, Win32, custom) ideally means rewriting only the
  boundary files, not gameplay or rendering code.
- **Current leak:** `GLFW_KEY_*` constants are used above the boundary
  (`OrthoCameraController.c`, testbed input). Follow-up: define engine key codes and translate
  GLFW → engine in `window.c`, then have `Event` carry engine codes.
- **Cheap win available:** `renderer.c`, `buffer.c`, `texture.c`, and `shader.c` include
  `<GLFW/glfw3.h>` but make no `glfw*` calls — those includes can be removed (they only need
  GLEW for `gl*`).
- The verification is simple and repeatable: `grep -rn 'glfw[A-Z]' engine/src` should only ever
  hit the boundary files.