// Author: Nagham Kheir
// Date: 20250223
// LICENSE "GNU Affero General Public License, version 3 "

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin (not used)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

byte I2C_address = 0x3F;



// void loop() {
//   // Nothing in loop, as scanner only runs once in setup
//   // if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
//   //   Serial.println(F("SSD1306 allocation failed"));
//   //   for (;;)
//   //     ;
//   // }

//   OLED_Splash();
//   testI2cAdress();
// }

void OLED_setup() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("OLED Initialized");
  display.display();
  delay(1000);
}

void OLED_Splash(void) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("PROG_NAME");
  display.println("VERSION");
  display.setCursor(0, 2);
  display.println(F("Compiled at: "));
  display.setCursor(0, 3);
  display.println(F(__DATE__ " " __TIME__));
  display.display();
  delay(3000);
}

void testI2cAdress(void) {
  Wire.beginTransmission(I2C_address);
  byte error = Wire.endTransmission();

  display.clearDisplay();
  display.setCursor(0, 0);

  if (error == 0) {
    display.print("Device found at: 0x");
    if (I2C_address < 16) {
      display.print("0");
    }
    display.println(I2C_address, HEX);
  } else {
    display.println("I2C device not found");
  }
  display.display();
}

void I2cScanner(void) {
  byte error, address;
  int nDevices = 0;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Scanning...");
  display.display();

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      display.print("Found: 0x");
      if (address < 16) {
        display.print("0");
      }
      display.println(address, HEX);
      nDevices++;
      display.display();
      delay(500);
    }
  }

  if (nDevices == 0) {
    display.println("No I2C devices found");
  } else {
    display.print("Total: ");
    display.println(nDevices);
  }
  display.display();
}
