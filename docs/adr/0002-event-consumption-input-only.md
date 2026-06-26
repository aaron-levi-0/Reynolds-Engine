# 0002 — `handled` consumption applies to input events, not broadcasts

- Status: Accepted
- Date: 2026-06-26

## Context

The layer stack dispatches events top → bottom, and a layer can set `e->handled` to stop
propagation to lower layers. But events fall into two kinds:

- **Input** (key, mouse button, scroll): usually exactly one layer should act — e.g. a UI
  panel eats a click so the game board behind it doesn't also receive it. Consumption is the
  whole point.
- **Broadcast / notification** (window resize, move, close): several systems independently
  need the *same* event — e.g. the renderer updates the GL viewport **and** the camera updates
  its projection on resize. Consuming it for one starves the others.

This is bug-prone: consuming a resize at a high layer would silently stop the renderer below
from ever updating its viewport.

## Decision

`handled` semantics apply to **input** events only. Broadcast/application events always fan out
to every interested layer and must **not** be consumed.

This is a **convention** today. It does not require splitting into two event systems — one
event type and one dispatch path are fine. It can be *enforced* cheaply later using the
existing `EventCategory` flags, by only short-circuiting for input-category events:

```c
layer->onEvent(e);
if (e->handled && isInCategory(e, EventCategoryInput)) break;
```

## Consequences

- Prevents a class of "only one system reacted to the resize" bugs.
- Keeps the design simple: it is primarily discipline, not structure.
- Enforcement via category flags is optional and low-cost, since the categories already exist.
- Practical rule of thumb: set `handled` in input handlers that "win" contention (UI click,
  camera scroll); never in window/app handlers.