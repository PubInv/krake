// IMPORTANT: Update DEVICE_UNDER_TEST to the actual serial number before flashing.
// Leaving this as the default value will cause the production record to be wrong.
#define DEVICE_UNDER_TEST "SN: LB0008"  // Serial number — CHANGE PER UNIT/BATCH
#define PROG_NAME "FactoryTest_wMenu"
#define FIRMWARE_VERSION "v0.4.7.0"
/*
------------------------------------------------------------------------------
File:            FactoryTest_wMenu.ino
Project:         Krake / GPAD v2 – Factory Test Firmware
Document Type:   Source Code (Factory Test)
Document ID:     KRAKE-FT-ESP32-FT01
Version:         v0.4.7.0
Date:            2026-04-01
Author(s):       Nagham Kheir, Public Invention
Status:          Draft
------------------------------------------------------------------------------
Revision History:
| Version |   Date    | Author        | Description                                     |
|---------|-----------|---------------|-------------------------------------------------|
| v0.1.0  | 2025-12-03| N. Kheir      | Initial unified factory test code               |
| v0.2.0  | 2025-12-27| N. Kheir      | Operator confirmation for LCD/LED               |
| v0.3.0  | 2025-12-27| N. Kheir      | Prompt/menu abort handling improvements         |
| v0.3.1  | 2025-12-27| N. Kheir      | SD gating to avoid DF actions with no SD        |
| v0.4.0  | 2025-12-27| N. Kheir      | Robust Y/N, safer Run-All, clearer I2C, DF/SD   |
| v0.4.1  | 2025-12-27| N. Kheir      | Cleanup: remove duplicate DF flags, fix         |
|         |           |               | LCD allow-list loop, clean line input parsing   |
|v0.4.1.1 | 2025-12-30| N. Kheir      | UART1 backloop working and final.               |
|v0.4.2.0 | 2026-1-1  | N. Kheir      | SPI backloop working and final. DFP debug       |
|v0.4.2.1 | 2026-1-1  | N. Kheir      | DFplayer cleanup test.                          |
|v0.4.2.2 | 2026-1-1  | N. Kheir      | SPI cleanup test.                               |
|v0.4.2.3 | 2026-1-5  | L. Erickson   | Make menu two columns.                          |
|v0.4.2.4 | 2026-1-6  | L. Erickson   | Make volume full, 30.                           |
|v0.4.2.5 | 2026-1-7  | L. Erickson   | Use myDFPlayer.getVersion()                     |
|v0.4.2.6 | 2026-1-8  | Yukti         | Fix DFPlayer test to fail cleanly when hardware |
|         |           |               | is missing; improve error handling              |
|v0.4.2.7 | 2026-2-7  | L. Erickson   | bugfix/356-firmware-factory-test-bring-up-add-  |
|                                       flow-control-test-for-com-port                  |
|v0.4.2.x | 2026-2-23 | N. Kheir      | Adding a splash serial                          |
|v0.4.3.0 | 2026-2-27 | Yukti         | add global 'exit' command to abort any test     |
|         |           |               | and return to menu.                             |
|v0.4.3.1 | 2026-2-28 | Yukti         | updated code and firmware ver. based on review  |
|v0.4.3.2 | 2026-2-28 | Yukti         | changed 'exit' to 'break'                       |
|v0.4.3.3 | 2026-3-01 | Yukti         | added global break support to all tests         |
|v0.4.3.4 | 2026-3-01 | Yukti         | Refactor GLOBAL BREAK handling; remove dynamic  |
|         |           |               | String usage from interactive paths; add        |
|         |           |               | cooperative break checks in long-running loops  |
|v0.4.3.5 | 2026-3-08 | Yukti         | Changed abort keyword from 'break' to 'q'       |
|         |           |               | to fix collision with menu keys B and R         |
|v0.4.4.0 | 2026-3-10 | Yukti         | Add ElegantOTA + LittleFS OTA background        |
|         |           |               | service; mount LittleFS at boot; add T_OTA      |
|         |           |               | test (key E) for operator browser verification  |
|         |           |               | Requires: AsyncTCP, ESPAsyncWebServer,          |
|         |           |               | ElegantOTA libs from Arduino Library Manager    |
|v0.4.4.1 | 2026-3-18 | Yukti         | Added automatic Wifi Testing inside OTA test    |
|v0.4.5.0 | 2026-3-17 | Yukti         | Add T_MUTE_BTN test (key F): tests S601 mute    |
|         |           |               | push button GPIO and LED_Status toggle.         |
|         |           |               | Uses OneButton for press-duration handling:     |
|         |           |               | too-short ignored, too-long warns operator.     |
|         |           |               | active-LOW, external pull-up R603, hardware     |
|         |           |               | RC debounce C602 on PCB. internalPullup=false.  |
|         |           |               | Requires: OneButton lib from Library Manager.   |
|v0.4.5.1 | 2026-3-23 | Yukti         | Fixed DFPlayer ACK handling                     |
|v0.4.5.2 | 2026-3-24 | Yukti         | Added MAC address display to splash screen      |
|v0.4.6.0 | 2026-3-31 | Yukti         | Migrate to PlatformIO (#352)                    |
|v0.4.7.0 | 2026-4-01 | Yukti         | Refactor: merge printBanner/splashserial into   |
|         |           |               | printSplash; remove stale VERSION define; wire  |
|         |           |               | verifyLCDConnection; encoder SW debounce; fix   |
|         |           |               | RS232 check order + char buf; LittleFS cleanup; |
|         |           |               | verdict + CSV result line in printSummary.      |
----------------------------------------------------------------------------------------|
Overview:
- Repeatable factory test sequence for ESP32-WROOM-32D Krake/GPAD v2 boards.
- Operator interacts via USB Serial @ 115200 baud.
- Y/N prompts accept Y/N immediately (Enter optional). CR/LF supported.
- During prompts, menu keys typed as the FIRST character abort that step
  (marks FAIL) and the command runs next.
- WiFi SSID/PASS input does NOT abort just because SSID starts with A/B/C/D.
  It aborts only when the entire line is exactly one menu key (e.g., "C"+Enter).
- DFPlayer test now fails cleanly when hardware is missing or unresponsive.
------------------------------------------------------------------------------
Build notes:
- Adjust pin mapping to match your PCB (especially UART1 pins).
- If LAMP2 shares DFPlayer BUSY, we skip driving LAMP2 in the LED test.
- Ensure DF_RXD2 and DF_TXD2 are correctly wired to DFPlayer module.
- MUTE_BTN_PIN: confirm GPIO number from ESP32 net connection sheet.
  Schematic shows active-LOW with external pull-up R603 and RC debounce C602.
  Do NOT enable internal pull-up; R603 is the pull-up.
- OneButton library required: install from Arduino Library Manager.
------------------------------------------------------------------------------
*/
#define BAUDRATE 115200  // USB serial baud rate


#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <FS.h>
#include <LittleFS.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobotDFPlayerMini.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <OneButton.h>

// ============================================================================
// Configuration
// ============================================================================

static const uint32_t SERIAL_BAUD = 115200;
static const uint32_t PROMPT_TIMEOUT_MS = 65000;
static const uint32_t WIFI_CONNECT_MS = 20000;

static const bool SKIP_DRIVING_LAMP2 = true;  // shared with DF BUSY on some builds
static const uint8_t LCD_ALLOWED_ADDRS[] = { 0x27, 0x3F, 0x38 };
static const bool ENABLE_RS232_TEST = true;

// ============================================================================
// Hardware configuration (ADJUST to match Krake / GPAD v2 wiring)
// ============================================================================

// Lamps / LEDs
const int LED_Status = 13;  // Mute LED / general status
const int LAMP1 = 12;       // Lamp 1
const int LAMP2 = 14;       // Lamp 2 (often DFPlayer BUSY on some builds)
const int LAMP3 = 27;       // Lamp 3 (also SPI CS)
const int LAMP4 = 26;       // Lamp 4 (also SPI SCK)
const int LAMP5 = 25;       // Lamp 5 (also SPI MISO)

// Rotary encoder (input-only pins OK on ESP32)
const int ENC_CLK = 39;
const int ENC_DT = 36;
const int ENC_SW = 34;

// DFPlayer / speaker (UART2)
const int DF_TXD2 = 17;        // ESP32 TX2 -> DFPlayer RX
const int DF_RXD2 = 16;        // ESP32 RX2 <- DFPlayer TX
const int DF_BUSY_IN = LAMP2;  // Active LOW when playing (if wired)

// SPI (VSPI)
const int SPI_MOSI_PIN = 23;
const int SPI_MISO_PIN = 19;
const int SPI_SCK_PIN = 18;
const int SPI_CS_PIN = 5;
static const uint32_t SPI_SPEED = 1000000;  // 1 MHz — conservative, safe for loopback test
static const uint8_t TEST_PATTERN[] = {
    0x55, 0xAA, 0x00, 0xFF, 0x12, 0x34, 0xA5};

// RS-232 (UART1) via MAX3232 — verify these match the PCB net connection sheet
const int UART1_TXD1 = 2;   // ESP32 TX1 → MAX3232 T1IN
const int UART1_RXD1 = 15;  // ESP32 RX1 ← MAX3232 R1OUT
const int UART1_RTS1 = 32;  // ESP32 output → MAX3232 T2IN (DB9 pin 7 via inversion)
const int UART1_CTS1 = 33;  // ESP32 input  ← MAX3232 R2OUT (DB9 pin 8 via inversion)
const long UART1_BAUD = 115200;
static const uint32_t TIMEOUT_MS = 600;

// ----------------------------------------------------------------------------
// Mute button (S601) — hardware notes from schematic:
//   - Active LOW: pressing connects GPIO to GND
//   - External pull-up via R603 to 3v3 (do NOT enable internal pull-up)
//   - Hardware RC debounce via C602 (100nF) already on PCB
//   - MUTE_BTN_PIN: confirm GPIO number from ESP32 net connection sheet
// ----------------------------------------------------------------------------
const int MUTE_BTN_PIN = 35;

static const uint32_t MUTE_DEBOUNCE_MS  =  20;   // low because C602 handles HW debounce
static const uint32_t MUTE_LONGPRESS_MS = 800;   // above this = held too long
static const uint32_t MUTE_WAIT_MS      = 15000; // operator press window per phase

// activeLow=true, internalPullup=false (R603 is the external pull-up on PCB)
static OneButton muteBtn(MUTE_BTN_PIN, true, false);

// Callback flags: set inside callbacks, read+cleared inside test
static volatile bool g_muteBtnClicked     = false;
static volatile bool g_muteBtnHeldTooLong = false;

// DFPlayer bookkeeping + SD cache
HardwareSerial dfSerial(2);
DFRobotDFPlayerMini dfPlayer;
enum dfState_t { DF_UNKNOWN, DF_OK, DF_FAIL };
static dfState_t dfState = DF_UNKNOWN;
// SD cache: read file count ONCE during test [5], reuse during test [6]
static bool g_sdChecked = false;
static int g_sdFileCount = -999;

// OTA server instance and state flags
static AsyncWebServer otaServer(80);
static bool g_otaServerStarted = false;  // true once ElegantOTA is running
static bool g_littleFsMounted  = false;  // true once LittleFS is mounted at boot

// ============================================================================
// Test bookkeeping
// ============================================================================

enum TestIndex {
  T_POWER = 0,
  T_INPUTS,
  T_LCD,
  T_LEDS,
  T_DFPLAYER,
  T_SD,
  T_Speaker,
  T_WIFI_AP,
  T_WIFI_STA,
  T_LITTLEFS,
  T_UART0,
  T_SPI,
  T_RS232,
  T_OTA,
  T_MUTE_BTN,
  T_COUNT
};

const char* TEST_NAMES[T_COUNT] = {
    "0 Power / ID",
    "1 Inputs (Encoder / Button)",
    "2 LCD (I2C)",
    "3 LEDs / Lamps",
    "4 DFPlayer ",
    "5 SD (DFPlayer card)",
    "6 Speaker",
    "7 Wi-Fi AP",
    "8 Wi-Fi STA (manual SSID/PASS)",
    "A LittleFS R/W",
    "B UART0 (USB Serial)",
    "C SPI loopback",
    "D RS-232 loopback",
    "E ElegantOTA",
    "F Mute Button + LED"
};

bool testResults[T_COUNT] = { false };

// ============================================================================
// Serial helpers + pending command capture
// ============================================================================

// When a test prompt is aborted by a menu key, we can't call handleCommand()
// recursively from inside the test. Instead we store the key here and dispatch
// it at the top of the next loop() iteration.
static char g_pendingCmd = 0;

static char up(char c)
{
  return (char)toupper((unsigned char)c);
}

// Returns true if c is a valid top-level menu key.
// Note: '0' is intentionally excluded — it is not recognised as an abort key
// inside prompts so that test [0] can only be launched from the main menu.
static bool isMenuKey(char c) {
  c = up(c);
  return (c >= '1' && c <= '8') || (c >= 'A' && c <= 'F') || c == 'P' || c == 'R' || c == 'Q';
}

static void flushSerialRx() {
  while (Serial.available()) (void)Serial.read();
}


// Reads a line from Serial into `out`. Returns true on normal completion
// (Enter pressed or timeout). Returns false if the completed line is exactly
// one menu key character — in that case the key is stored in g_pendingCmd
// and `out` is cleared. Prevents SSID/password inputs from false-triggering
// just because the first character happens to be a menu key.
static bool readLineOrMenuAbort(String& out, uint32_t timeoutMs = 15000) {
  out = "";
  uint32_t start = millis();

  while (millis() - start < timeoutMs) {
    while (Serial.available()) {
      char c = Serial.read();

      if (c == '\n' || c == '\r') {
        out.trim();
        if (out.length() == 1 && isMenuKey(out[0])) {
          g_pendingCmd = up(out[0]);
          out = "";
          return false;  // aborted
        }

        return true;  // completed normally
      }

      out += c;
    }
    delay(5);
  }

  out.trim();
  return true;  // timeout (out may be empty)
}


// Prints `question` and waits for a Y/N response. Accepts bare Y or N (no
// Enter needed) as well as Y/N + Enter. A menu key typed as the first
// character aborts the current test (stores key in g_pendingCmd, returns false).
// Timeout returns !defaultNo — most tests default to FAIL on no response.
static bool promptYesNo(const __FlashStringHelper* question,
                        uint32_t timeoutMs = PROMPT_TIMEOUT_MS,
                        bool defaultNo = true)
{
  Serial.println(question);
  Serial.println(F("Press Y to PASS or N to FAIL (Enter optional)."));
  Serial.println(F("(Type 'q' to quit and return to menu.)"));
  Serial.print(F("> "));

  uint32_t start = millis();

  char buf[16];
  uint8_t idx = 0;

  while (millis() - start < timeoutMs) {

    while (Serial.available()) {

      char c = Serial.read();

      if (c == '\n' || c == '\r') {
        buf[idx] = '\0';

        if (idx == 0) continue;  // bare newline, wait for real input

        char k = up(buf[0]);

        if (k == 'Y') return true;
        if (k == 'N') return false;

        if (isMenuKey(k)) {
          g_pendingCmd = k;
          Serial.println(F("\nAborted -> FAIL."));
          return false;
        }

        Serial.println(F("\nUnknown input -> FAIL"));
        return false;
      }

      // First char is a menu key: abort immediately without waiting for Enter
      if (idx == 0 && isMenuKey(c)) {
        g_pendingCmd = up(c);
        Serial.println(F("\nAborted -> FAIL."));
        return false;
      }

      // Accept bare Y or N (no Enter required)
      char u = up(c);
      if (u == 'Y') { Serial.println(F("Y")); return true;  }
      if (u == 'N') { Serial.println(F("N")); return false; }

      if (idx < sizeof(buf) - 1) buf[idx++] = c;
    }

    delay(5);
  }

  Serial.println(defaultNo ? F("\nNo response -> FAIL")
                           : F("\nNo response -> PASS"));

  return !defaultNo;
}

// ============================================================================
// UI
// ============================================================================

// Combined device identity banner — called once at boot and after reboot.
static void printSplash() {
  Serial.println();
  Serial.println(F("======================================================="));
  Serial.println(F("        KRAKE FACTORY TEST — ESP32-WROOM-32D           "));
  Serial.println(F("======================================================="));
  Serial.print(F("Firmware:  "));   Serial.println(F(FIRMWARE_VERSION));
  Serial.print(F("Program:   "));   Serial.println(F(PROG_NAME));
  Serial.print(F("Device:    "));   Serial.println(F(DEVICE_UNDER_TEST));
  Serial.print(F("Built:     "));   Serial.println(F(__DATE__ " " __TIME__));
  Serial.print(F("MAC (STA): "));   Serial.println(WiFi.macAddress());
  Serial.printf("Chip rev:  %d\n",  ESP.getChipRevision());
  Serial.printf("Flash:     %u bytes\n", ESP.getFlashChipSize());
  Serial.println(F("License:   GNU Affero General Public License v3"));
  Serial.println(F("======================================================="));
  Serial.println();
}

static void printSummary() {
  Serial.println();
  Serial.println(F("========== FACTORY TEST SUMMARY =========="));

  int passCount = 0;
  int failCount = 0;

  for (int i = 0; i < T_COUNT; i += 2) {

    // Left column
    Serial.printf(
        "%-33s : %-4s",
        TEST_NAMES[i],
        testResults[i] ? "PASS" : "FAIL");
    testResults[i] ? passCount++ : failCount++;

    // Right column (if present)
    if (i + 1 < T_COUNT) {
      Serial.printf(
          "    %-33s : %-4s",
          TEST_NAMES[i + 1],
          testResults[i + 1] ? "PASS" : "FAIL");
      testResults[i + 1] ? passCount++ : failCount++;
    }

    Serial.println();
  }

  Serial.println(F("=========================================="));

  // Overall verdict
  if (failCount == 0) {
    Serial.println(F(">>> ALL TESTS PASSED <<<"));
  } else {
    Serial.printf(">>> FAIL: %d test(s) failed, %d passed <<<\n", failCount, passCount);
  }

  // Machine-parseable result line for automated test station capture
  Serial.printf("RESULT:device=%s,fw=%s,pass=%d,fail=%d,verdict=%s\n",
                DEVICE_UNDER_TEST, FIRMWARE_VERSION,
                passCount, failCount,
                (failCount == 0) ? "PASS" : "FAIL");

  Serial.println();
}

static void printMenu() {
  Serial.println(F("Test menu (order matters):"));
  Serial.println(F(" 0 Power / ID                    1 Inputs (Encoder / Button)"));
  Serial.println(F(" 2 LCD (I2C)                     3 LEDs / Lamps"));
  Serial.println(F(" 4 DFPlayer                      5 SD (DFPlayer card)"));
  Serial.println(F(" 6 Speaker                       7 Wi-Fi AP"));
  Serial.println(F(" 8 Wi-Fi STA (manual SSID/PASS)  A LittleFS R/W"));
  Serial.println(F(" B UART0 (USB Serial)            C SPI loopback"));
  Serial.println(F(" D RS-232 loopback               E ElegantOTA"));
  Serial.println(F(" F Mute Button + LED"));
  Serial.println(F(" P Run ALL (0 -> F)              R Reboot"));
  Serial.println(F(" Q Quit current test"));
  Serial.println();
  Serial.println(F("Enter command: "));
}

// ============================================================================
// Tests
// ============================================================================

static bool runTest_Power() {
  Serial.println(F("\n[0] Power / ID"));
  printSplash();
  Serial.println(F("Power OK: MCU running and Serial is active."));
  return true;
}

static bool runTest_Inputs() {
  Serial.println(F("\n[1] Inputs (Encoder / Button)"));

  pinMode(ENC_CLK, INPUT);
  pinMode(ENC_DT, INPUT);
  pinMode(ENC_SW, INPUT_PULLUP);

  Serial.println(F("Rotate encoder CLOCKWISE, then COUNTER-CLOCKWISE,"));
  Serial.println(F("then PRESS the encoder button within 10 seconds."));
  delay(200);

  int lastCLK = digitalRead(ENC_CLK);
  bool sawCW = false;
  bool sawCCW = false;
  bool sawPress = false;
  unsigned long lastEdgeMs = 0;  // software debounce: ignore edges < 10 ms apart

  unsigned long start = millis();
  while (millis() - start < 10000UL) {
    int clk = digitalRead(ENC_CLK);
    unsigned long now = millis();
    if (clk != lastCLK && (now - lastEdgeMs) >= 10UL) {
      lastEdgeMs = now;
      if (digitalRead(ENC_DT) != clk) {
        sawCW = true;
        Serial.println(F("  Detected rotation: CW"));
      } else {
        sawCCW = true;
        Serial.println(F("  Detected rotation: CCW"));
      }
      lastCLK = clk;
    }

    if (!sawPress && digitalRead(ENC_SW) == LOW) {
      sawPress = true;
      Serial.println(F("  Detected encoder button press"));
      delay(150);
    }

    if (sawCW && sawCCW && sawPress) break;
  }

  bool pass = (sawCW && sawCCW && sawPress);
  Serial.printf("Inputs test: %s\n", pass ? "PASS" : "FAIL (missing rotation and/or press)");
  return pass;
}

static bool addrInAllowList(uint8_t addr) {
  const size_t n = sizeof(LCD_ALLOWED_ADDRS) / sizeof(LCD_ALLOWED_ADDRS[0]);
  for (size_t i = 0; i < n; ++i) {
    if (LCD_ALLOWED_ADDRS[i] == addr) return true;
  }
  return false;
}

// Confirms an LCD panel is physically wired to the PCF8574 backpack by reading
// the LCD busy flag. PCF8574 pinout assumed: P0=RS, P1=RW, P2=EN, P3=BL, P4-P7=D4-D7.
// Requires R/W (P1) connected to the LCD R/W pin — standard on all common backpacks.
//
// Technique: set RW=1 (read mode) and drive P4-P7 high (quasi-bidirectional inputs).
// Then pulse EN to clock out the upper nibble from the LCD. If the LCD is absent the
// PCF8574 pull-ups float all data lines high (reads 0xF0). A connected LCD drives at
// least one data line low.
static bool verifyLCDConnection(uint8_t i2c_addr) {
  const uint8_t RW = 0x02;
  const uint8_t EN = 0x04;
  const uint8_t BL = 0x08;  // backlight on P3 — standard PCF8574 backpack wiring

  // P4-P7 written high so they act as inputs; BL kept on; RW=1 selects read mode
  const uint8_t dataMask = 0xF0;
  const uint8_t ctrlSet  = BL | RW;

  // Pulse EN high, read upper nibble
  Wire.beginTransmission(i2c_addr);
  Wire.write(dataMask | ctrlSet | EN);
  Wire.endTransmission();

  Wire.requestFrom((int)i2c_addr, 1);
  if (!Wire.available()) return false;
  uint8_t readVal = Wire.read();

  Wire.beginTransmission(i2c_addr);
  Wire.write(dataMask | ctrlSet);  // EN low — complete the pulse
  Wire.endTransmission();

  // In 4-bit mode we must complete a second EN pulse for the lower nibble
  // to keep the LCD's internal nibble counter aligned for subsequent writes.
  Wire.beginTransmission(i2c_addr);
  Wire.write(dataMask | ctrlSet | EN);
  Wire.endTransmission();
  Wire.beginTransmission(i2c_addr);
  Wire.write(dataMask | ctrlSet);
  Wire.endTransmission();

  // Restore write mode (RW=0) so the LiquidCrystal library can take over
  Wire.beginTransmission(i2c_addr);
  Wire.write(BL);
  Wire.endTransmission();

  uint8_t upperNibble = readVal & 0xF0;
  Serial.printf("  [LCD Diagnostic] Read: 0x%02X (upper nibble: 0x%02X)\n", readVal, upperNibble);

  // 0xF0 means all data lines are floating high — no LCD panel connected
  return (upperNibble != 0xF0);
}

static bool runTest_LCD() {
  Serial.println(F("\n[2] LCD (I2C)"));

  Wire.begin();
  delay(100);

  // STEP 1: Scan allowed I2C addresses
  uint8_t lcdAddr = 0;
  for (uint8_t addr : LCD_ALLOWED_ADDRS) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      lcdAddr = addr;
      Serial.printf("  LCD backpack found at 0x%02X\n", addr);
      break;
    }
  }

  if (!lcdAddr) {
    Serial.println(F("FAIL: No LCD backpack detected."));
    return false;
  }

  // STEP 2: Verify LCD is physically connected (busy-flag read via PCF8574)
  if (!verifyLCDConnection(lcdAddr)) {
    Serial.println(F("FAIL: LCD backpack found on I2C but no LCD panel responding (open circuit?)."));
    return false;
  }
  Serial.println(F("  LCD panel connected (busy-flag check OK)."));

  // STEP 3: Initialize LCD cleanly
  LiquidCrystal_I2C lcd(lcdAddr, 20, 4);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(5);

  // STEP 4: Deterministic pattern test
  const char* patterns[4] = {
      "####################",
      "ABCDEFGHIJKLMNOPQRST",
      "abcdefghijklmnopqrst",
    "12345678901234567890"
  };

  for (uint8_t row = 0; row < 4; row++) {
    lcd.setCursor(0, row);
    lcd.print(patterns[row]);
  }

  // STEP 5: Operator optical confirmation
  bool visible = promptYesNo(
      F("Do you see 4 FULL lines, aligned, no garbage characters?"),
      PROMPT_TIMEOUT_MS,
    false
  );

  if (!visible) {
    Serial.println(F("FAIL: LCD optical test failed."));
    return false;
  }

  // STEP 6: Backlight toggle (non-destructive)
  lcd.noBacklight();
  delay(150);
  lcd.backlight();

  Serial.println(F("LCD test: PASS"));
  return true;
}

static bool runTest_LEDs() {
  Serial.println(F("\n[3] LEDs / Lamps"));

  const int pins[] = { LAMP1, LAMP2, LAMP3, LAMP4, LAMP5, LED_Status };
  const char* names[] = {
      "LAMP1  (GPIO 12)",
      "LAMP2  (GPIO 14 / DF BUSY — skipped)",
      "LAMP3  (GPIO 27 / SPI CS)",
      "LAMP4  (GPIO 26 / SPI SCK)",
      "LAMP5  (GPIO 25 / SPI MISO)",
      "LED_Status (GPIO 13 / Mute LED)"
  };

  for (int i = 0; i < 6; ++i) {
    if (SKIP_DRIVING_LAMP2 && pins[i] == LAMP2) continue;
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
  delay(50);

  for (int i = 0; i < 6; ++i) {
    if (SKIP_DRIVING_LAMP2 && pins[i] == LAMP2) {
      Serial.println(F("  -> Skipping LAMP2 drive (BUSY shared safety)")); continue;
    }
    Serial.print(F("  -> "));
    Serial.print(names[i]);
    Serial.println(F(" blink"));
    digitalWrite(pins[i], HIGH);
    delay(300);
    digitalWrite(pins[i], LOW);
    delay(150);
  }

  bool ok = promptYesNo(F("Did you see the LEDs/Lamps blink as expected?"), PROMPT_TIMEOUT_MS, true);
  Serial.printf("LEDs/Lamps test: %s\n", ok ? "PASS" : "FAIL");
  return ok;
}


// Resets the DFPlayer state machine and closes the UART session so the next
// initDFPlayer() call starts from a clean slate.
static void clearDFPlayerCache() {
  dfState = DF_UNKNOWN;
  dfSerial.end();
  delay(50);
}

// Quick "ping" to verify the module is actually responding NOW.
// If module is removed, most DF commands return < 0 (no reply).
static bool dfPlayerResponding() {
  int c = dfPlayer.readFileCounts();
  if (c >= 0) return true;

  // One small retry (some modules are slow / SD busy)
  delay(150);
  c = dfPlayer.readFileCounts();
  return (c >= 0);
}

// Initialises the DFPlayer Mini over UART2. Caches result in dfState so
// repeated calls are cheap. If the module was previously OK but stops
// responding (e.g. physically removed), the cache is invalidated and a
// fresh init is attempted. Returns false immediately if a prior init failed.
static bool initDFPlayer() {
  Serial.print("initDFPlayer() called at: ");
  Serial.println(millis());

  if (dfState == DF_OK) {
    if (dfPlayerResponding()) return true;
    Serial.println(F("DFPlayer lost response, clearing cache."));
    clearDFPlayerCache();
  }

  if (dfState == DF_FAIL) return false;

  Serial.println(F("Initializing DFPlayer (UART2)..."));

  pinMode(DF_BUSY_IN, INPUT_PULLUP);

  dfSerial.begin(9600, SERIAL_8N1, DF_RXD2, DF_TXD2);

  delay(300);

  if (!dfPlayer.begin(dfSerial, false, true)) {
    Serial.println(F("DFPlayer not detected (check connections)."));
    dfState = DF_FAIL;
    return false;
  }
  else {
    Serial.println(F("DFPlayer detected."));
  }

  dfPlayer.setTimeOut(1000);
  dfPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  delay(1200);

  if (!dfPlayerResponding()) {
    Serial.println(F("DFPlayer init completed but no response."));
    dfState = DF_FAIL;
    return false;
  }

  dfState = DF_OK;
  Serial.println(F("DFPlayer detected and responding."));
  return true;
}


// Decodes and prints DFPlayer async event notifications.
// Call when dfPlayer.available() returns true (e.g. from loop()).
void printDetail(uint8_t type, int value) {
  switch (type) {
  case TimeOut:
    Serial.println(F("Time Out!"));
    break;
  case WrongStack:
    Serial.println(F("Stack Wrong!"));
    break;
  case DFPlayerCardInserted:
    Serial.println(F("Card Inserted!"));
    break;
  case DFPlayerCardRemoved:
    Serial.println(F("Card Removed!"));
    break;
  case DFPlayerCardOnline:
    Serial.println(F("Card Online!"));
    break;
  case DFPlayerUSBInserted:
    Serial.println("USB Inserted!");
    break;
  case DFPlayerUSBRemoved:
    Serial.println("USB Removed!");
    break;
    case DFPlayerPlayFinished:
    Serial.print(F("Number:"));
    Serial.print(value);
    Serial.println(F(" Play Finished!"));
    break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
    case Busy:
      Serial.println(F("Card not found"));
      break;
    case Sleeping:
      Serial.println(F("Sleeping"));
      break;
    case SerialWrongStack:
      Serial.println(F("Get Wrong Stack"));
      break;
    case CheckSumNotMatch:
      Serial.println(F("Check Sum Not Match"));
      break;
    case FileIndexOut:
      Serial.println(F("File Index Out of Bound"));
      break;
    case FileMismatch:
      Serial.println(F("Cannot Find File"));
      break;
    case Advertise:
      Serial.println(F("In Advertise"));
      break;
    default:
      break;
      }
      break;
  default:
    break;
  }
}


static bool runTest_DFPlayer(bool forceReinit = false) {
  Serial.println(F("\n[4] DF Player"));

  if (forceReinit) {
    clearDFPlayerCache();
  }

  if (!initDFPlayer()) {
    Serial.println(F("DFPlayer test FAIL: module not detected or not responding."));
    return false;
  }

  Serial.println(F("DFPlayer test PASS: module detected and responding."));
  return true;
}

static bool runTest_SD() {
  Serial.println(F("\n[5] SD (DFPlayer card)"));

  if (!initDFPlayer()) {
    Serial.println(F("SD test skipped: DFPlayer not present."));
    g_sdChecked = true;
    g_sdFileCount = -1;
    return false;
  }

  g_sdChecked = true;
  g_sdFileCount = -1;

  Serial.println(F("Reading SD file count (once)..."));
  g_sdFileCount = dfPlayer.readFileCounts();

  if (g_sdFileCount < 0) {
    Serial.println(F("  No reply / SD not ready. Waiting 1.5s and retrying ONCE..."));
    delay(1500);
    g_sdFileCount = dfPlayer.readFileCounts();
  }

  g_sdChecked = true;
  Serial.printf("  DFPlayer reports %d files.\n", g_sdFileCount);

  if (g_sdFileCount <= 0) {
    Serial.println(F("SD test: FAIL (no readable files)."));
    return false;
  }

  return true;
}

static bool runTest_Speaker() {
  Serial.println(F("\n[6] Speaker"));
  Serial.println(F("Playing track #1 for ~3 seconds..."));

  if (dfState != DF_OK) {
    Serial.println(F("Speaker test FAIL: DFPlayer not present."));
    return false;
  }

  if (!g_sdChecked || g_sdFileCount <= 0) {
    Serial.println(F("Speaker test FAIL: SD not ready or no readable files."));
    return false;
  }

  const int VOL = 30;
  dfPlayer.volume(VOL);
  Serial.printf("DFPlayer volume set to: %d\n", VOL);

  dfPlayer.play(1);

  uint32_t start = millis();
  while (millis() - start < 3000) {
    delay(10);
  }

  dfPlayer.stop();
  delay(200);

  bool ok = promptYesNo(F("Did you hear audio from the speaker?"),
                        PROMPT_TIMEOUT_MS,
                        true);
  Serial.printf("Speaker test: %s\n", ok ? "PASS" : "FAIL");
  return ok;
}

// Starts the ElegantOTA web server on port 80. Safe to call from both AP and
// STA tests — does nothing after the first successful call.
static void startOTAServerIfNeeded() {
  if (g_otaServerStarted) return;

  otaServer.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send(200, "text/plain",
              "KRAKE Factory Test OTA. Go to /update to upload firmware.");
  });

  ElegantOTA.begin(&otaServer);  // mounts the /update endpoint
  otaServer.begin();
  g_otaServerStarted = true;

  Serial.println(F("ElegantOTA server started. Browse to http://<IP>/update"));
}

static bool runTest_WifiAP() {
  Serial.println(F("\n[7] Wi-Fi AP"));

  WiFi.mode(WIFI_AP);
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String defaultSsid = "KRAKE_" + mac;

  flushSerialRx();

  Serial.print(F("AP SSID [default: "));
  Serial.print(defaultSsid);
  Serial.println(F("] — press Enter to keep:"));
  Serial.print(F("> "));
  String ssid;
  if (!readLineOrMenuAbort(ssid, 25000)) {
    Serial.println(F("\nAborted by menu key."));
    return false;
  }
  ssid.trim();
  if (ssid.length() == 0) ssid = defaultSsid;

  Serial.println(F("AP PASSWORD [default: krakefactory] — press Enter to keep:"));
  Serial.print(F("> "));
  String pass;
  if (!readLineOrMenuAbort(pass, 25000)) {
    Serial.println(F("\nAborted by menu key."));
    return false;
  }
  pass.trim();
  if (pass.length() == 0) pass = "krakefactory";

  bool ok = WiFi.softAP(ssid.c_str(), pass.c_str());
  if (!ok) {
    Serial.println(F("Failed to start Wi-Fi AP."));
    return false;
  }

  IPAddress ip = WiFi.softAPIP();
  Serial.print(F("AP SSID: ")); Serial.println(ssid);
  Serial.print(F("AP Pass: ")); Serial.println(pass);
  Serial.print(F("AP IP:   ")); Serial.println(ip);
  Serial.println(F("Operator: verify AP is visible from phone/PC."));

  startOTAServerIfNeeded();
  return true;
}

static bool runTest_WifiSTA() {
  Serial.println(F("\n[8] Wi-Fi STA (manual SSID/PASS)"));

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  delay(300);

  flushSerialRx();

  Serial.println(F("Enter Wi-Fi SSID then press Enter (25 s timeout):"));
  Serial.print(F("> "));
  String ssid;
  if (!readLineOrMenuAbort(ssid, 25000)) {
    Serial.println(F("\nAborted by menu key."));
    return false;
  }
  ssid.trim();
  if (ssid.length() == 0) {
    Serial.println(F("No SSID entered. Wi-Fi STA test FAIL."));
    return false;
  }

  Serial.println(F("Enter Wi-Fi PASSWORD then press Enter (25 s timeout, can be empty):"));
  Serial.print(F("> "));
  String pass;
  if (!readLineOrMenuAbort(pass, 25000)) {
    Serial.println(F("\nAborted by menu key."));
    return false;
  }
  pass.trim();

  Serial.print(F("Connecting to: "));
  Serial.println(ssid);
  WiFi.begin(ssid.c_str(), pass.c_str());

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_MS) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("Wi-Fi STA connection FAILED."));
    return false;
  }

  Serial.println(F("Wi-Fi STA connected successfully."));
  Serial.print(F("IP:   "));
  Serial.println(WiFi.localIP());
  Serial.print(F("RSSI: "));
  Serial.println(WiFi.RSSI());

  startOTAServerIfNeeded();
  return true;
}

static bool runTest_LittleFS() {
  Serial.println(F("\n[A] LittleFS R/W"));

  // LittleFS is mounted in setup(); only attempt a recovery mount here if that failed
  if (!g_littleFsMounted) {
    Serial.println(F("  LittleFS not mounted at boot, attempting mount now..."));
    g_littleFsMounted = LittleFS.begin(true);
  }

  if (!g_littleFsMounted) {
    Serial.println(F("LittleFS mount FAILED."));
    return false;
  }

  const char* path = "/factory_test.txt";
  const char* payload = "KRAKE_FACTORY_TEST";

  Serial.print(F("Writing "));
  Serial.println(path);
  File f = LittleFS.open(path, FILE_WRITE);
  if (!f) {
    Serial.println(F("Failed to open file for write."));
    return false;
  }
  f.print(payload);
  f.close();

  Serial.println(F("Reading back..."));
  f = LittleFS.open(path, FILE_READ);
  if (!f) {
    Serial.println(F("Failed to open file for read."));
    return false;
  }
  String readBack = f.readString();
  f.close();

  bool ok = (readBack == payload);
  // Clean up: remove the test file so re-runs start fresh
  LittleFS.remove(path);
  Serial.printf("LittleFS test: %s\n", ok ? "PASS" : "FAIL (content mismatch)");
  return ok;
}

static bool runTest_UART0() {
  Serial.println(F("\n[B] UART0 (USB Serial)"));
  bool ok = promptYesNo(F("UART0 check: can you see this prompt and respond?"), PROMPT_TIMEOUT_MS, true);
  Serial.printf("UART0 test: %s\n", ok ? "PASS" : "FAIL");
  return ok;
}

static bool runTest_SPI() {
  Serial.println(F("\n[C] SPI loopback"));
  Serial.println(F("Connect MOSI <-> MISO on SPI header for automatic test."));
  Serial.printf("Using pins: MOSI=%d, MISO=%d, SCK=%d, CS=%d\n",
                SPI_MOSI_PIN, SPI_MISO_PIN, SPI_SCK_PIN, SPI_CS_PIN);

  SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_CS_PIN);
  pinMode(SPI_CS_PIN, OUTPUT);

  digitalWrite(SPI_CS_PIN, HIGH);
  delay(20);
  Serial.println("[SPI] Starting RJ12 loopback test");

  SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  digitalWrite(SPI_CS_PIN, LOW);

  bool pass = true;

  for (size_t i = 0; i < sizeof(TEST_PATTERN); i++) {
    uint8_t tx = TEST_PATTERN[i];
    uint8_t rx = SPI.transfer(tx);

    if (rx != tx) {
      Serial.printf("[SPI] FAIL at byte %u: TX=0x%02X RX=0x%02X\n",
                    (unsigned)i, tx, rx);
      pass = false;
      break;
    }
  }

  digitalWrite(SPI_CS_PIN, HIGH);
  SPI.endTransaction();
  SPI.end();

  if (pass) {
    Serial.println("[SPI] PASS: RJ12 loopback OK");
  }
  return pass;
}

static bool runTest_RS232() {

  if (!ENABLE_RS232_TEST) {
    Serial.println(F("\n[D] RS-232 loopback disabled (ENABLE_RS232_TEST=false)"));
    return false;
  }

  Serial.println(F("\n[D] RS-232 loopback (UART1)"));
  Serial.println(F("Connected RS-232 TX <-> RX at DB9 (pins 2 and 3) via MAX3232."));
  Serial.printf("Using UART1 TXD=%d, RXD=%d, BAUD=%ld\n", UART1_TXD1, UART1_RXD1, UART1_BAUD);

  pinMode(UART1_RTS1, OUTPUT);
  pinMode(UART1_CTS1, INPUT);  // no pull-up: MAX3232 actively drives this line
  // RTS LOW → MAX3232 inverts → positive RS-232 voltage on DB9 pin 7 → loopback drives CTS LOW
  digitalWrite(UART1_RTS1, LOW);

  HardwareSerial rs232(1);
  rs232.begin(UART1_BAUD, SERIAL_8N1, UART1_RXD1, UART1_TXD1);
  delay(150);
  // Clear any pending garbage
  while (rs232.available()) rs232.read();

  const char* payload = "KRAKE_rs232_UART1_LOOPBACK_123\r\n";
  const size_t n = strlen(payload);

  size_t written = rs232.write((const uint8_t*)payload, n);
  rs232.flush();
  if (written != n) {
    Serial.println("[rs232] FAIL: write length mismatch");
    rs232.end();
    return false;
  }

  // STEP 1: Validate flow-control lines FIRST (clear diagnostic before data test)
  if (HIGH == digitalRead(UART1_CTS1)) {
    Serial.println(F("[rs232] FAIL: FLOW CONTROL. CTS should be LOW (positive RS-232 voltage looped back)."));
    Serial.println(F("Tip: ensure the RS-232 RTS<->CTS loopback plug is installed at DB9."));
    rs232.end();
    return false;
  }

  // STEP 2: Receive loopback data (fixed-size buffer — no heap String)
  char rxBuf[64];
  size_t rxLen = 0;
  uint32_t t0 = millis();
  while (millis() - t0 < TIMEOUT_MS && rxLen < n) {
    while (rs232.available() && rxLen < sizeof(rxBuf) - 1) {
      rxBuf[rxLen++] = (char)rs232.read();
    }
    delay(2);
  }
  rxBuf[rxLen] = '\0';
  rs232.end();

  // STEP 3: Validate received data
  if (rxLen != n) {
    Serial.printf("[rs232] FAIL: expected %u bytes, got %u\n", (unsigned)n, (unsigned)rxLen);
    Serial.println(F("Tip: ensure rs232 TX<->RX loopback plug is installed at DB9."));
    return false;
  }

  if (memcmp(rxBuf, payload, n) != 0) {
    Serial.println(F("[rs232] FAIL: payload mismatch"));
    Serial.print(F("Sent: ")); Serial.print(payload);
    Serial.print(F("Recv: ")); Serial.print(rxBuf);
    return false;
  }

  Serial.print(F("Sent: ")); Serial.print(payload);
  Serial.print(F("Recv: ")); Serial.print(rxBuf);
  Serial.println(F("[rs232] PASS: UART1 loopback OK"));
  return true;
}

static bool runTest_OTA() {
  Serial.println(F("\n[E] ElegantOTA"));

  // OTA server needs Wi-Fi. If neither AP nor STA test has run yet, attempt STA now.
  if (!g_otaServerStarted) {
    Serial.println(F("  Wi-Fi not up. Trying Wi-Fi STA first..."));
    testResults[T_WIFI_STA] = runTest_WifiSTA();

      if (!testResults[T_WIFI_STA]) {
        Serial.println(F(" STA  failed. ElegantOTA test FAIL."));
        return false;
      }
    }
  

  if (!g_littleFsMounted) {
    Serial.println(F("  WARNING: LittleFS not mounted. ElegantOTA may not serve files."));
  }

  IPAddress ip = (WiFi.getMode() == WIFI_AP) ? WiFi.softAPIP() : WiFi.localIP();
  Serial.print(F("  OTA endpoint: http://"));
  Serial.print(ip);
  Serial.println(F("/update"));

  bool ok = promptYesNo(
      F("Open http://<IP>/update in a browser. Do you see the ElegantOTA upload page?"),
      PROMPT_TIMEOUT_MS,
      true);

  Serial.printf("ElegantOTA test: %s\n", ok ? "PASS" : "FAIL");
  return ok;
}

// ============================================================================
// [F] Mute Button + LED Test
// ============================================================================
// Tests S601 mute push button (MUTE_BTN_PIN) and LED_Status toggle.
//
// Hardware: active LOW, external pull-up R603, RC debounce C602 (100 nF).
// OneButton handles press duration:
//   - Too short (< MUTE_DEBOUNCE_MS) : silently ignored, operator retries
//   - Valid click                     : toggles mute state + LED
//   - Too long  (> MUTE_LONGPRESS_MS): prints warning, operator retries
//
// PASS: two valid presses detected, LED ON after first, LED OFF after second.
// ============================================================================
static bool runTest_MuteButton() {
  Serial.println(F("\n[F] Mute Button + LED"));
  Serial.printf("  Button: GPIO %d  |  LED: GPIO %d (LED_Status)\n",
                MUTE_BTN_PIN, LED_Status);

  // Configure OneButton
  muteBtn.setDebounceTicks(MUTE_DEBOUNCE_MS);
  muteBtn.setClickTicks(MUTE_LONGPRESS_MS);
  muteBtn.attachClick([]()         { g_muteBtnClicked     = true; });
  muteBtn.attachLongPressStop([]() { g_muteBtnHeldTooLong = true; });

  pinMode(LED_Status, OUTPUT);
  digitalWrite(LED_Status, LOW);  // start: unmuted, LED off

  Serial.println(F("  Press the mute button twice. (q to quit)"));
  Serial.println(F("  Short tap = ignored (accidental push)."));
  Serial.println(F("  Hold too long = warning, try again."));

  for (int presses = 0; presses < 2; presses++) {

    g_muteBtnClicked     = false;
    g_muteBtnHeldTooLong = false;
    bool gotPress = false;
    uint32_t start = millis();

    Serial.printf("\n  Waiting for press %d of 2...\n", presses + 1);

    while (millis() - start < MUTE_WAIT_MS) {
      muteBtn.tick();

      if (g_muteBtnHeldTooLong) {
        g_muteBtnHeldTooLong = false;
        Serial.println(F("  WARNING: Held too long. Release and press briefly."));
      }

      if (g_muteBtnClicked) {
        g_muteBtnClicked = false;
        gotPress = true;
        bool muted = (presses == 0);
        digitalWrite(LED_Status, muted ? HIGH : LOW);
        Serial.printf("  Press %d detected -> %s\n",
                      presses + 1, muted ? "MUTED   (LED ON)" : "UNMUTED (LED OFF)");
        break;
      }

      if (Serial.available() && up((char)Serial.peek()) == 'Q') {
        (void)Serial.read();
        g_pendingCmd = 'Q';
        digitalWrite(LED_Status, LOW);
        return false;
      }

      delay(5);
    }

    if (!gotPress) {
      Serial.println(F("  FAIL: Timed out waiting for valid press."));
      digitalWrite(LED_Status, LOW);
      return false;
    }
  }

  Serial.println(F("Mute Button + LED test: PASS"));
  return true;
}

// ============================================================================
// Dispatcher / Run All
// ============================================================================

static bool runSingleTestFromIndex(TestIndex idx) {
  switch (idx) {
    case T_POWER: return runTest_Power();
    case T_INPUTS: return runTest_Inputs();
    case T_LCD: return runTest_LCD();
    case T_LEDS: return runTest_LEDs();
    case T_DFPLAYER: return runTest_DFPlayer();
    case T_SD: return runTest_SD();
    case T_Speaker: return runTest_Speaker();
    case T_WIFI_AP: return runTest_WifiAP();
    case T_WIFI_STA: return runTest_WifiSTA();
    case T_LITTLEFS: return runTest_LittleFS();
    case T_UART0: return runTest_UART0();
    case T_SPI: return runTest_SPI();
    case T_RS232: return runTest_RS232();
    case T_OTA: return runTest_OTA();
    case T_MUTE_BTN: return runTest_MuteButton();
    default: return false;
  }
}

static void runAllTests() {
  Serial.println(F("\n[P] Running ALL tests (0 -> F) in order..."));

  for (int i = 0; i < T_COUNT; ++i) {

    if (g_pendingCmd) break;

    testResults[i] = runSingleTestFromIndex(static_cast<TestIndex>(i));
  }

  printSummary();
}

// ============================================================================
// Command handler
// ============================================================================

static void handleCommand(char c) {
  c = up(c);

  bool recognized = true;
  switch (c) {
    case '0': testResults[T_POWER]      = runTest_Power();      break;
    case '1': testResults[T_INPUTS]     = runTest_Inputs();     break;
    case '2': testResults[T_LCD]        = runTest_LCD();        break;
    case '3': testResults[T_LEDS]       = runTest_LEDs();       break;
    case '4': testResults[T_DFPLAYER]   = runTest_DFPlayer();   break;
    case '5': testResults[T_SD]         = runTest_SD();         break;
    case '6': testResults[T_Speaker]    = runTest_Speaker();    break;
    case '7': testResults[T_WIFI_AP]    = runTest_WifiAP();     break;
    case '8': testResults[T_WIFI_STA]   = runTest_WifiSTA();    break;
    case 'A': testResults[T_LITTLEFS]   = runTest_LittleFS();   break;
    case 'B': testResults[T_UART0]      = runTest_UART0();      break;
    case 'C': testResults[T_SPI]        = runTest_SPI();        break;
    case 'D': testResults[T_RS232]      = runTest_RS232();      break;
    case 'E': testResults[T_OTA]        = runTest_OTA();        break;
    case 'F': testResults[T_MUTE_BTN]   = runTest_MuteButton(); break;
    case 'P': runAllTests();                                     break;
    case 'Q':
      Serial.println(F("Returning to menu."));
      printMenu();
      return;
    case 'R':
      Serial.println(F("Rebooting..."));
      delay(200);
      ESP.restart();
      break;
    default:
      recognized = false;
      Serial.println(F("Unknown command."));
      break;
  }

  if (recognized && c != 'P' && c != 'R') printSummary();
  printMenu();
  
}

// ============================================================================
// Arduino entry points
// ============================================================================

void setup() {
  Serial.begin(SERIAL_BAUD);

  // Give USB-CDC time to enumerate; 2 s cap prevents hanging on benchtop power-up
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0 < 2000)) delay(10);

  // Mount LittleFS early so ElegantOTA can serve its web assets immediately
  // if the OTA test is run before a Wi-Fi test. format-on-fail=true handles a
  // blank/corrupted partition without requiring manual intervention.
  g_littleFsMounted = LittleFS.begin(true);
  Serial.println(g_littleFsMounted ? F("[boot] LittleFS... OK") : F("[boot] WARNING: LittleFS mount failed."));

  // Briefly init the Wi-Fi peripheral so MAC address is readable, then disable
  // it — tests 7 and 8 will re-enable it when needed.
  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.println(F("[boot] WiFi init... OK"));
  WiFi.mode(WIFI_OFF);
  delay(50);

  printSplash();
  printSummary();
  printMenu();
  
}

void loop() {
  ElegantOTA.loop();  // must be called regularly to handle OTA transfers
  muteBtn.tick();     // advance OneButton state machine for mute button

  // ---------------------------------------------------------------------------
  // Pending menu command from aborted prompt
  // ---------------------------------------------------------------------------
  if (g_pendingCmd) {
    char c = g_pendingCmd;
    g_pendingCmd = 0;
    Serial.println();
    Serial.print(F("[Pending command executed] "));
    Serial.println(c);
    handleCommand(c);
    return;
  }

  // ---------------------------------------------------------------------------
  // Main menu serial input (fixed buffer, no String)
  // ---------------------------------------------------------------------------
  if (Serial.available()) {

    static char lineBuf[32];   // fixed-size buffer
    static uint8_t idx = 0;    // current write index

    char c = Serial.read();

    // If newline -> process full line
    if (c == '\r' || c == '\n') {

      lineBuf[idx] = '\0';  // null-terminate

      // Single-character command
      if (idx == 1) {
        char cmd = up(lineBuf[0]);
        Serial.println(cmd);
        handleCommand(cmd);
      }

      idx = 0;  // reset buffer
      return;
    }

    // Append character safely
    if (idx < sizeof(lineBuf) - 1) {
      lineBuf[idx++] = c;
  }
}
}
