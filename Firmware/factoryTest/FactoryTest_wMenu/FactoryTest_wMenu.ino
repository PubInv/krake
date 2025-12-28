#define FIRMWARE_VERSION   "v0.4.0"
/*
------------------------------------------------------------------------------
File:            FactoryTest_wMenu.ino
Project:         Krake / GPAD v2 – Factory Test Firmware
Document Type:   Source Code (Factory Test)
Document ID:     KRAKE-FT-ESP32-FT01
Version:         v0.4.0
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
| v0.4.0  | 2025-12-27| N. Kheir      | Cleaned: robust Y/N (Enter optional),           |
|         |           |               | safer Run-All (break on pending command),       |
|         |           |               | clearer I2C reporting, safer DF/SD flow         |
------------------------------------------------------------------------------
Overview:
- Repeatable factory test sequence for ESP32-WROOM-32D Krake/GPAD v2 boards.
- Operator interacts via USB Serial @ 115200 baud.
- Y/N prompts accept Y/N immediately (Enter optional). CR/LF both supported.
- Menu keys (1..8/A..D/P/R) typed at the start of a prompt abort that step
  (marks FAIL) and the command runs next.
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

static const uint32_t SERIAL_BAUD          = 115200;
static const uint32_t PROMPT_TIMEOUT_MS   = 25000;
static const uint32_t WIFI_CONNECT_MS     = 20000;

// If true: during LED test we do NOT drive LAMP2 because it may be DF BUSY
static const bool SKIP_DRIVING_LAMP2 = true;

// LCD expected I2C addresses (common backpacks). Add/remove as needed.
static const uint8_t LCD_ALLOWED_ADDRS[] = { 0x27, 0x3F, 0x38 };

// Enable/disable optional tests
static const bool ENABLE_RS232_TEST = true;

// ============================================================================
// Hardware configuration (ADJUST to match Krake / GPAD v2 wiring)
// ============================================================================

// Lamps / LEDs
const int LED_D9 = 23;    // Mute LED / general status
const int LAMP1  = 15;    // Lamp 1
const int LAMP2  = 4;     // Lamp 2 (often DFPlayer BUSY on some builds)
const int LAMP3  = 5;     // Lamp 3 (also SPI CS)
const int LAMP4  = 18;    // Lamp 4 (also SPI SCK)
const int LAMP5  = 19;    // Lamp 5 (also SPI MISO)

// Rotary encoder (input-only pins OK on ESP32)
const int ENC_CLK = 39;
const int ENC_DT  = 36;
const int ENC_SW  = 34;

// DFPlayer / speaker (UART2)
const int DF_TXD2    = 17;       // ESP32 TX2 -> DFPlayer RX
const int DF_RXD2    = 16;       // ESP32 RX2 <- DFPlayer TX
const int DF_BUSY_IN = LAMP2;    // Active LOW when playing (if wired)

// SPI (VSPI)
const int SPI_MOSI_PIN = LED_D9; // 23
const int SPI_MISO_PIN = LAMP5;  // 19
const int SPI_SCK_PIN  = LAMP4;  // 18
const int SPI_CS_PIN   = LAMP3;  // 5

// RS-232 (UART1) – IMPORTANT: set these to YOUR PCB pins (via MAX3232)
// NOTE: Do NOT set these to GPIO15 if you use LAMP1=15.
const int UART1_TXD1 = 27;       // placeholder safe GPIO
const int UART1_RXD1 = 26;       // placeholder safe GPIO
const long UART1_BAUD = 115200;

// ============================================================================
// DFPlayer & test bookkeeping
// ============================================================================

HardwareSerial dfSerial(2);
DFRobotDFPlayerMini dfPlayer;
bool dfInitialized = false;

enum TestIndex {
  T_POWER = 0,
  T_INPUTS,
  T_LCD,
  T_LEDS,
  T_SD,
  T_DFPLAYER,
  T_WIFI_AP,
  T_WIFI_STA,
  T_LITTLEFS,
  T_UART0,
  T_SPI,
  T_RS232,
  T_COUNT
};

const char* TEST_NAMES[T_COUNT] = {
  "1 Power / ID",
  "2 Inputs (Encoder / Button)",
  "3 LCD (I2C)",
  "4 LEDs / Lamps",
  "5 SD (DFPlayer card)",
  "6 DFPlayer + Speaker",
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

// Read a line with menu-abort: if FIRST char is menu key, abort and queue it.
// Returns true if line ended (CR/LF) or timeout; false if aborted by menu key.
static bool readLineOrMenuAbort(String &out, uint32_t timeoutMs = 15000) {
  out = "";
  uint32_t start = millis();

  while (millis() - start < timeoutMs) {
    while (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') return true;   // accept CR or LF

      if (out.length() == 0 && isMenuKey(c)) {
        g_pendingCmd = up(c);
        return false;
      }

      out += c;
    }
    delay(5);
  }
  return true; // timeout (out may be empty)
}

// Bulletproof Y/N:
// - Accepts Y/N immediately (Enter optional)
// - CR/LF supported
// - If menu key is pressed at the start, aborts step (FAIL) and queues command
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

      // Newline: if buffer has content, evaluate it
      if (c == '\n' || c == '\r') {
        buf.trim();
        if (buf.length() == 0) continue; // ignore empty Enter
        char k = up(buf[0]);
        if (k == 'Y') return true;
        if (k == 'N') return false;
        if (isMenuKey(k)) { g_pendingCmd = k; Serial.println(F("\nAborted -> FAIL.")); return false; }
        Serial.println(F("\nUnknown input -> FAIL"));
        return false;
      }

      // First char menu key => abort now
      if (buf.length() == 0 && isMenuKey(c)) {
        g_pendingCmd = up(c);
        Serial.println(F("\nAborted -> FAIL."));
        return false;
      }

      // Immediate Y/N (no Enter needed)
      char u = up(c);
      if (u == 'Y') { Serial.println(F("Y")); return true; }
      if (u == 'N') { Serial.println(F("N")); return false; }

      // Otherwise buffer
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
  for (int i = 0; i < T_COUNT; ++i) {
    Serial.printf("%-33s : %s\n", TEST_NAMES[i], testResults[i] ? "PASS" : "FAIL");
  }
  Serial.println(F("=========================================="));
  Serial.println();
}

static void printMenu() {
  Serial.println(F("Test menu (order matters):"));
  Serial.println(F(" 1 Power / ID"));
  Serial.println(F(" 2 Inputs (Encoder / Button)"));
  Serial.println(F(" 3 LCD (I2C)"));
  Serial.println(F(" 4 LEDs / Lamps"));
  Serial.println(F(" 5 SD (DFPlayer card)"));
  Serial.println(F(" 6 DFPlayer + Speaker"));
  Serial.println(F(" 7 Wi-Fi AP"));
  Serial.println(F(" 8 Wi-Fi STA (manual SSID/PASS)"));
  Serial.println(F(" A LittleFS R/W"));
  Serial.println(F(" B UART0 (USB Serial)"));
  Serial.println(F(" C SPI loopback"));
  Serial.println(F(" D RS-232 loopback"));
  Serial.println(F(" P Run ALL (1 -> D)"));
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
  for (uint8_t i = 0; i < sizeof(LCD_ALLOWED_ADDRS); ++i) {
    if (LCD_ALLOWED_ADDRS[i] == addr) return true;
  }
  return false;
}

static bool runTest_LCD() {
  Serial.println(F("\n[3] LCD (I2C 20x4)"));

  Wire.begin();
  delay(80);

  // Full scan for operator visibility
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

  // Try to write something; final PASS is operator-confirmed
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

  const int pins[] = { LAMP1, LAMP2, LAMP3, LAMP4, LAMP5, LED_D9 };
  const char* names[] = {
    "LAMP1",
    "LAMP2 (maybe DF BUSY)",
    "LAMP3 (SPI_CS)",
    "LAMP4 (SPI_SCK)",
    "LAMP5 (SPI_MISO)",
    "LED_D9 (SPI_MOSI)"
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

static bool initDFPlayer() {
  if (dfInitialized) return true;

  Serial.println(F("Initializing DFPlayer (UART2)..."));
  pinMode(DF_BUSY_IN, INPUT_PULLUP);

  dfSerial.begin(9600, SERIAL_8N1, DF_RXD2, DF_TXD2);
  delay(400);

  if (!dfPlayer.begin(dfSerial, true, false)) {
    Serial.println(F("  DFPlayer not detected. Check wiring & power."));
    return false;
  }

  dfPlayer.setTimeOut(500);
  dfPlayer.volume(20);

  dfInitialized = true;
  Serial.println(F("  DFPlayer detected and initialized."));
  return true;
}

// SD test is gated: if SD not inserted, we do NOT query DFPlayer for file counts.
static bool runTest_SD() {
  Serial.println(F("\n[5] SD (DFPlayer card)"));

  // Gate: avoids DF queries when SD is absent (and avoids “hang” behavior)
  if (!promptYesNo(F("Is the SD card inserted with audio files?"), PROMPT_TIMEOUT_MS, true)) {
    Serial.println(F("SD test: FAIL (operator says SD not inserted)."));
    return false;
  }

  if (!initDFPlayer()) {
    Serial.println(F("SD test FAIL (no DFPlayer)."));
    return false;
  }

  Serial.println(F("Querying SD file count..."));
  int fileCount = dfPlayer.readFileCounts();
  Serial.printf("  DFPlayer reports %d files.\n", fileCount);

  if (fileCount < 0) {
    Serial.println(F("SD test FAIL: could not read SD/file index."));
    Serial.println(F("Check: FAT32 format, good contacts, stable power."));
    return false;
  }

  bool pass = (fileCount > 0);
  Serial.printf("SD test: %s\n", pass ? "PASS" : "FAIL (no files on SD)");
  return pass;
}

static bool runTest_DFPlayer() {
  Serial.println(F("\n[6] DFPlayer + Speaker"));

  if (!initDFPlayer()) {
    Serial.println(F("DFPlayer test FAIL (no DFPlayer)."));
    return false;
  }

  // Must have SD + files (otherwise playing is pointless / can misbehave)
  int fileCount = dfPlayer.readFileCounts();
  Serial.printf("  SD file count = %d\n", fileCount);
  if (fileCount <= 0) {
    Serial.println(F("No readable SD/files -> NOT playing audio. DFPlayer test FAIL."));
    return false;
  }

  Serial.println(F("Playing track #1 for ~3 seconds..."));
  dfPlayer.play(1);

  unsigned long start = millis();
  while (millis() - start < 3000UL) {
    if (((millis() - start) % 250) < 10) Serial.print('.');
    delay(10);
  }
  Serial.println();

  dfPlayer.stop();
  delay(200);

  bool ok = promptYesNo(F("Did you hear audio from the speaker?"), PROMPT_TIMEOUT_MS, true);
  Serial.printf("DFPlayer/Speaker test: %s\n", ok ? "PASS" : "FAIL");
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

  flushSerialRx(); // only here (line entry)

  Serial.println(F("Enter Wi-Fi SSID then press Enter (15 s timeout):"));
  Serial.print(F("> "));
  String ssid;
  if (!readLineOrMenuAbort(ssid, 15000)) {
    Serial.println(F("\nAborted by menu key."));
    return false;
  }
  ssid.trim();
  if (ssid.length() == 0) {
    Serial.println(F("No SSID entered. Wi-Fi STA test FAIL."));
    return false;
  }

  Serial.println(F("Enter Wi-Fi PASSWORD then press Enter (can be empty):"));
  Serial.print(F("> "));
  String pass;
  if (!readLineOrMenuAbort(pass, 15000)) {
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
  digitalWrite(SPI_CS_PIN, LOW);

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  uint8_t out = 0xA5;
  uint8_t in  = SPI.transfer(out);
  SPI.endTransaction();
  digitalWrite(SPI_CS_PIN, HIGH);

  Serial.printf("SPI sent 0x%02X, received 0x%02X\n", out, in);
  bool ok = (in == out);
  Serial.printf("SPI loopback test: %s\n", ok ? "PASS" : "FAIL (check jumper)");
  return ok;
}

static bool runTest_RS232() {
  if (!ENABLE_RS232_TEST) {
    Serial.println(F("\n[D] RS-232 loopback disabled (ENABLE_RS232_TEST=false)"));
    return false;
  }

  Serial.println(F("\n[D] RS-232 loopback (UART1)"));
  Serial.println(F("Connect RS-232 TX <-> RX at DB9 (pins 2 and 3) via MAX3232."));
  Serial.printf("Using UART1 TXD=%d, RXD=%d, BAUD=%ld\n", UART1_TXD1, UART1_RXD1, UART1_BAUD);

  HardwareSerial rs232(1);
  rs232.begin(UART1_BAUD, SERIAL_8N1, UART1_RXD1, UART1_TXD1);
  delay(150);

  const uint8_t pattern = 0x55;
  unsigned long start = millis();
  bool ok = false;

  while (millis() - start < 3000UL) {
    rs232.write(pattern);
    rs232.flush();
    delay(20);
    if (rs232.available()) {
      int b = rs232.read();
      if (b == pattern) { ok = true; break; }
    }
  }

  rs232.end();
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
    case T_SD:         return runTest_SD();
    case T_DFPLAYER:   return runTest_DFPlayer();
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
    // If operator pressed a menu key during any prompt, stop Run-All and execute it
    if (g_pendingCmd) break;

    // If SD failed, skip DFPlayer play test
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
    case '1': testResults[T_POWER]      = runTest_Power();      break;
    case '2': testResults[T_INPUTS]     = runTest_Inputs();     break;
    case '3': testResults[T_LCD]        = runTest_LCD();        break;
    case '4': testResults[T_LEDS]       = runTest_LEDs();       break;
    case '5': testResults[T_SD]         = runTest_SD();         break;
    case '6': testResults[T_DFPLAYER]   = runTest_DFPlayer();   break;
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
 
