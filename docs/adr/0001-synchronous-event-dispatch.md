# 0001 — Synchronous, blocking event dispatch with shared event objects

- Status: Accepted
- Date: 2026-06-26

## Context

GLFW delivers input and window notifications via callbacks. The engine needs to route them to
interested layers. Two broad models: dispatch immediately (synchronous), or enqueue events and
drain them later (an event queue / bus).

## Decision

Dispatch **synchronously**. A GLFW callback builds an `Event` and immediately calls through
`entry.c:onEvent → handle_layer_events`, which is fully processed before the next event. Each
event *category* reuses a single shared `Event` object (`mouse_event.c`, `key_event.c`,
`app_event.c`).

## Consequences

- Simple, allocation-free, and easy to follow.
- Because an event is created and consumed before the next one exists, the shared object is
  safe **today**.
- It blocks an event **queue/bus**, is not reentrant, and the shared object would corrupt if
  events were ever buffered or produced from more than one source. `event.h` already notes the
  intent to buffer events in future.
- Migration path: a value-type **tagged-union** `Event` (one struct, `type` + a `union`
  payload) created on the stack. That removes the shared global and makes queuing trivial.
  Tracked as a future change, not yet done.