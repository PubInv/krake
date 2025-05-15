#include "OLEDDisplay.h"
#include "BPM.h"
#include "ID.h"

#define PROG_NAME "PMD_WiFiReady_Elegant"
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void initOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    // TODO: DO NOT TRAP AN ERROR. SET A FLAG.
  }
}

// void splashOLED() {
//   int16_t rowPosition = 0;
//   int16_t columnPosition = 0;
//   int16_t rowHeight = 10;  // Just a guess

//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(WHITE);

//   display.setCursor(0, rowPosition);
//   display.println("Hello, I am a PMD!");
//   rowPosition += rowHeight;
//   display.setCursor(0, rowPosition);
//   display.print(PROG_NAME);
//   rowPosition += rowHeight;
//   display.setCursor(0, rowPosition);
//   display.print(VERSION);
//   rowPosition += rowHeight;
//   display.setCursor(0, rowPosition);
//   display.print(F("Compiled at:"));
//   rowPosition += rowHeight;
//   display.setCursor(0, rowPosition);
//   display.print(F(__DATE__ " " __TIME__));
//   // rowPosition += rowHeight;
//   // display.setCursor(0, rowPosition);
//   // display.print(F("Compiled at:"));
//   // Serial.println("\nConnected. IP Address: " + WiFi.localIP().toString());
//   // rowPosition += rowHeight;
//   // display.setCursor(0, rowPosition);
//   // extern int _BPM;
//   // display.print(_BPM / 10);  // rough BPM estimation if sampling every 10ms
//   // display.print(F("  BPM"));

//   display.display();
// }
