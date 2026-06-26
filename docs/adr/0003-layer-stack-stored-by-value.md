# 0003 — Layers are stored by value in the stack

- Status: Accepted
- Date: 2026-06-26

## Context

`LayerStack` is a `Vector` of `Layer`. Layers could be stored by pointer (caller owns the
object) or by value (the stack owns a copy).

## Decision

Store `Layer` structs **by value**. `push_layer` copies the layer into the stack's `Vector`.

## Consequences

- Simple ownership: the stack fully owns its layers and iterates them in contiguous memory.
- A layer **cannot** keep per-instance mutable state inside the `Layer` struct, because the
  live copy is the one in the stack while the handlers are plain functions. The codebase works
  around this with **module-static** state per layer translation unit (`gameplay_layer.c`,
  `ui_layer.c`, and the camera controller).
- Trade-off: each such layer is effectively a **singleton** (one set of module-statics), so you
  can't have two independent instances of the same layer type.
- Lift path: the `Layer` struct already carries a `void* data` user pointer; routing per-layer
  state through that (or through small heap allocations) would remove the singleton limitation
  without changing the by-value storage.