# Firmware Regression Testing — Task List

Status: **draft**. What needs to get built, grouped by what's ready to start vs. what's blocked on a decision. Follows on from [Scope.md](Scope.md). This is planning, not test code — nothing here has been written yet.

## Order of attack

Grouped by what it takes to run each test, same split as [Scope.md §2](Scope.md#2-test-categories-in-scope) — that's what actually determines whether something can start today.

| Ordering by | Tradeoff |
| --- | --- |
| Most safety-critical first (alarm delivery, since that's the actual point of the device) | Right instinct, but the most critical logic (GPAP alarm parsing) also happens to be the cheapest to test — it's already testable on a dev machine with existing precedent. Ordering by criticality alone buries an easy win under a "start with the scary stuff" framing. |
| Arbitrary / by category | No signal in the order, doesn't help decide what to pick up first. |
| **Cheapest-to-build first (chosen): things testable on a dev machine → hardware-in-the-loop → networked-device tests → decisions blocking the rest** | Matches reality: group 1 needs no new setup and can start immediately. Groups 2 and 3 are blocked on decisions in group 4. Within each group, items are still ordered by risk — so the safety-relevant work is first *within* the group that's actually unblocked. |

---

## 1. Tests we can write now (run on a dev machine, no hardware needed)

Extends the existing pattern in [test/native/test_GPAP](../../Firmware/GPAD_API/test/native/test_GPAP/test_gpap_message.cpp) (`pio test -e native`). Nothing here is blocked.

- **Cover the other GPAP message types.** [GPAPMessage.h](../../Firmware/GPAD_API/lib/GPAP/src/GPAPMessage.h) defines five message types — `ALARM`, `INFO`, `MUTE`, `UNMUTE`, `HELP` — plus an invalid case. The existing test only covers `ALARM`. Four types plus the malformed-input path have zero coverage right now.
- **Serial command interpretation.** `processSerial`/`interpretBuffer` ([gpad_serial.h](../../Firmware/GPAD_API/GPAD_API/gpad_serial.h)) take a `Stream*` — same mockable pattern the existing test already uses. No tests exist yet.
- **MQTT topic/JSON config parsing.** `extractJsonString`, `parseCsvIntoTopics`, `isManagedSubscribedTopic`, `endsWithAckTopic`, topic allow-listing — pure string logic in `GPAD_API.ino`, no hardware involved.
- **Operator settings bounds checking.** Volume / mute-timeout / alarm-repeat clamping in [operator_settings.h](../../Firmware/GPAD_API/GPAD_API/operator_settings.h) — the min/max constants are already defined, easy to test against.
- **Mute timeout logic.** `setMuted`/`unmute`/`serviceMuteTimeout` in `alarm_api` — time-based, testable with a fake clock.

## 2. Tests that need real hardware — blocked on deciding how often HIL runs (Scope.md §6)

Can be designed now, but not scheduled until that decision lands.

- **Alarm end-to-end: MQTT in → LCD/LED/audio out → ack/dismiss/shelve.** The actual safety-relevant path, and currently nothing exercises it start to finish — top priority once this group is unblocked.
- **WiFi connect / AP-fallback / captive portal**, including credential reset.
- **MQTT live broker connect/reconnect**, including switching to a custom broker.
- **Rotary encoder driving LCD menu navigation.**
- **DFPlayer audio playback.**
- **SPI broker mirror / SPI alarm input.**
- **Boot-time `setup_status` self-checks** — confirm each failure flag still fires when its peripheral is actually faulted (this tests the diagnostic itself — see Scope.md §3 on why it's not a test tool).

## 3. Tests that need a powered, networked unit — blocked on deciding if that unit gets built (Scope.md §6)

- WiFi credential save/load endpoints.
- MQTT broker/settings endpoints (profile switch, custom broker config, topic config).
- Device status JSON endpoint (`trackedKrakesJson` and related).
- ElegantOTA update flow, including whether a completed OTA actually boots the new image (still an open question in Scope.md §2 on whether this is in scope at all — confirm before spending time on it).

## 4. Decisions blocking groups 2 and 3

Straight from [Scope.md §6](Scope.md#6-open-questions-for-the-team) — not test work, but what's stopping the rest from moving:

- Stand up a CI check running `pio test -e native` on every PR — doesn't unblock anything new by itself, but it's what turns group 1 from "tests that exist" into "tests that actually catch regressions" instead of ones someone has to remember to run.
- Decide whether to build a persistent networked test unit — unblocks group 3.
- Decide how often/when group 2 (HIL) actually runs — unblocks group 2.
- Decide whether power-cycle/reliability and OTA-boot verification are worth covering in this first pass.

---

## Not doing yet

Sizing and exact assertions per item — those depend on group 4's decisions, since cadence and tooling change how a group-1 item gets written (exact test code) versus a group-2 item (can't nail down automation detail until the HIL cadence question is settled).
