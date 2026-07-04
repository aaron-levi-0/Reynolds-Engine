# Architecture Decision Records

Short notes capturing *why* a non-obvious decision was made, so future-me doesn't have to reverse-engineer intent from the code.

## What is an ADR?

A small, mostly-immutable record. Once **Accepted** it isn't rewritten; if the decision
later changes, add a *new* ADR that supersedes the old one (and update the old one's status).
Each record answers three things: the **context** (forces at play), the **decision**, and the
**consequences** (what gets easier or harder).

## Index

| # | Title | Status |
|---|-------|--------|
| [0001](0001-synchronous-event-dispatch.md) | Synchronous, blocking event dispatch with shared event objects | Accepted |
| [0002](0002-event-consumption-input-only.md) | `handled` consumption applies to input events, not broadcasts | Accepted (representation superseded by 0007) |
| [0003](0003-layer-stack-stored-by-value.md) | Layers are stored by value in the stack | Accepted |
| [0004](0004-glfw-platform-boundary.md) | Quarantine GLFW behind a thin platform boundary | Accepted |
| [0005](0005-audio-via-miniaudio.md) | Audio via a thin wrapper over miniaudio's high-level engine | Accepted |
| [0006](0006-input-polling-events-internal.md) | Game/layer input via polling; event vocabulary stays internal for now | Accepted |
| [0007](0007-tagged-union-event.md) | Value-type tagged-union `Event`, replacing per-category shared objects | Accepted |

## Template

```markdown
# NNNN — Title

- Status: Proposed | Accepted | Superseded by ADR-XXXX
- Date: YYYY-MM-DD

## Context
What problem am I solving? What forces / constraints are at play?

## Decision
What I decided to do.

## Consequences
What becomes easier or harder; trade-offs and follow-ups.
```