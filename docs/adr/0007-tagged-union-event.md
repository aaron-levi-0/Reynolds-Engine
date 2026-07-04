# 0007 — Value-type tagged-union `Event`, replacing per-category shared objects

- Status: Accepted
- Date: 2026-07-04

## Context

ADR-0001 chose synchronous dispatch with a **shared `Event` object per category**, living in
separate `mouse_event.c` / `key_event.c` / `app_event.c` files. That was simple and
allocation-free, but it scattered the event vocabulary across many small files and — because each
category mutated one shared object — made **buffering impossible**: an event could not outlive the
moment of its dispatch. ADR-0001 already flagged a value-type tagged union as the migration path.

## Decision

Represent every event with a single **value-type tagged union** (`event_include.h`):

```c
typedef struct {
    EventType type;      // selects which union member is valid
    bool      handled;
    union {              // read only the member named by `type`
        struct { int code, repeat; }       key;
        struct { int button; }             button;
        struct { double x, y; }            mouse;
        struct { uint32_t width, height; } resize;
        struct { uint32_t x, y; }          moved;
    };
} Event;
```

Consolidate the `create*Event` factory functions into one `events/event.c`, and delete the
per-category `*_event.{c,h}` files and the shared objects they held.

## Consequences

- One self-contained `Event` value — the prerequisite for an event **queue/bus**: a buffered
  event no longer aliases a shared global, so it can safely outlive its dispatch.
- Read only the union member named by `type`; reading another member is undefined behaviour.
- Fewer files, and a single home for the event vocabulary.
- **Not yet done:** the factories still `malloc` each `Event` (a follow-up can build them on the
  stack to stay allocation-free), and dispatch is still **synchronous** — ADR-0001's core
  decision stands; only the event representation changed here.