#define FIRMWARE_VERSION   "v0.4.2.3"
/*
------------------------------------------------------------------------------
File:            FactoryTest_wMenu.ino
Project:         Krake / GPAD v2 – Factory Test Firmware
Document Type:   Source Code (Factory Test)
Document ID:     KRAKE-FT-ESP32-FT01
Version:         v0.4.1
Date:            2025-12-27
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
| v0.4.2  | 2026-1-1  | N. Kheir      | SPI backloop working and final. DFP debug       |
|v0.4.2.1 | 2026-1-1  | N. Kheir      | DFplayer cleanup test.                          |
|v0.4.2.2 | 2026-1-1  | N. Kheir      | SPI cleanup test.                               |
|v0.4.2.3 | 2026-1-5  | L. Erickson   | Make menu two colums.                           |
----------------------------------------------------------------------------------------|
Overview:
- Repeatable factory test sequence for ESP32-WROOM-32D Krake/GPAD v2 boards.
- Operator interacts via USB Serial @ 115200 baud.
- Y/N prompts accept Y/N immediately (Enter optional). CR/LF supported.
- During prompts, menu keys typed as the FIRST character abort that step
  (marks FAIL) and the command runs next.
- WiFi SSID/PASS input does NOT abort just because SSID starts with A/B/C/D.
  It aborts only when the entire line is exactly one menu key (e.g., "C"+Enter).
------------------------------------------------------------------------------
Build notes:
- Adjust pin mapping to match your PCB (especially UART1 pins).
- If LAMP2 shares DFPlayer BUSY, we skip driving LAMP2 in the LED test.
------------------------------------------------------------------------------
*/

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <FS.h>
#include <LittleFS.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobotDFPlayerMini.h>

// ============================================================================
// Configuration
// ============================================================================

static const uint32_t SERIAL_BAUD       = 115200;
static const uint32_t PROMPT_TIMEOUT_MS = 65000;
static const uint32_t WIFI_CONNECT_MS   = 20000;

static const bool SKIP_DRIVING_LAMP2 = true;  // shared with DF BUSY on some builds
static const uint8_t LCD_ALLOWED_ADDRS[] = { 0x27, 0x3F, 0x38 };
static const bool ENABLE_RS232_TEST = true;

// ============================================================================
// Hardware configuration (ADJUST to match Krake / GPAD v2 wiring)
// ============================================================================

// Lamps / LEDs
const int LED_Status = 13;    // Mute LED / general status
const int LAMP1  = 12;    // Lamp 1
const int LAMP2  = 14;     // Lamp 2 (often DFPlayer BUSY on some builds)
const int LAMP3  = 27;     // Lamp 3 (also SPI CS)
const int LAMP4  = 26;    // Lamp 4 (also SPI SCK)
const int LAMP5  = 25;    // Lamp 5 (also SPI MISO)

// Rotary encoder (input-only pins OK on ESP32)
const int ENC_CLK = 39;
const int ENC_DT  = 36;
const int ENC_SW  = 34;

// DFPlayer / speaker (UART2)
const int DF_TXD2    = 17;       // ESP32 TX2 -> DFPlayer RX
const int DF_RXD2    = 16;       // ESP32 RX2 <- DFPlayer TX
const int DF_BUSY_IN = LAMP2;    // Active LOW when playing (if wired)

// SPI (VSPI)
const int SPI_MOSI_PIN = 23 ;
const int SPI_MISO_PIN = 19 ;
const int SPI_SCK_PIN  = 18 ;
const int SPI_CS_PIN   = 5 ;
// ===== TEST PARAMETERS =====
static const uint32_t SPI_SPEED = 1000000; // 1 MHz (safe for factory)
static const uint8_t TEST_PATTERN[] = {
  0x55, 0xAA, 0x00, 0xFF, 0x12, 0x34, 0xA5
};

// RS-232 (UART1) – IMPORTANT: set these to YOUR PCB pins (via MAX3232)
const int  UART1_TXD1 = 2;       // placeholder safe GPIO
const int  UART1_RXD1 = 15;       // placeholder safe GPIO
const long UART1_BAUD = 115200;
static const uint32_t TIMEOUT_MS = 600; 
 
// DFPlayer bookkeeping + SD cache
HardwareSerial dfSerial(2);
DFRobotDFPlayerMini dfPlayer;
enum dfState { DF_UNKNOWN, DF_OK, DF_FAIL };
static bool dfState = DF_UNKNOWN;
// SD cache: read file count ONCE during test [5], reuse during test [6]
static bool g_sdChecked   = false;
static int  g_sdFileCount = -999;

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
  "D RS-232 loopback"
};

bool testResults[T_COUNT] = { false };

// ============================================================================
// Serial helpers + pending command capture
// ============================================================================

static char g_pendingCmd = 0;

static char up(char c) { return (char)toupper((unsigned char)c); }

static bool isMenuKey(char c) {
  c = up(c);
  return (c >= '1' && c <= '8') || (c >= 'A' && c <= 'D') || c == 'P' || c == 'R';
}

static void flushSerialRx() {
  while (Serial.available()) (void)Serial.read();
}

 
static bool readLineOrMenuAbort(String &out, uint32_t timeoutMs = 15000) {
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
          return false; // aborted
        }

        return true; // completed normally
      }

      out += c;
    }
    delay(5);
  }

  out.trim();
  return true; // timeout (out may be empty)
}

 
static bool promptYesNo(const __FlashStringHelper* question,
                        uint32_t timeoutMs = PROMPT_TIMEOUT_MS,
                        bool defaultNo = true) {
  Serial.println(question);
  Serial.println(F("Press Y to PASS or N to FAIL (Enter optional)."));
  Serial.println(F("(Menu keys abort this step and run next.)"));
  Serial.print(F("> "));

  uint32_t start = millis();
  String buf;

  while (millis() - start < timeoutMs) {
    while (Serial.available()) {
      char c = Serial.read();

      if (c == '\n' || c == '\r') {
        buf.trim();
        if (buf.length() == 0) continue;

        char k = up(buf[0]);
        if (k == 'Y') return true;
        if (k == 'N') return false;
        if (isMenuKey(k)) { g_pendingCmd = k; Serial.println(F("\nAborted -> FAIL.")); return false; }

        Serial.println(F("\nUnknown input -> FAIL"));
        return false;
      }

      if (buf.length() == 0 && isMenuKey(c)) {
        g_pendingCmd = up(c);
        Serial.println(F("\nAborted -> FAIL."));
        return false;
      }

      char u = up(c);
      if (u == 'Y') { Serial.println(F("Y")); return true; }
      if (u == 'N') { Serial.println(F("N")); return false; }

      buf += c;
    }
    delay(5);
  }

  Serial.println(defaultNo ? F("\nNo response -> FAIL") : F("\nNo response -> PASS"));
  return !defaultNo;
}

// ============================================================================
// UI
// ============================================================================

static void printBanner() {
  Serial.println();
  Serial.println(F("======================================="));
  Serial.println(F("   KRAKE FACTORY TEST - ESP32-WROOM   "));
  Serial.print("Firmware Version: ");
  Serial.println(F(FIRMWARE_VERSION));
  Serial.println(F("======================================="));
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier
   Serial.printf("Chip revision: %d\n", ESP.getChipRevision());
  Serial.printf("Flash size: %u bytes\n", ESP.getFlashChipSize());
  Serial.print(F("MAC (STA): "));
  Serial.println(WiFi.macAddress());
  Serial.println();
}

static void printSummary() {
  Serial.println();
  Serial.println(F("========== FACTORY TEST SUMMARY =========="));

  for (int i = 0; i < T_COUNT; i += 2) {

    // Left column
    Serial.printf(
      "%-33s : %-4s",
      TEST_NAMES[i],
      testResults[i] ? "PASS" : "FAIL"
    );

    // Right column (if present)
    if (i + 1 < T_COUNT) {
      Serial.printf(
        "    %-33s : %-4s",
        TEST_NAMES[i + 1],
        testResults[i + 1] ? "PASS" : "FAIL"
      );
    }

    Serial.println();
  }

  Serial.println(F("=========================================="));
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
Serial.println(F(" D RS-232 loopback               P Run ALL (1 -> D)"));
Serial.println(F(" R Reboot"));
Serial.println();
Serial.print(F("Enter command: "));

}

// ============================================================================
// Tests
// ============================================================================

static bool runTest_Power() {
  Serial.println(F("\n[1] Power / ID"));
  printBanner();
  Serial.println(F("Power OK: MCU running and Serial is active."));
  return true;
}

static bool runTest_Inputs() {
  Serial.println(F("\n[2] Inputs (Encoder / Button)"));

  pinMode(ENC_CLK, INPUT);
  pinMode(ENC_DT,  INPUT);
  pinMode(ENC_SW,  INPUT_PULLUP);

  Serial.println(F("Rotate encoder CLOCKWISE, then COUNTER-CLOCKWISE,"));
  Serial.println(F("then PRESS the encoder button within 10 seconds."));
  delay(200);

  int  lastCLK  = digitalRead(ENC_CLK);
  bool sawCW    = false;
  bool sawCCW   = false;
  bool sawPress = false;

  unsigned long start = millis();
  while (millis() - start < 10000UL) {
    int clk = digitalRead(ENC_CLK);
    if (clk != lastCLK) {
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

static bool runTest_LCD() {
  Serial.println(F("\n[3] LCD (I2C 20x4)"));

  Wire.begin();
  delay(80);

  Serial.println(F("Scanning I2C bus (show all devices)..."));
  bool any = false;
  uint8_t firstAllowed = 0;

  for (uint8_t addr = 1; addr < 127; ++addr) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      any = true;
      Serial.printf("  I2C device at 0x%02X%s\n", addr, addrInAllowList(addr) ? " (allowed LCD addr)" : "");
      if (!firstAllowed && addrInAllowList(addr)) firstAllowed = addr;
    }
  }

  if (!any) {
    Serial.println(F("No I2C devices found. LCD test FAIL."));
    return false;
  }
  if (!firstAllowed) {
    Serial.println(F("No LCD found at expected addresses (0x27/0x3F/0x38). LCD test FAIL."));
    return false;
  }

  LiquidCrystal_I2C lcd(firstAllowed, 20, 4);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0); lcd.print("KRAKE FACTORY TEST");
  lcd.setCursor(0, 1); lcd.print("LCD @ 0x"); lcd.print(firstAllowed, HEX);
  lcd.setCursor(0, 2); lcd.print("Confirm display OK");
  lcd.setCursor(0, 3); lcd.print("Press Y/N on Serial");

  Serial.println(F("LCD initialized and message written."));
  bool ok = promptYesNo(F("Visually confirm LCD shows the message on all lines."), PROMPT_TIMEOUT_MS, true);
  Serial.printf("LCD test: %s\n", ok ? "PASS" : "FAIL");
  return ok;
}

static bool runTest_LEDs() {
  Serial.println(F("\n[4] LEDs / Lamps"));

  const int pins[] = { LAMP1, LAMP2, LAMP3, LAMP4, LAMP5, LED_Status };
  const char* names[] = {
    "LAMP1",
    "LAMP2 (LAMP2)",
    "LAMP3 (LAMP3)",
    "LAMP4 (LAMP4)",
    "LAMP5 (LAMP5)",
    "LED_Status (LED_Status)"
  };

  for (int i = 0; i < 6; ++i) {
    if (SKIP_DRIVING_LAMP2 && pins[i] == LAMP2) continue;
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
  delay(50);

  for (int i = 0; i < 6; ++i) {
    if (SKIP_DRIVING_LAMP2 && pins[i] == LAMP2) {
      Serial.println(F("  -> Skipping LAMP2 drive (BUSY shared safety)"));
      continue;
    }
    Serial.print(F("  -> ")); Serial.print(names[i]); Serial.println(F(" blink"));
    digitalWrite(pins[i], HIGH); delay(300);
    digitalWrite(pins[i], LOW);  delay(150);
  }

  bool ok = promptYesNo(F("Did you see the LEDs/Lamps blink as expected?"), PROMPT_TIMEOUT_MS, true);
  Serial.printf("LEDs/Lamps test: %s\n", ok ? "PASS" : "FAIL");
  return ok;
}


static void clearDFPlayerCache() {
  dfState = DF_UNKNOWN;
  // optional: hard reset the UART session as well
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

static bool initDFPlayer() {

  if (dfState == DF_OK) {
    if (dfPlayerResponding()) return true;

    Serial.println(F("  DFPlayer was OK but no reply now -> clearing cache & re-init..."));
    clearDFPlayerCache();
  }

  if (dfState == DF_FAIL) return false;

  Serial.println(F("Initializing DFPlayer (UART2)..."));
  pinMode(DF_BUSY_IN, INPUT_PULLUP);

  dfSerial.begin(9600, SERIAL_8N1, DF_RXD2, DF_TXD2);
  delay(300);

  // doReset=true helps a LOT with SD indexing reliability
  if (!dfPlayer.begin(dfSerial, true, true)) {
    Serial.println(F("  DFPlayer not detected. Check wiring & power."));
    dfState = DF_FAIL;
    return false;
  }

  dfPlayer.setTimeOut(1000);     // give replies more time (some modules are slow)

  // Force device selection (important on some clones)
  dfPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  delay(1200);                   // let TF card mount + index after reset/device select

  // Optional: verify it actually replies after init
  if (!dfPlayerResponding()) {
    Serial.println(F("  DFPlayer init done, but still no reply -> FAIL"));
    dfState = DF_FAIL;
    return false;
  }

  dfState = DF_OK;
  Serial.println(F("  DFPlayer detected and initialized (SD selected)."));
  return true;
}


//  static bool initDFPlayer() {
//   if (dfState == DF_OK)   return true;
//   if (dfState == DF_FAIL) return false;

//   Serial.println(F("Initializing DFPlayer (UART2)..."));
//   pinMode(DF_BUSY_IN, INPUT_PULLUP);

//   dfSerial.begin(9600, SERIAL_8N1, DF_RXD2, DF_TXD2);
//   delay(300);

//   // doReset=true helps a LOT with SD indexing reliability
//   if (!dfPlayer.begin(dfSerial, true, true)) {
//     Serial.println(F("  DFPlayer not detected. Check wiring & power."));
//     dfState = DF_FAIL;
//     return false;
//   }else {

//    dfState = DF_OK;
//    Serial.println(F("  DFPlayer detected and initialized (SD selected)."));
//   }
  
//   dfPlayer.setTimeOut(1000);     // give replies more time (some modules are slow)
//   return true;
// }


// Put this OUTSIDE of runTest_DFPlayer() (global scope).
// Call it when dfPlayer.available() is true.
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
  // If you want live DFPlayer diagnostics, call this elsewhere during waits:
  // if (dfPlayer.available()) printDetail(dfPlayer.readType(), dfPlayer.read());
   if (forceReinit) {
    clearDFPlayerCache();
  }

   if (!initDFPlayer()) {
  Serial.println(F(" DFPlayer initializing Failed (DFPlayer not detected)."));
  dfState = DF_FAIL;
  return false;
  }else {
  // Serial.println(F("\n[6] DFPlayer + Speaker"));
  Serial.println(F("DFPlayer Mini online."));
  dfState = DF_OK;
  }
  return true;
  // Force device selection (important on some clones)
  dfPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  delay(1200);                   // let TF card mount + index after reset/device select

  bool ok = (dfState);
  Serial.printf("DFPLAYER Initialized: %s\n", ok ? "PASS" : "FAIL (wiring/MAX3232/pins)");
  return ok;
}

static bool runTest_SD() {
  Serial.println(F("\n[5] SD (DFPlayer card)"));

  // reset SD cache each time test [5] runs (so it stays consistent per run)
  // g_sdChecked   = false;
  // g_sdFileCount = -999;

  g_sdChecked   = true;
  g_sdFileCount = -1;

  // if (!promptYesNo(F("Is the SD card inserted with audio files?"), PROMPT_TIMEOUT_MS, true)) {
  //   Serial.println(F("SD test: FAIL (operator says SD not inserted)."));
  //   g_sdChecked   = true;
  //   g_sdFileCount = -1;
  //   return false;
  // }

  if (!initDFPlayer()) {
    Serial.println(F("SD test: FAIL (DFPlayer not detected)."));
    g_sdChecked   = true;
    g_sdFileCount = -1;
    return false;
  }

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

  bool ok = runTest_SD();
  Serial.printf("SD test: %s\n", ok ? "PASS" : "FAIL");
  
    void printDetail(uint8_t type, int value);
 
}

static bool runTest_Speaker() {
  Serial.println(F("Playing track #1 for ~3 seconds..."));

  if (!initDFPlayer()) {
    Serial.println(F("Speaker test: FAIL (DFPlayer not detected)."));
    return false;
  }

  if (!g_sdChecked) {
    Serial.println(F("DFPlayer test requires SD test first."));
    Serial.println(F("Run [5] SD test, then run speaker test."));
    return false;
  }

  Serial.printf("  Using cached SD file count = %d\n", g_sdFileCount);
  if (g_sdFileCount <= 0) {
    Serial.println(F("No readable SD/files -> NOT playing audio. Speaker test FAIL."));
    return false;
  }
  const int VOL = 20;
  dfPlayer.volume(VOL);
  Serial.printf("DFPlayer volume set to: %d\n", VOL);

  dfPlayer.play(1);

  uint32_t start = millis();
  while (millis() - start < 3000) {
    // Optional: report DFPlayer events while playing
    if (dfPlayer.available()) {
      printDetail(dfPlayer.readType(), dfPlayer.read());
    }
    delay(10);
  }

  dfPlayer.stop();
  delay(200);

  bool ok = promptYesNo(F("Did you hear audio from the speaker?"), PROMPT_TIMEOUT_MS, true);
  Serial.printf("Speaker test: %s\n", ok ? "PASS" : "FAIL");
  return ok;
}

static bool runTest_WifiAP() {
  Serial.println(F("\n[7] Wi-Fi AP"));

  WiFi.mode(WIFI_AP);
  String mac = WiFi.macAddress(); mac.replace(":", "");
  String ssid = "KRAKE_" + mac;

  bool ok = WiFi.softAP(ssid.c_str(), "krakefactory");
  if (!ok) {
    Serial.println(F("Failed to start Wi-Fi AP."));
    return false;
  }

  IPAddress ip = WiFi.softAPIP();
  Serial.print(F("AP SSID: ")); Serial.println(ssid);
  Serial.print(F("AP IP:   ")); Serial.println(ip);
  Serial.println(F("Operator: verify AP is visible from phone/PC."));
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

  Serial.print(F("Connecting to: ")); Serial.println(ssid);
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
  Serial.print(F("IP:   ")); Serial.println(WiFi.localIP());
  Serial.print(F("RSSI: ")); Serial.println(WiFi.RSSI());
  return true;
}

static bool runTest_LittleFS() {
  Serial.println(F("\n[A] LittleFS R/W"));

  if (!LittleFS.begin(true)) {
    Serial.println(F("LittleFS mount FAILED."));
    return false;
  }

  const char* path    = "/factory_test.txt";
  const char* payload = "KRAKE_FACTORY_TEST";

  Serial.print(F("Writing ")); Serial.println(path);
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
 
  Serial.printf("SPI sent 0x%02X, received 0x%02X\n", RX, TX);
  bool ok = runTest_UART0();
  Serial.printf("SPI loopback test: %s\n", ok ? "PASS" : "FAIL (check jumper)");
  return ok;
}

static bool runTest_RS232() {

  if (!ENABLE_RS232_TEST) {
    Serial.println(F("\n[D] RS-232 loopback disabled (ENABLE_RS232_TEST=false)"));
    return false;
  }

  Serial.println(F("\n[D] RS-232 loopback (UART1)"));
  Serial.println(F("Connected RS-232 TX <-> RX at DB9 (pins 2 and 3) via MAX3232."));
  Serial.printf("Using UART1 TXD=%d, RXD=%d, BAUD=%ld\n", UART1_TXD1, UART1_RXD1, UART1_BAUD);

  HardwareSerial rs232(1);
  rs232.begin(UART1_BAUD, SERIAL_8N1, UART1_RXD1, UART1_TXD1);
  delay(150);
  // Clear any pending garbage
  while (rs232.available()) rs232.read();

  const char *payload = "KRAKE_rs232_UART1_LOOPBACK_123\r\n";
  const size_t n = strlen(payload);

  // Send
  size_t written = rs232.write((const uint8_t*)payload, n);
  rs232.flush();
  if (written != n) {
    Serial.println("[rs232] FAIL: write length mismatch");
    rs232.end();
    return false;
  }

  // Receive
  String rx;
  uint32_t t0 = millis();
  while (millis() - t0 < TIMEOUT_MS && rx.length() < n) {
    while (rs232.available()) {
      char c = (char)rs232.read();
      rx += c;
    }
    delay(2);
  }
  rs232.end();

  // Validate
  if (rx.length() != n) {
    Serial.printf("[rs232] FAIL: expected %u bytes, got %u\n",
                  (unsigned)n, (unsigned)rx.length());
    Serial.println("Tip: ensure rs232 TX<->RX loopback plug is installed.");
    return false;
  }

  if (rx != payload) {
    Serial.println("[rs232] FAIL: payload mismatch");
    Serial.print("Sent: "); Serial.print(payload);
    Serial.print("Recv: "); Serial.print(rx);
    return false;
  }

  Serial.print("Sent: "); Serial.print(payload);
  Serial.print("Recv: "); Serial.print(rx);
  Serial.println("[rs232] PASS: UART1 loopback OK");
  return true;
 
  bool ok = (rx == payload) ;
  Serial.printf("RS-232 loopback test: %s\n", ok ? "PASS" : "FAIL (wiring/MAX3232/pins)");
  return ok;
}

// ============================================================================
// Dispatcher / Run All
// ============================================================================

static bool runSingleTestFromIndex(TestIndex idx) {
  switch (idx) {
    case T_POWER:      return runTest_Power();
    case T_INPUTS:     return runTest_Inputs();
    case T_LCD:        return runTest_LCD();
    case T_LEDS:       return runTest_LEDs();
    case T_DFPLAYER:   return runTest_DFPlayer();
    case T_SD:         return runTest_SD();
    case T_Speaker:    return runTest_Speaker();
    case T_WIFI_AP:    return runTest_WifiAP();
    case T_WIFI_STA:   return runTest_WifiSTA();
    case T_LITTLEFS:   return runTest_LittleFS();
    case T_UART0:      return runTest_UART0();
    case T_SPI:        return runTest_SPI();
    case T_RS232:      return runTest_RS232();
    default:           return false;
  }
}

static void runAllTests() {
  Serial.println(F("\n[P] Running ALL tests (1 -> D) in order..."));

  for (int i = 0; i < T_COUNT; ++i) {
    if (g_pendingCmd) break;

    // skip audio play if SD failed
    if (i == T_DFPLAYER && testResults[T_SD] == false) {
      Serial.println(F("\n[6] DFPlayer + Speaker"));
      Serial.println(F("Skipping audio play because SD test FAILED."));
      testResults[T_DFPLAYER] = false;
      continue;
    }

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
    case 'P': runAllTests();                                     break;
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

  // Do not block forever waiting for Serial
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0 < 2000)) delay(10);

  WiFi.mode(WIFI_OFF);
  delay(50);

  printBanner();
  printSummary();
  printMenu();
}

void loop() {
  // If a menu key was pressed during a prompt, execute it now.
  if (g_pendingCmd) {
    char c = g_pendingCmd;
    g_pendingCmd = 0;
    Serial.println();
    Serial.print(F("[Pending command executed] "));
    Serial.println(c);
    handleCommand(c);
    return;
  }

  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\r' || c == '\n') return;

    Serial.println(up(c)); // echo
    handleCommand(c);
  }
}