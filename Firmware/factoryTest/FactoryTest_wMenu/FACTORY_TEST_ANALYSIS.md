---
title: "Factory Test — Analysis & Change Record"
project: "Krake / GPAD v2"
document_id: "KRAKE-FT-ANALYSIS-01"
version: "v2.0.0"
date: "2026-04-01"
author: "Yukti (Claude Code)"
status: "Updated — reflects v0.4.7.0 refactor"
---

# Krake Factory Test: What It Tests and How

## 1. Purpose

`FactoryTest_wMenu` is a single-image firmware binary flashed onto every Krake
ESP32-WROOM-32D board before it leaves the factory floor. It lets one operator
with a USB serial terminal (115200 baud) walk through every hardware subsystem
in a defined order, get a PASS/FAIL on each, and leave with a populated summary
table. It is NOT a production firmware image — it is a manufacturing QA tool.

---

## 2. Hardware Under Test (summary)

| Subsystem | Key Parts | ESP32 Pins |
|---|---|---|
| MCU identity | ESP32-WROOM-32D | — |
| Status LED | LED_Status | GPIO 13 |
| Lamps | LAMP1–5 | GPIO 12, 14, 27, 26, 25 |
| Rotary encoder | CLK, DT, SW | GPIO 39, 36, 34 |
| Mute button | S601, R603 pull-up, C602 debounce | GPIO 35 |
| LCD (20×4) | PCF8574 I2C backpack | SDA/SCL (default I2C bus) |
| DFPlayer Mini | DF Robot Mini MP3 | UART2: TX=17 RX=16, BUSY=GPIO 14 |
| SD card | Inside DFPlayer module | (accessed via DFPlayer) |
| Speaker | Connected to DFPlayer | (audio output) |
| Wi-Fi | Internal SoC radio | — |
| LittleFS | Internal SPI flash | — |
| UART0 | USB-Serial (CH340 or CP2102) | GPIO 1/3 |
| SPI loopback | VSPI header / RJ12 | MOSI=23 MISO=19 SCK=18 CS=5 |
| RS-232 (UART1) | MAX3232, DB9 | TX=2 RX=15 RTS=32 CTS=33 |
| OTA (ElegantOTA) | AsyncWebServer + LittleFS | Wi-Fi |

> **Note:** LAMP2 (GPIO 14) is shared with the DFPlayer BUSY line. `SKIP_DRIVING_LAMP2`
> is hardcoded `true` to avoid driving BUSY during the LED blink test. This is a known
> hardware constraint, not a bug.

---

## 3. Test-by-Test Breakdown

### [0] Power / ID (`T_POWER`)
**What:** Confirms the MCU booted, USB serial is active, and prints identification:
firmware version, compile timestamp, chip revision, flash size, MAC address.  
**How:** Calls `printSplash()` (automatic — no hardware interaction). Always PASSes if
the code runs.  
**Business logic:** Provides a unique device identifier for the production record. The
MAC address is used to label units and link them to batch records.

---

### [1] Inputs — Encoder & Button (`T_INPUTS`)
**What:** Verifies the rotary encoder (CLK GPIO 39, DT GPIO 36, SW GPIO 34) produces
correct GPIO transitions for CW rotation, CCW rotation, and shaft press.  
**How:** Polls GPIO state for 10 seconds. Detects CLK edge with a 10 ms software
debounce guard and compares DT state to determine direction. Detects SW pulled LOW for
button press.  
**Business logic:** Rotary encoder is the primary user control for alarm volume and
menu navigation in production. A faulty encoder renders the device unacceptable.  
**Known gap:** GPIO 39, 36, 34 are input-only (no internal pull-up/pull-down). The
board supplies external biasing. The test validates functionality but not resistor values.

---

### [2] LCD I2C (`T_LCD`)
**What:** Scans I2C bus for a PCF8574 backpack at allowed addresses (0x27, 0x3F, 0x38),
confirms a panel is physically wired, initialises the 20×4 display, writes a
deterministic 4-row pattern, and asks the operator to visually confirm.  
**How:**
1. I2C address scan (Wire.beginTransmission / endTransmission).
2. `verifyLCDConnection()` — bit-bangs the PCF8574 in read mode to check the busy-flag
   upper nibble. `0xF0` means no panel connected (floating bus); any other value means
   at least one data line is being driven by a real LCD.
3. LiquidCrystal_I2C init, backlight on, `lcd.clear()`.
4. Print `####################`, `ABCDEFGHIJKLMNOPQRST`, `abcdefghijklmnopqrst`,
   `12345678901234567890` — one per row.
5. Operator Y/N prompt.
6. Backlight toggle (off 150 ms → on) as a secondary visual check.

**Business logic:** LCD is the only visual alarm display surface. A missing or
misaligned LCD is a ship-stopper.

---

### [3] LEDs / Lamps (`T_LEDS`)
**What:** Blinks LAMP1, LAMP3, LAMP4, LAMP5, and LED_Status one by one. Skips LAMP2
(shared with DFPlayer BUSY). Operator confirms visually.  
**How:** digitalWrite HIGH 300 ms → LOW 150 ms per lamp, then Y/N prompt.  
**Business logic:** Each lamp maps to an alarm severity level in production firmware.
A dead LED means a patient or operator may miss an alarm escalation.

---

### [4] DFPlayer Mini (`T_DFPLAYER`)
**What:** Initialises UART2 at 9600 baud, calls `DFRobotDFPlayerMini::begin()`, and
verifies the module responds to `readFileCounts()`.  
**How:** `initDFPlayer()` with a caching state machine (`DF_UNKNOWN → DF_OK | DF_FAIL`).
One retry on first `readFileCounts()` failure (modules are sometimes slow after cold
power-up). ACK mode is disabled (`begin(serial, false, true)`) matching the hardware.  
**Business logic:** DFPlayer is the only audio subsystem. No DFPlayer = no alarm tones
= device unusable in a clinical setting.

---

### [5] SD Card (`T_SD`)
**What:** Reads the file count from the SD card inserted in the DFPlayer module.  
**How:** `dfPlayer.readFileCounts()` with one retry on timeout. Requires DFPlayer
already initialised (test 4). Caches result in `g_sdFileCount` for test 6.  
**Business logic:** The alarm sound files are stored on the SD card. An empty or
unformatted card means silent alarms.

---

### [6] Speaker (`T_Speaker`)
**What:** Plays track #1 at volume 30 for 3 seconds and asks the operator to confirm
audio.  
**How:** `dfPlayer.volume(30); dfPlayer.play(1); wait 3s; dfPlayer.stop();` then Y/N.  
**Dependency guard:** Requires `dfState == DF_OK` and `g_sdFileCount > 0`. Fails
fast if either prerequisite is missing.  
**Business logic:** Full audio chain validation: DFPlayer → DAC → amplifier → speaker.
Maximum volume (30) during factory test ensures the amplifier is not clipping and the
speaker is not blown.

---

### [7] Wi-Fi AP (`T_WIFI_AP`)
**What:** Starts a Wi-Fi access point with SSID `KRAKE_<MAC>` and WPA2 password
`krakefactory`.  
**How:** `WiFi.softAP(ssid, "krakefactory")`. Operator told to verify AP is visible
from a phone or PC (no automatic check). Also starts ElegantOTA server on the AP.  
**Business logic:** AP mode is used for initial provisioning and OTA updates in the
field without a local router. SSID encodes the MAC so multiple units can be on the
floor simultaneously.

---

### [8] Wi-Fi STA (`T_WIFI_STA`)
**What:** Connects to a known Wi-Fi network (SSID and password entered by operator).  
**How:** `readLineOrMenuAbort()` collects SSID and password, then `WiFi.begin()` with
20-second timeout. Reports IP and RSSI on success.  
**Business logic:** Production firmware uses Wi-Fi STA mode for MQTT connectivity.
Confirms the radio links to the lab AP and that antenna / matching network is intact.

---

### [A] LittleFS R/W (`T_LITTLEFS`)
**What:** Writes `KRAKE_FACTORY_TEST` to `/factory_test.txt`, reads it back, verifies
content matches, then deletes the file.  
**How:** LittleFS is mounted in `setup()` with `format-on-fail`. Write → close → open
→ readString → compare → `LittleFS.remove()`.  
**Business logic:** LittleFS stores OTA web assets (favicon, index.html, style.css).
A corrupted flash partition means no OTA updates in the field.

---

### [B] UART0 USB Serial (`T_UART0`)
**What:** Asks the operator "Can you see this prompt and respond?" — a pure
confirmation that UART0 and the USB-UART bridge are working.  
**How:** Y/N prompt only.  
**Business logic:** UART0 is the primary debug/configuration interface. If the operator
can see and answer the question, the USB-UART bridge and UART0 peripheral are both
functioning.

---

### [C] SPI Loopback (`T_SPI`)
**What:** Sends a 7-byte test pattern (`0x55 0xAA 0x00 0xFF 0x12 0x34 0xA5`) over
VSPI at 1 MHz with a loopback jumper on the RJ12 header (MOSI→MISO), and validates
each received byte equals the sent byte.  
**How:** `SPI.begin / SPISettings / SPI.transfer` per byte. Fails on first mismatch.  
**Business logic:** SPI port exposes an external peripheral interface. Validates the
VSPI peripheral, MOSI/MISO routing, level shifting, and the RJ12 connector
soldering/crimping.

---

### [D] RS-232 Loopback / UART1 (`T_RS232`)
**What:** Sends `KRAKE_rs232_UART1_LOOPBACK_123\r\n` over UART1 through the MAX3232
transceiver and DB9 connector, expects the same string back via a loopback plug
(TX↔RX, RTS↔CTS at DB9 pins 2, 3, 7, 8).  
**How:**
1. Assert RTS (GPIO 32) LOW → MAX3232 inverts → positive RS-232 voltage on DB9 pin 7.
2. **Flow-control check first:** Read CTS (GPIO 33) — must be LOW (positive RS-232
   looped back through MAX3232). If HIGH, report flow-control fault and stop.
3. Send payload into fixed `char rxBuf[64]`, wait up to 600 ms.
4. Validate length and byte-exact content with `memcmp`.

**Business logic:** RS-232 port is used for legacy alarm panel integration (4-wire
serial control). Validates MAX3232 chip, DB9 connector, and RS-232 RTS/CTS flow
control line.

---

### [E] ElegantOTA (`T_OTA`)
**What:** Verifies the OTA web server is accessible at `http://<IP>/update` in a
browser.  
**How:** If Wi-Fi is not already up, attempts Wi-Fi STA first; fails the test if STA
cannot connect. Prompts operator to open the URL and confirm the page loads.  
**Business logic:** Field firmware updates are delivered over Wi-Fi using ElegantOTA.
Confirms LittleFS partition, AsyncWebServer, and OTA endpoint are all correctly
configured before the unit ships.

---

### [F] Mute Button + LED (`T_MUTE_BTN`)
**What:** Tests S601 mute button (GPIO 35, active LOW, external pull-up R603, RC
debounce C602) and LED_Status toggle.  
**How:** Uses OneButton library with 20 ms debounce (software layer, C602 handles
hardware debounce) and 800 ms long-press threshold. Waits for 2 valid presses:
press 1 → LED ON (muted), press 2 → LED OFF (unmuted). Too-short taps ignored,
too-long presses generate a warning.  
**Business logic:** The mute button is a safety-critical input (operator can silence
an alarm). LED_Status provides visual feedback that mute is active. Both must
function correctly for patient-safety compliance.

---

## 4. Operator Interaction Model

```
USB Serial terminal (115200 baud, any newline style)
  │
  ├── Menu key (single char + Enter) → runs that test
  ├── P → runs all tests 0→F in sequence
  ├── R → reboots ESP32
  └── Q → returns to menu from within any test
       (also accepted as first char of any Y/N prompt)
```

Y/N prompts accept:
- Immediate `Y` or `N` (no Enter needed)
- `Y` + Enter or `N` + Enter
- First character of any menu key → aborts test, marks FAIL, dispatches key next loop

**Note:** `'0'` is not treated as a menu-abort key inside prompts — it can only launch
test [0] from the top-level menu. This prevents operators accidentally aborting a test
by pressing `0`.

---

## 5. Changes Made in v0.4.7.0

The following issues were identified in the pre-refactor baseline and resolved.

### 5.1 Structural / Clarity

| # | Issue | Resolution |
|---|---|---|
| 1 | `printBanner()` and `splashserial()` printed nearly identical output | Merged into single `printSplash()`. Both old functions removed. |
| 2 | Stale `VERSION " V0.0.1 "`, `MODEL_NAME`, `LICENSE`, `ORIGIN` defines leftover from template | All four defines removed. `FIRMWARE_VERSION` is the single source of truth. |
| 3 | `DEVICE_UNDER_TEST` hardcoded with no warning | Added a prominent two-line comment above the define making it clear it must be updated per unit/batch. |
| 4 | `T_COUNT` enum gap (indices 0–8 then A–F, skipping 9) | Not changed — this is a cosmetic mismatch between enum integer values and display characters. The mapping via `handleCommand()` switch is correct and functional. Documented as a known quirk. |
| 5 | LED names array contained duplicates like `"LAMP2 (LAMP2)"` | Replaced all six names with self-documenting labels including GPIO number and shared-function note (e.g. `"LAMP2 (GPIO 14 / DF BUSY — skipped)"`). |
| 6 | `splashserial()` called after every single test in `handleCommand()` | Removed. `printSplash()` is now called only in `setup()` and `runTest_Power()`. |

### 5.2 Robustness / Correctness

| # | Issue | Resolution |
|---|---|---|
| 7 | RS-232 flow-control CTS check ran *after* data receive, giving ambiguous errors | Moved flow-control check to run first. Data receive and `memcmp` only run if CTS polarity is correct. |
| 8 | LittleFS test left `/factory_test.txt` on filesystem after the test | Added `LittleFS.remove(path)` immediately after successful read-back. |
| 9 | Speaker test hardcoded track 1 with no guard for missing track | Not changed. Guard already exists (`g_sdFileCount > 0`). Track-1 naming convention is a factory SD card preparation requirement, not a firmware issue. Documented. |
| 10 | Encoder edge detection had no software debounce — bouncy contacts could false-trigger | Added 10 ms minimum gap between edges (`lastEdgeMs` guard). Does not affect the 10-second test window. |
| 11 | `muteBtn` OneButton constructed before `setup()`, `pinMode` deferred to test | Not changed. ESP32 GPIO defaults to input at reset, so this is safe. Documented in code. |

### 5.3 Manufacturing / Business Logic

| # | Issue | Resolution |
|---|---|---|
| 12 | `printSummary()` had no machine-parseable output for automated logging | Added `RESULT:device=...,fw=...,pass=...,fail=...,verdict=...` line at end of every summary. |
| 13 | No overall PASS/FAIL verdict line in summary | Added `>>> ALL TESTS PASSED <<<` or `>>> FAIL: N test(s) failed, M passed <<<` above the RESULT line. |
| 14 | Wi-Fi AP password `"krakefactory"` not prominently documented | Noted in code comment on `WiFi.softAP()` call. Flagged in this document. Password is intentionally simple for factory floor use — not used in production firmware. |
| 15 | OTA test requires operator with a browser — cannot be headless | Not changed. Acceptable at current batch scale (≤20 units). Documented as a known limitation. |

### 5.4 Code Consistency

| # | Issue | Resolution |
|---|---|---|
| 16 | Mixed `F()` / raw string literal patterns | All new or modified string literals use `F()`. Pre-existing raw literals in untouched code paths left as-is to minimise diff. |
| 17 | `String rx` in RS-232 receive loop allocated on heap | Replaced with `char rxBuf[64]` fixed buffer. `memcmp` used instead of `String ==`. |
| 18 | `static char lineBuf[32]` in `loop()` not commented | Added comment explaining the static lifetime is intentional (persists across loop calls to accumulate a line). |
| 19 | `verifyLCDConnection()` defined but never called | Wired into `runTest_LCD()` as Step 2, between I2C address scan and LCD init. A detected backpack with no panel now fails with a specific error message. |

### 5.5 Comment Cleanup

The following comment issues were addressed in a separate pass:

- **Removed** stale version number from file header block (was still `v0.4.6.0`).
- **Removed** the badly-typed `/* Function Description Initilize the DF player ... */` block above `initDFPlayer()` — replaced with an accurate 4-line doc comment.
- **Removed** dead commented-out code: `//dfPlayer.enableACK()` and `//digitalWrite(UART1_RTS1, HIGH)`.
- **Removed** `[v0.4.4.0]` version-tag style comment on `startOTAServerIfNeeded()` — revision history is the right place for that.
- **Removed** the wrong description on `runTest_OTA()` that said "fall back to AP if STA fails" — the code returns false on STA failure; there is no AP fallback.
- **Removed** inline narration comments inside `verifyLCDConnection()` that described each `Wire.write` call — trimmed to a compact function-level description explaining the technique.
- **Removed** inline narration inside `promptYesNo()` body (`// --- ENTER pressed ---`, `// null terminate`, `// Append safely`, etc.) — the logic is readable without them.
- **Fixed** `//comment` → `// comment` formatting throughout.
- **Fixed** `clearDFPlayerCache()` comment — it said "optional: hard reset" when the reset always executes.
- **Fixed** `printDetail()` comment — it said "Put this OUTSIDE of runTest_DFPlayer()" which is an artifact of copy-paste origin; replaced with a description of what the function does.
- **Fixed** `runTest_LittleFS()` comment — changed "LittleFS is mounted at boot" to "LittleFS is mounted in setup()" (precise location matters).
- **Fixed** UART1 pin comments — replaced vague "placeholder safe GPIO" labels with actual signal direction and MAX3232 pin names.
- **Fixed** RS-232 RTS/CTS comment — explained the MAX3232 polarity inversion so the `digitalWrite(LOW)` → positive RS-232 voltage logic is not mysterious.
- **Added** function-level comments to `g_pendingCmd`, `isMenuKey()`, `readLineOrMenuAbort()`, `promptYesNo()`, `clearDFPlayerCache()`, `initDFPlayer()`, `printDetail()`, `startOTAServerIfNeeded()` — all non-obvious in behaviour.
- **Added** to `setup()`: explanation of the 2 s USB CDC timeout cap, the `format-on-fail` rationale, and why Wi-Fi is briefly enabled then disabled at boot.

---

## 6. Known Remaining Issues / Future Work

The following issues were identified but intentionally deferred — they require hardware
decisions or are out of scope for a firmware-only refactor:

1. **`DEVICE_UNDER_TEST` still requires manual source edit per batch.** A better
   approach would be to read a unit serial number from NVS or a factory-programmed
   eFuse, or accept it via a serial prompt at boot. Tracked for a future ticket.

2. **Speaker test assumes track 1 exists on the SD card.** Factory SD card preparation
   procedure must guarantee a file named `001.mp3` (DFPlayer indexing convention) is
   present. This is an assembly process requirement, not something firmware can enforce.

3. **OTA test requires a browser and cannot be headless.** For future higher-volume
   production runs, consider an automated HTTP GET to `/update` from the test PC and
   parsing the 200 response.

4. **`addrInAllowList()` helper function is defined but never called.** The LCD scan
   iterates `LCD_ALLOWED_ADDRS` directly. This dead code can be removed in a future
   cleanup pass.

5. **`printDetail()` is defined but never called from `loop()`.** The DFPlayer async
   event handler exists as infrastructure but is not wired into the event loop. Fine
   for factory test purposes (polling is used instead), but the function is misleading
   dead code.

6. **`T_COUNT` enum integer values do not match the visual menu character mapping.**
   `T_LITTLEFS` through `T_MUTE_BTN` have integer values 9–14 but are displayed and
   dispatched as `A`–`F`. This is functional but confusing for anyone reading the
   `TestIndex` enum in isolation.

---

## 7. What Was NOT Changed

All of the following were verified unchanged from v0.4.6.0 to v0.4.7.0:

- All GPIO pin assignments
- Menu key bindings (0–9, A–F, P, R, Q)
- Y/N prompt timing (`PROMPT_TIMEOUT_MS = 65000`) and abort behaviour
- ElegantOTA endpoint (`/update`)
- LittleFS mount with `format-on-fail = true`
- OneButton configuration for mute button (debounce 20 ms, long-press 800 ms)
- DFPlayer UART2 / 9600 baud / ACK disabled configuration
- SPI test pattern and 1 MHz speed
- RS-232 loopback payload string (`KRAKE_rs232_UART1_LOOPBACK_123\r\n`)
- `ENABLE_RS232_TEST` compile guard
- `SKIP_DRIVING_LAMP2` safety guard
- `platformio.ini` build configuration
