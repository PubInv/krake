# Firmware Regression Testing — Scope

Status: **draft, structural questions resolved from the code and process decisions recorded; one question still open (see §6)**. This doc defines *what* we will test and *how tests are classified* — not the test cases themselves.

## 1. Purpose

Catch regressions in existing, already-working firmware behavior as the codebase changes — as opposed to verifying a freshly-manufactured unit is built correctly (that's factory test, see §3). Complete firmware coverage: everything shipped in [GPAD_API](../../Firmware/GPAD_API), not just one module.

**Baseline for "expected behavior":** existing firmware behavior provides a characterization baseline. Expected behavior should be agreed against intended requirements and documented interfaces (e.g. the GPAP protocol and the command list in the [GPAD_API README](../../Firmware/GPAD_API/GPAD_API/README.md)). Code/documentation discrepancies and known defects must be recorded explicitly rather than automatically treated as correct behavior. For example, the MQTT broker credentials hardcoded in [GPAD_API.ino](../../Firmware/GPAD_API/GPAD_API/GPAD_API.ino) are a known issue tracked separately; a test that captures current behavior there should mark it as a known defect, not lock it in as correct.

**Known defects.** These are open bugs found in the current code. Tests must not lock them in as expected behavior. A test in one of these areas should check the intended behavior and be marked as a known failure until the fix lands, or leave that case out for now.
- [#615](https://github.com/PubInv/krake/issues/615): the USB and COM ports share one command buffer, so a half-finished line from one port can be joined with characters from the other port.
- [#616](https://github.com/PubInv/krake/issues/616): the web page publish and the main loop share the MQTT send queue with no lock, so a message can be lost or sent with the wrong topic.

## 2. Test categories in scope

Read through [GPAD_API.ino](../../Firmware/GPAD_API/GPAD_API/GPAD_API.ino) and the module headers to ground this in what's actually built, rather than guess. The firmware splits cleanly into three groups by *what it takes to run the test* — which turns out to be the more useful axis than "functional vs. HIL" alone, because it's what determines whether a test can be automated:

- **Host-testable (no hardware needed).** Logic that can run on a dev machine with PlatformIO's `native` platform, Unity, and the mocks in [lib/mocking](../../Firmware/GPAD_API/lib/mocking). Precedent exists: [test/native/test_GPAP](../../Firmware/GPAD_API/test/native/test_GPAP/test_gpap_message.cpp).

  Important limit, found in the code: the `native` test build only compiles `lib/` (the GPAP library and the mocks), not the firmware source folder `GPAD_API/`. Almost every file in `GPAD_API/` includes `<Arduino.h>` or `<Stream.h>`, which do not exist on a laptop. `lib/mocking` only has `MockPrint`/`MockPrintable` (text output). There is no mock for input streams, `millis()`, `yield()`, Arduino `String` or ESP32 `Preferences`. So this category splits in two:

  **a) Can be tested on the host today, with no code changes:**
  - GPAP message parsing in [lib/GPAP](../../Firmware/GPAD_API/lib/GPAP): alarm messages (partly covered, see "Existing coverage" below), plus mute, unmute, help and info messages and malformed input, which are not yet covered. This library already switches to the mocks under `PIO_UNIT_TESTING`.

  **b) Needs preparation first:**

  | Module / behavior | What blocks host testing | What is needed |
  | --- | --- | --- |
  | Operator settings (`operator_settings.cpp`) | Reads/writes ESP32 `Preferences` (flash storage). The clamping logic (`clampSetting`) is private to the file. Calls `setSetupError()`, whose header includes `<Arduino.h>`. | A mock `Preferences`, or extract the clamping/default logic into a function that can be called on its own. |
  | Mute timing (`alarm_api.cpp`: `setMuteTimeoutMinutes`, `serviceMuteTimeout`) | Calls `millis()` directly. File includes `<Arduino.h>`. | An injectable clock (pass the current time in, or a replaceable time function), plus a small Arduino shim. |
  | Alarm state (`alarm_api.cpp`: `alarm()`, `setMuted`, `unmute`) | Includes `<Arduino.h>`. Uses `Stream` and `printError()` (from `gpad_utility`, which needs `<Stream.h>`). Keeps state in globals (`currentLevel`, `currentlyMuted`, `AlarmMessageBuffer`). | A small Arduino shim (`Stream`, `F()`), and a way to reset the globals between tests. |
  | Serial line handling (`gpad_serial.cpp`: `processSerial()`) | Calls `interpretBuffer()` (in `GPAD_HAL.cpp`, next to LCD/audio code), `applyInterpretedCommand()` (in the main sketch: MQTT client, `ESP.restart()`), `printAlarmState()`, `cancelPendingAlarmAudio()` (audio) and `yield()`. Keeps its buffer in a global `buf` and a `static writeIndex` that both serial ports share (known defect [#615](https://github.com/PubInv/krake/issues/615)). | Separate line assembly from command execution (extract), a mock input `Stream`, and a stub for `yield()`. |
  | Command interpretation (`GPAD_HAL.cpp`: `interpretBuffer()`) | Shared by serial and MQTT, but it lives in the hardware layer file with LCD, LED and DFPlayer code. Uses `Stream::printf`, and the reset command calls LCD functions. | Extract it from `GPAD_HAL.cpp` into its own file, plus the Arduino shim. |
  | MQTT parsing helpers (`extractJsonString`, `parseJsonStringAt`, `parseCsvIntoTopics`, `normalizeMqttBrokerName`, `isAllowedPublishTopic`, `jsonEscape`, and the topic checks used by `callback()`) | Still in the main sketch `GPAD_API.ino`, which cannot be built on the host. Use Arduino `String`. `isAllowedPublishTopic` reads global topic lists. | Extract them from the main sketch into their own `.cpp/.h`, an Arduino `String` shim (or switch to plain C strings), and pass topic lists in as parameters. |
  | MQTT send queue (`mqtt_handler.cpp`: `queueMqtt`, `serviceMqttQueue`) | Includes `<Arduino.h>` and `<PubSubClient.h>`. Every queued message is also copied to the SPI mirror (`queueSpiBrokerMirror`). The queue is a global array. Shared by the main loop and the web page task (known defect [#616](https://github.com/PubInv/krake/issues/616)). | A mock MQTT client (only `connected()` and `publish()` are used), a stub for the SPI mirror, and a way to empty the queue between tests. |
  | Boot self-diagnostic text (`setup_status.cpp`) | The logic is plain C, but its header includes `<Arduino.h>` only for basic integer types. | Swap that include for standard headers, or add a small Arduino shim. |
  | `millis()` rollover helpers (`gpad_utility.h`: `millisIntervalElapsed`, `millisDeadlineReached`) | The helpers themselves are plain inline functions, but the header includes `<Stream.h>`. | Move the helpers into their own header, or add the Arduino shim. |
- **Hardware-in-the-loop (HIL) — needs a physical unit or fixture.** LCD output + menu navigation (`GPAD_menu`), rotary encoder input (`InterruptRotator`), DFPlayer audio playback, SPI broker mirror / SPI alarm input, boot-time self-diagnostics (`setup_status` — see §3, this one's interesting), real WiFi station connect / AP-fallback / captive portal, real MQTT broker connect/reconnect.
- **Network-reachable, semi-automatable — needs a powered, networked unit but not physical interaction.** The `AsyncWebServer` REST endpoints (WiFi credential save/load, MQTT settings, device status JSON) and the ElegantOTA update flow. These could in principle be driven by HTTP requests from a test runner against a unit that stays on the test network, without someone physically touching it — a middle ground worth calling out separately since it changes what "automated" could mean here.

**Existing coverage: partial GPAP parser coverage exists.** The native suite ([test_gpap_message.cpp](../../Firmware/GPAD_API/test/native/test_GPAP/test_gpap_message.cpp)) has five GPAP alarm-message parsing tests: two field orders (content/designator/id and designator/content/id), content only, id only, and type designator only. Not yet covered:
- Other GPAP message types (mute, unmute, help, info) and malformed GPAP input
- `alarm_api` state transitions (level changes, message replacement, invalid level)
- Mute expiry (timed unmute, "mute forever" setting, `millis()` rollover)
- Serial buffering (line assembly, `\r` handling, overlong lines, two ports at once)
- Command interpretation (`interpretBuffer()`) and the MQTT parsing helpers
- Operator settings limits and persistence
- The full command-to-output path: a command arriving on serial or MQTT → alarm state → LEDs, LCD, audio and MQTT reply

**Critical firmware behaviors in scope.** Whatever the test layer, these behaviors must be covered (test cases come later, in the backlog):
- **Severity to LED/audio mapping:** each alarm level (`a0`–`a5`) drives the right LEDs, LCD backlight and sound.
- **Mute/unmute and timeout:** mute by command (`s`) stays muted until unmuted; mute by button or menu uses the saved mute duration and ends on its own; "mute forever" (`0` minutes) stays muted; unmute (`u`) works in every case.
- **Acknowledgement:** acknowledge/dismiss/shelve/complete from the knob menu sends the correct GPAP reply with the alarm ID.
- **Alarm replacement:** a new alarm replaces the current level, message, ID and type.
- **Malformed commands:** bad or overlong input is rejected with an error and pending alarm audio is cancelled, without changing the current alarm. An empty serial line is ignored.
- **Settings persistence:** volume, mute duration, alarm repeat, COM port and MQTT settings survive a restart and stay within their limits.
- **Continued alarm operation during network failure:** with Wi-Fi or the MQTT broker down, alarms from serial (and SPI where fitted) still sound, show and light up, and the device reconnects when the network returns.

**Decided: power-cycle and OTA.** The first pass covers basic restart, settings persistence across restart, and successful boot after an OTA update. Extensive brownout and endurance testing is deferred.

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
| Host-testable (native, no hardware) | Every PR, automatically (once native CI is added, see "Decided" below) | Already has a working pattern ([test/native](../../Firmware/GPAD_API/test)), costs nothing but CI minutes, catches parsing/logic regressions before merge. |
| Network-reachable (REST/OTA, needs a powered networked unit) | Repeated, but not per-commit — e.g. nightly or per release candidate against a dedicated always-on test unit | Automatable in principle, but requires infra (a unit permanently on a reachable network) that doesn't exist yet — running it per-commit isn't worth building for a team this size right now. |
| HIL (needs physical interaction / a fixture) | Repeated at release-candidate cut, not per-commit; one-time re-run when its specific hardware/firmware area changes | With a 1–3 person team and no automated rig, per-commit HIL isn't realistic. Tying it to release candidates keeps it repeated (so it's still regression testing) without demanding constant manual effort. |

So the one-time/repeated distinction isn't really about the *kind* of behavior being tested — it's about how expensive the test is to run, which §2's categories already determine. A test is "one-time" in practice only when it's tied to an event that doesn't recur on its own (e.g. validating a new hardware revision the first time it shows up), not because of what it's testing.

**Change impact also sets frequency.** Test frequency is based on change impact as well as execution cost. Changes affecting alarm handling, timing, audio or settings must trigger the relevant device checks before a release candidate is prepared, even if those checks are normally run less often.

**Decided:**
- **Native CI:** add it in a follow-up PR, after confirming the existing native suite runs. The workflow should run the native tests and an ESP32 firmware build check.
- **Persistent networked test unit:** start with manual device testing and recorded results. Add a dedicated unit when recurring testing justifies it.
- **Release candidate:** a specific commit/build nominated for release, with its test results and unresolved defects recorded.

## 5. Out of scope (for now)

- Hardware regression (enclosure, PCB) — will get its own section under [RegressionTesting](../README.md) when we pick it up.
- Anything beyond firmware.

## 6. Open questions for the team

Code-answerable questions are resolved above (§2 category split, §3 factory-test relationship, §4 cost-driven trigger framework).

**Decided:**

- [x] Power-cycle and OTA: first pass covers basic restart, settings persistence, and successful boot after OTA. Extensive brownout and endurance testing is deferred. (§2)
- [x] Native CI: in a follow-up PR, after confirming the existing suite runs. Include native tests and an ESP32 firmware build check. (§4)
- [x] Persistent networked test unit: start with manual device testing and recorded results. Add a dedicated unit when recurring testing justifies it. (§4)
- [x] Release candidate: a specific commit/build nominated for release, with its test results and unresolved defects recorded. (§4)

**Still open:**

- [ ] Any GPAD_API modules or behaviors that should be explicitly out of scope?

## 7. Process notes

- Small team, no formal process overhead — plain docs, decided and executed directly.
- Docs-first: scope → task list → test cases. No test-writing until scope is agreed.
- This doc changes as decisions get made — keep it current rather than starting a new one each time.
