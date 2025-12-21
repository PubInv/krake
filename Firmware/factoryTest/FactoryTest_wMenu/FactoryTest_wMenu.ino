/*
------------------------------------------------------------------------------
File:            FactoryTest_Krake01.ino
Project:         Krake / GPAD v2 – Factory Test Firmware
Document Type:   Source Code (Factory Test)
Document ID:     KRAKE-FT-ESP32-FT01
Version:         v0.1.0
Date:            2025-12-03
Author(s):       Nagham Kheir, Public Invention
Status:          Draft
------------------------------------------------------------------------------
Revision History:
| Version |   Date    | Author        | Description                         |
|---------|-----------|---------------|-------------------------------------|
| v0.1.0  | 2025-12-03| N. Kheir      | Initial unified factory test code   |
------------------------------------------------------------------------------
Overview:
- Executes a repeatable factory test sequence for ESP32-WROOM-32D Krake/GPAD v2
  boards, validating:
    1) Power / ID
    2) Inputs (rotary encoder + push button)
    3) LCD (I²C, 20x4)
    4) LEDs / Lamps
    5) SD card via DFPlayer
    6) DFPlayer + Speaker (operator confirmation)
    7) Wi-Fi AP
    8) Wi-Fi STA (SSID & PASS entered via Serial console)
    A) LittleFS Read/Write
    B) UART0 (USB Serial, operator confirmation)
    C) SPI loopback (MOSI <-> MISO jumper)
    D) RS-232 loopback (TX <-> RX on DB9, via MAX3232)
- Operator interacts via USB Serial at 115200 baud.
- Intended for FACTORY USE ONLY (not for field deployment).
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

// -----------------------------------------------------------------------------
// Hardware configuration (ADJUST to match Krake / GPAD v2 wiring)
// -----------------------------------------------------------------------------

// Lamps / LEDs
const int LED_D9 = 23;    // Mute LED on PMD / general status
const int LAMP1  = 15;    // Lamp 1  (e.g. cold food)
const int LAMP2  = 4;     // Lamp 2  (also DFPlayer BUSY input)
const int LAMP3  = 5;     // Lamp 3  (also SPI CS)
const int LAMP4  = 18;    // Lamp 4  (also SPI SCK)
const int LAMP5  = 19;    // Lamp 5  (also SPI MISO)

// Rotary encoder (input)
const int ENC_CLK = 39;
const int ENC_DT  = 36;
const int ENC_SW  = 34;

// DFPlayer / speaker (UART2)
const int DF_TXD2    = 17;       // ESP32 TX2 -> DFPlayer RX
const int DF_RXD2    = 16;       // ESP32 RX2 <- DFPlayer TX
const int DF_BUSY_IN = LAMP2;    //     Shared with LAMP2, active LOW when playing

// RS-232 (UART1) – ADJUST TO YOUR BOARD (pins via MAX3232 to DB9)
const int UART1_TXD1 = 2;        // ESP32 TX1 -> MAX3232 -> DB9
const int UART1_RXD1 = 15;       // ESP32 RX1 <- MAX3232 <- DB9
const long UART1_BAUD = 115200;

// SPI (VSPI) – shared with lamps on Krake header
const int SPI_MOSI_PIN = LED_D9; // 23
const int SPI_MISO_PIN = LAMP5;  // 19
const int SPI_SCK_PIN  = LAMP4;  // 18
const int SPI_CS_PIN   = LAMP3;  // 5

// -----------------------------------------------------------------------------
// DFPlayer & test bookkeeping
// -----------------------------------------------------------------------------

HardwareSerial dfSerial(2);
DFRobotDFPlayerMini dfPlayer;
bool dfInitialized = false;

// Test indices (keep order as actual test order)
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

// -----------------------------------------------------------------------------
// Utility
// -----------------------------------------------------------------------------

// Read one line from Serial with timeout (for SSID/PASS)
String readLine(uint32_t timeoutMs = 15000) {
  String s;
  uint32_t start = millis();
  while (millis() - start < timeoutMs) {
    while (Serial.available()) {
      char c = Serial.read();
      if (c == '\r') continue;
      if (c == '\n') return s;
      s += c;
    }
  }
  return s;
}

void printBanner() {
  Serial.println();
  Serial.println(F("======================================="));
  Serial.println(F("   KRAKE FACTORY TEST - ESP32-WROOM   "));
  Serial.println(F("======================================="));
  Serial.printf("Chip revision: %d\n", ESP.getChipRevision());
  Serial.printf("Flash size: %u bytes\n", ESP.getFlashChipSize());
  Serial.print(F("MAC (STA): "));
  Serial.println(WiFi.macAddress());
  Serial.println();
}

void printSummary() {
  Serial.println();
  Serial.println(F("========== FACTORY TEST SUMMARY =========="));
  for (int i = 0; i < T_COUNT; ++i) {
    Serial.printf("%-33s : %s\n",
                  TEST_NAMES[i],
                  testResults[i] ? "PASS" : "FAIL");
  }
  Serial.println(F("=========================================="));
  Serial.println();
}

void printMenu() {
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

// -----------------------------------------------------------------------------
// Individual tests
// -----------------------------------------------------------------------------

// 1) Power / ID
bool runTest_Power() {
  Serial.println(F("\n[1] Power / ID"));
  printBanner();
  Serial.println(F("Power OK: MCU running and Serial is active."));
  return true;
}

// 2) Inputs (Encoder / Button)
bool runTest_Inputs() {
  Serial.println(F("\n[2] Inputs (Encoder / Button)"));
  pinMode(ENC_CLK, INPUT);
  pinMode(ENC_DT,  INPUT);
  pinMode(ENC_SW,  INPUT_PULLUP);

  Serial.println(F("Rotate encoder CLOCKWISE, then COUNTER-CLOCKWISE,"));
  Serial.println(F("then PRESS the encoder button within 10 seconds."));
  delay(500);

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
    }

    if (sawCW && sawCCW && sawPress) break;
  }

  bool pass = sawCW && sawCCW && sawPress;
  Serial.printf("Inputs test: %s\n",
                pass ? "PASS" : "FAIL (missing rotation and/or press)");
  return pass;
}

// 3) LCD (I2C)
bool runTest_LCD() {
  Serial.println(F("\n[3] LCD (I2C 20x4)"));

  Wire.begin();
  delay(100);

  uint8_t foundAddr = 0;
  Serial.println(F("Scanning I2C bus..."));
  for (uint8_t addr = 1; addr < 127; ++addr) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  Found I2C device at 0x%02X\n", addr);
      if (!foundAddr) foundAddr = addr;
    }
  }

  if (!foundAddr) {
    Serial.println(F("No I2C devices found. LCD test FAIL."));
    return false;
  }

  LiquidCrystal_I2C lcd(foundAddr, 20, 4);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("KRAKE FACTORY TEST");
  lcd.setCursor(0, 1);
  lcd.print("LCD OK @ 0x");
  lcd.print(foundAddr, HEX);
  lcd.setCursor(0, 2);
  lcd.print("Check all 4 lines.");
  lcd.setCursor(0, 3);
  lcd.print("SN: ");
  lcd.print(WiFi.macAddress());

  Serial.println(F("LCD initialized and test message written."));
  Serial.println(F("Operator: visually confirm 20x4 LCD content."));
  return true;
}

// 4) LEDs / Lamps
bool runTest_LEDs() {
  Serial.println(F("\n[4] LEDs / Lamps"));
  int pins[] = { LAMP1, LAMP2, LAMP3, LAMP4, LAMP5, LED_D9 };
  const char* names[] = {
    "LAMP1",
    "LAMP2 / DF_BUSY",
    "LAMP3 / SPI_CS",
    "LAMP4 / SPI_SCK",
    "LAMP5 / SPI_MISO",
    "LED_D9 / SPI_MOSI"
  };

  for (int i = 0; i < 6; ++i) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
  delay(50);

  for (int i = 0; i < 6; ++i) {
    Serial.print(F("  -> "));
    Serial.print(names[i]);
    Serial.println(F(" ON"));
    digitalWrite(pins[i], HIGH);
    delay(300);
    digitalWrite(pins[i], LOW);
    delay(100);
  }

  Serial.println(F("Operator: visually confirm each lamp/LED toggled ON/OFF."));
  return true;
}

// DFPlayer init helper
bool initDFPlayer() {
  if (dfInitialized) return true;

  Serial.println(F("Initializing DFPlayer (UART2)..."));
  pinMode(DF_BUSY_IN, INPUT_PULLUP);
  dfSerial.begin(9600, SERIAL_8N1, DF_RXD2, DF_TXD2);
  delay(500);

  if (!dfPlayer.begin(dfSerial, true, false)) {
    Serial.println(F("  DFPlayer not detected. Check wiring & SD card."));
    return false;
  }

  dfPlayer.setTimeOut(500);
  dfPlayer.volume(20);      // volume 0..30
  dfInitialized = true;

  Serial.println(F("  DFPlayer detected and initialized."));
  return true;
}

// 5) SD (via DFPlayer)
bool runTest_SD() {
  Serial.println(F("\n[5] SD (DFPlayer card)"));
  if (!initDFPlayer()) {
    Serial.println(F("SD test FAIL (no DFPlayer)."));
    return false;
  }

  int fileCount = dfPlayer.readFileCounts();
  Serial.printf("  DFPlayer reports %d files on SD card.\n", fileCount);
  bool pass = (fileCount > 0);
  Serial.printf("SD test: %s\n", pass ? "PASS" : "FAIL (no files on SD)");
  return pass;
}

// 6) DFPlayer + Speaker (operator confirms)
bool runTest_DFPlayer() {
  Serial.println(F("\n[6] DFPlayer + Speaker"));
  if (!initDFPlayer()) {
    Serial.println(F("DFPlayer/Speaker test FAIL (no DFPlayer)."));
    return false;
  }

  Serial.println(F("Playing test track #1 for ~3 seconds. Listen for audio."));
  dfPlayer.play(1);
  unsigned long start = millis();
  while (millis() - start < 3000UL) {
    // Busy line is active LOW when track is playing
    (void)digitalRead(DF_BUSY_IN);
    delay(50);
  }
  dfPlayer.stop();

  Serial.println(F("Did you hear audio from the speaker? (Y/N, 10 s timeout)"));

  unsigned long waitStart = millis();
  while (millis() - waitStart < 10000UL) {
    if (Serial.available()) {
      char c = tolower(Serial.read());
      if (c == 'y') {
        Serial.println(F("DFPlayer/Speaker test: PASS"));
        return true;
      } else if (c == 'n') {
        Serial.println(F("DFPlayer/Speaker test: FAIL (operator)"));
        return false;
      }
    }
  }

  Serial.println(F("No operator response. DFPlayer/Speaker test: FAIL."));
  return false;
}

// 7) Wi-Fi AP
bool runTest_WifiAP() {
  Serial.println(F("\n[7] Wi-Fi AP"));

  WiFi.mode(WIFI_AP);
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String ssid = "KRAKE_" + mac;

  bool ok = WiFi.softAP(ssid.c_str(), "krakefactory");
  if (!ok) {
    Serial.println(F("Failed to start Wi-Fi AP."));
    return false;
  }

  IPAddress ip = WiFi.softAPIP();
  Serial.print(F("AP SSID: "));
  Serial.println(ssid);
  Serial.print(F("AP IP: "));
  Serial.println(ip);
  Serial.println(F("Operator: connect from phone/PC and verify AP is visible."));
  return true;
}

// 8) Wi-Fi STA (manual SSID/PASS via Serial)
bool runTest_WifiSTA() {
  Serial.println(F("\n[8] Wi-Fi STA (manual SSID/PASS)"));

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  delay(500);

  Serial.println(F("Enter Wi-Fi SSID then press Enter (15 s timeout):"));
  Serial.print(F("> "));
  String ssid = readLine();
  if (ssid.length() == 0) {
    Serial.println(F("No SSID entered. Wi-Fi STA test: FAIL."));
    return false;
  }

  Serial.println(F("Enter Wi-Fi PASSWORD then press Enter (can be empty):"));
  Serial.print(F("> "));
  String pass = readLine(); // can be empty

  Serial.print(F("Connecting to: "));
  Serial.println(ssid);

  WiFi.begin(ssid.c_str(), pass.c_str());

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000UL) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("Wi-Fi STA connection FAILED."));
    return false;
  }

  Serial.println(F("Wi-Fi STA connected successfully."));
  Serial.print(F("IP: "));
  Serial.println(WiFi.localIP());
  Serial.print(F("RSSI: "));
  Serial.println(WiFi.RSSI());

  Serial.println(F("Operator: verify connectivity from your console if needed."));
  return true;
}

// A) LittleFS R/W
bool runTest_LittleFS() {
  Serial.println(F("\n[A] LittleFS R/W"));

  if (!LittleFS.begin(true)) {   // true = format if mount fails
    Serial.println(F("LittleFS mount FAILED."));
    return false;
  }

  const char* path    = "/factory_test.txt";
  const char* payload = "KRAKE_FACTORY_TEST";

  Serial.print(F("Writing file "));
  Serial.println(path);
  File f = LittleFS.open(path, FILE_WRITE);
  if (!f) {
    Serial.println(F("Failed to open file for write."));
    return false;
  }
  f.print(payload);
  f.close();

  Serial.println(F("Reading file back..."));
  f = LittleFS.open(path, FILE_READ);
  if (!f) {
    Serial.println(F("Failed to open file for read."));
    return false;
  }
  String readBack = f.readString();
  f.close();

  bool ok = (readBack == payload);
  Serial.printf("LittleFS R/W test: %s\n",
                ok ? "PASS" : "FAIL (content mismatch)");
  return ok;
}

// B) UART0 (USB Serial)
bool runTest_UART0() {
  Serial.println(F("\n[B] UART0 (USB Serial)"));
  Serial.println(F("This test is interactive."));
  Serial.println(F("Type 'Y' then Enter to PASS, 'N' to FAIL (10 s timeout)."));

  unsigned long start = millis();
  while (millis() - start < 10000UL) {
    if (Serial.available()) {
      char c = tolower(Serial.read());
      if (c == 'y') {
        Serial.println(F("UART0 test: PASS (operator confirmed)."));
        return true;
      } else if (c == 'n') {
        Serial.println(F("UART0 test: FAIL (operator)."));
        return false;
      } else {
        Serial.print(F("Echo: "));
        Serial.println(c);
      }
    }
  }

  Serial.println(F("No operator response. UART0 test: FAIL."));
  return false;
}

// C) SPI loopback (MOSI <-> MISO)
bool runTest_SPI() {
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
  Serial.printf("SPI loopback test: %s\n",
                ok ? "PASS" : "FAIL (check loopback wiring)");
  return ok;
}

// D) RS-232 loopback (UART1)
bool runTest_RS232() {
  Serial.println(F("\n[D] RS-232 loopback (UART1)"));
  Serial.println(F("Connect RS-232 TX <-> RX at DB9 (pins 2 and 3) via MAX3232."));
  Serial.printf("Using UART1 TXD=%d, RXD=%d, BAUD=%ld\n",
                UART1_TXD1, UART1_RXD1, UART1_BAUD);

  HardwareSerial rs232(1);
  rs232.begin(UART1_BAUD, SERIAL_8N1, UART1_RXD1, UART1_TXD1);
  delay(200);

  const uint8_t pattern = 0x55;  // 01010101
  unsigned long start = millis();
  bool ok = false;

  while (millis() - start < 3000UL) {
    rs232.write(pattern);
    rs232.flush();
    delay(20);
    if (rs232.available()) {
      int b = rs232.read();
      if (b == pattern) {
        ok = true;
        break;
      }
    }
  }
  rs232.end();

  Serial.printf("RS-232 loopback test: %s\n",
                ok ? "PASS" : "FAIL (check wiring / MAX3232)");
  return ok;
}

// -----------------------------------------------------------------------------
// Test dispatcher
// -----------------------------------------------------------------------------

bool runSingleTestFromIndex(TestIndex idx) {
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

void runAllTests() {
  Serial.println(F("\n[P] Running ALL tests (1 -> D) in order..."));
  for (int i = 0; i < T_COUNT; ++i) {
    testResults[i] = runSingleTestFromIndex(static_cast<TestIndex>(i));
  }
  printSummary();
}

// -----------------------------------------------------------------------------
// Arduino entry points
// -----------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // wait for USB Serial (on some hosts this may always be true; OK)
    delay(10);
  }

  WiFi.mode(WIFI_OFF);   // start from a clean Wi-Fi state
  delay(100);

  printBanner();
  printSummary();        // initial (all FAIL) summary for the unit
  printMenu();
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\r' || c == '\n') {
      // ignore pure newlines
      return;
    }
    c = toupper(c);
    Serial.println(c);   // echo

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

    if (recognized && c != 'P' && c != 'R') {
      printSummary();
    }
    printMenu();
  }
}
