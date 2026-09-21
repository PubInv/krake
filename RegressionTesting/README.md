# Krake Regression Testing

Master reference hub for Krake regression testing documentation. Start here, then click through to the subsystem you need.

This is a living set of docs, built incrementally following an agile approach (small iterations, reviewed and adjusted sprint by sprint, backlog/scope decisions made before any test itself is written).

## Sections

| Subsystem | Status | Docs |
| --- | --- | --- |
| Firmware | Scope in progress | [Firmware/README.md](Firmware/README.md) |
| Hardware | Not started | — |

More sections get added here as we pick up each subsystem. Nothing beyond Firmware is in scope yet.

## How this hub works

- This file is the master doc — it only ever holds a short index and links out.
- Each subsystem gets its own folder with a local `README.md` acting as that subsystem's index, plus supporting docs (scope, test plan, test cases, results) added as they're written.
- Detail belongs in the subsystem docs, not here — keep this page short so it stays useful as a jumping-off point.

## Current focus

Firmware regression testing, complete firmware coverage. See [Firmware/README.md](Firmware/README.md) — currently working through **scope** (what's in/out, one-time vs. repeated tests, relationship to the existing factory test) before any test content is written.
