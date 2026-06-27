# 0006 — Game/layer input via polling; the event vocabulary stays internal for now

- Status: Accepted
- Date: 2026-06-27

## Context

Layers can receive events through `onEvent(Event*)`, and engine-internal layers (the camera,
the renderer's resize handler) do exactly that. But the event vocabulary — the `EventType`
enum (`KeyPressed`, ...) and the full `struct Event` — lives in the private
`engine/src/events/event.h`, so game-side layers only see the forward declaration from
`event_include.h`. Worse, the current `struct Event` carries **no key payload** (key codes live
in a separate `KeyEvent`, with no public getter), so even if it were exposed a handler still
couldn't answer "which key?" — `e->key.code` does not exist.

A debug layer that wants "print stats when L is pressed" runs straight into this.

## Decision

Game-side layers read input by **polling** (`isKeyPressed`, already public), using a small latch
for edge-triggered ("just pressed") actions. The event types and `struct Event` stay
**engine-internal**. The event vocabulary will be made public only as part of the tagged-union
event redesign (ADR-0001), which replaces the function-pointer struct with a clean value type
carrying a typed payload (`e->type`, `e->key.code`).

## Consequences

- Unblocks debug / UI / gameplay layers today with **zero new public API**.
- Avoids exposing the messy, soon-to-be-replaced internal event struct as public API.
- Edge-triggered inputs need a manual latch until events are public; continuous inputs
  (movement, holding) suit polling regardless.
- When ADR-0001 lands this record is superseded: the new `Event` value type goes in a public
  `events.h` (via `engine.h`), and layers can handle events directly.