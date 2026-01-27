# Multithreaded Task Scheduler Plan

## Goals
- Keep the public API and threading contracts unambiguous before implementation details drift.
- Track a clear milestone timeline and per-phase tasks (see `ROADMAP.md` and `TASKS.md`).
- Preserve a predictable directory layout as the project grows.

## Where to Look
- `ROADMAP.md` — milestone definitions and timeline windows.
- `TASKS.md` — per-milestone checklists and concrete deliverables.

## Current Focus
- Finalize the public API for `include/tp/blocking_queue.hpp`, including close/pop wake rules.
- Write the method-level contract notes in `TASKS.md` under milestone M0.
- Move on to worker-thread interactions once the M0 checklist is complete.

## Update Rules
- Keep `TASKS.md` as the source of truth for what is done vs. pending.
