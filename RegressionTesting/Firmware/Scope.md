# Firmware Regression Testing — Scope

Status: **draft, most structural questions resolved from the code; a few process calls still open (see §6)**. This doc defines *what* we will test and *how tests are classified* — not the test cases themselves.

## 1. Purpose

Catch regressions in existing, already-working firmware behavior as the codebase changes — as opposed to verifying a freshly-manufactured unit is built correctly (that's factory test, see §3). Complete firmware coverage: everything shipped in [GPAD_API](../../Firmware/GPAD_API), not just one module.

**Ground truth for "expected behavior":** what the code currently does, not what a README/manual/spec says it does. The firmware may get rewritten for efficiency later, and some current behavior may even be a known rough edge (e.g. the MQTT broker credentials are hardcoded in [GPAD_API.ino](../../Firmware/GPAD_API/GPAD_API/GPAD_API.ino) rather than authenticated per-device — already tracked as its own effort elsewhere in the repo, not a regression-test concern). Regression tests exist to catch *unintended* behavior changes, not to enforce a design the code doesn't currently implement. When code and docs disagree, the code wins as the baseline.

## 2. Test categories in scope

Read through [GPAD_API.ino](../../Firmware/GPAD_API/GPAD_API/GPAD_API.ino) and the module headers to ground this in what's actually built, rather than guess. The firmware splits cleanly into three groups by *what it takes to run the test* — which turns out to be the more useful axis than "functional vs. HIL" alone, because it's what determines whether a test can be automated:

- **Host-testable (no hardware needed).** Logic that's already decoupled from hardware behind `Stream`/`Print` abstractions, so it can run on a dev machine. Confirmed precedent already exists: [test/native/test_GPAP](../../Firmware/GPAD_API/test/native/test_GPAP/test_gpap_message.cpp) unit-tests GPAP alarm message parsing on PlatformIO's `native` platform using Unity + a `MockPrint`. Same pattern applies to:
  - GPAP/alarm message parsing (`alarm_api`, `GPAPMessage` — already covered)
  - Serial command interpretation (`gpad_serial.processSerial`, takes a `Stream*`)
  - MQTT topic/JSON config parsing and validation (`extractJsonString`, `parseCsvIntoTopics`, topic allow-listing — all pure string logic in `GPAD_API.ino`)
  - Operator settings bounds/validation (volume, mute timeout, alarm repeat — `operator_settings`)
  - Mute timeout state machine (`alarm_api`: `setMuted`/`serviceMuteTimeout`)
- **Hardware-in-the-loop (HIL) — needs a physical unit or fixture.** LCD output + menu navigation (`GPAD_menu`), rotary encoder input (`InterruptRotator`), DFPlayer audio playback, SPI broker mirror / SPI alarm input, boot-time self-diagnostics (`setup_status` — see §3, this one's interesting), real WiFi station connect / AP-fallback / captive portal, real MQTT broker connect/reconnect.
- **Network-reachable, semi-automatable — needs a powered, networked unit but not physical interaction.** The `AsyncWebServer` REST endpoints (WiFi credential save/load, MQTT settings, device status JSON) and the ElegantOTA update flow. These could in principle be driven by HTTP requests from a test runner against a unit that stays on the test network, without someone physically touching it — a middle ground worth calling out separately since it changes what "automated" could mean here.

**OPEN:** power-cycle/reliability (repeated reboot, brown-out recovery) and OTA-update-specific regression (does a completed OTA actually boot the new image correctly) aren't clearly covered above — need a decision on whether they're in scope for the first pass or deferred.

## 3. Relationship to the existing Factory Test

[Firmware/factoryTest](../../Firmware/factoryTest) has two things: `component-Focused/` (standalone per-component bring-up sketches — DFPlayer, ESP32, LCD, MQTT, RotaryEncoder, SPI, UART, each its own `.ino`, not linked into the GPAD_API build) and `FactoryTest_wMenu/` (one menu-driven sketch run on the production line). Neither reports structured pass/fail beyond what a human reads off the LCD/serial monitor at the bench, and neither builds against the actual shipping firmware — they're separate sketches that happen to exercise the same peripherals.

There's also a third thing worth distinguishing, found while reading the code: [setup_status.h](../../Firmware/GPAD_API/GPAD_API/setup_status.h) is a boot-time self-diagnostic *baked into the shipping firmware itself* — it flags LittleFS/DFPlayer/rotary-encoder/preferences init failures at every boot. That's not a test tool at all; it's product behavior, which makes it something regression tests should verify (does it still correctly flag a failure?), not a test method itself.

| | Factory Test | Setup-status self-diagnostic | Regression Test |
| --- | --- | --- | --- |
| Question it answers | "Was *this physical unit* assembled/wired correctly?" | "Did peripherals initialize OK *this boot*?" | "Did *this firmware change* break something that used to work?" |
| What it is | Standalone bench sketches, human-read result | Shipping firmware code (`setup_status.cpp`) | Not yet built |
| Runs against | Every unit, once, at manufacture | Every boot, every unit, forever | Every firmware change/release |
| Scope | Component presence/wiring/basic function | A fixed set of init failure flags | Full feature behavior, edge cases |

**Decision: reference the factory test sketches, don't reuse them as-is, and treat `setup_status` as a thing regression tests cover rather than a tool.**

| Option | Tradeoff |
| --- | --- |
| Reuse factory-test sketches as-is | Fast start, but they're separate `.ino` files disconnected from the real GPAD_API build — a HIL regression test would be verifying the sketch, not the shipping firmware. Defeats the purpose. |
| Build regression HIL harnesses fully independently | Cleanest separation, but throws away working pin-mapping/wiring knowledge already encoded in the component sketches (e.g. which pins the SPI test uses) — more work than necessary. |
| **Adapt: use the component sketches as a wiring/pin-mapping reference, write new harnesses that run against GPAD_API itself (chosen)** | Keeps the useful knowledge (how to wire/drive each peripheral) without inheriting the "standalone sketch, human reads result" limitation — new harnesses can assert programmatically and run against the actual firmware. |

This is a code-grounded call, not a preference call, so I made it rather than leaving it open — flag if that reasoning looks wrong.

## 4. One-time vs. repeated tests

Checked what actually runs today: [.github/workflows/gpad-firmware-version.yml](../../.github/workflows/gpad-firmware-version.yml) is the only workflow, and it only bumps the firmware version number after a PR merges. **No test of any kind currently runs automatically** — not even the existing native GPAP unit tests. That's a concrete gap, not a hypothetical one, and it changes this from "how do we classify tests" to "we currently have zero automated regression signal, and the §2 host-testable category is sitting there unused."

The host-testable/HIL/network-reachable split from §2 maps directly onto this, because cost-to-run is what should decide frequency:

| Category (§2) | Recommended trigger | Why |
| --- | --- | --- |
| Host-testable (native, no hardware) | Every PR, automatically | Already has a working pattern ([test/native](../../Firmware/GPAD_API/test)), costs nothing but CI minutes, catches parsing/logic regressions before merge. |
| Network-reachable (REST/OTA, needs a powered networked unit) | Repeated, but not per-commit — e.g. nightly or per release candidate against a dedicated always-on test unit | Automatable in principle, but requires infra (a unit permanently on a reachable network) that doesn't exist yet — running it per-commit isn't worth building for a team this size right now. |
| HIL (needs physical interaction / a fixture) | Repeated at release-candidate cut, not per-commit; one-time re-run when its specific hardware/firmware area changes | With a 1–3 person team and no automated rig, per-commit HIL isn't realistic. Tying it to release candidates keeps it repeated (so it's still regression testing) without demanding constant manual effort. |

So the one-time/repeated distinction isn't really about the *kind* of behavior being tested — it's about how expensive the test is to run, which §2's categories already determine. A test is "one-time" in practice only when it's tied to an event that doesn't recur on its own (e.g. validating a new hardware revision the first time it shows up), not because of what it's testing.

**OPEN — genuinely the team's call, not something the code can answer:**
- Is standing up CI for the host-testable tests (a new workflow running `pio test -e native` on every PR) worth doing now, given it's the one category with zero new infra required? This seems like the highest-leverage next step but I'm not adding a workflow file without a decision from you first, since it changes CI.
- Is a persistent networked test unit (for the network-reachable category) worth setting up at all, or does that tier just stay manual for now?
- Release-candidate cadence for HIL — is there an existing release cycle to hook into, or does this need to be defined from scratch?

## 5. Out of scope (for now)

- Hardware regression (enclosure, PCB) — will get its own section under [RegressionTesting](../README.md) when we pick it up.
- Anything beyond firmware.

## 6. Open questions for the team

Code-answerable questions are resolved above (§2 category split, §3 factory-test relationship, §4 cost-driven trigger framework). What's left is genuinely a team/process call:

- [ ] Are power-cycle/reliability and OTA-boot-verification in scope for the first pass, or deferred? (§2)
- [ ] Stand up CI now for the host-testable tests (`pio test -e native` on every PR), or later? (§4)
- [ ] Worth building a persistent networked test unit for the REST/OTA tier, or keep it manual? (§4)
- [ ] What defines a "release candidate" here for HIL cadence — is there an existing release process to hook into? (§4)
- [ ] Any GPAD_API modules or behaviors that should be explicitly out of scope?

## 7. Process notes

- Agile, small team.
- Docs-first: scope → (next) test plan / backlog → test cases. No test-writing until scope is agreed.
- This doc changes as decisions get made — keep it current rather than starting a new one each time.
