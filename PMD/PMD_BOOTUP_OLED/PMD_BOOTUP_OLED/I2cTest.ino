//*Rui Santos
//  Complete project details at https://randomnerdtutorials.com
//*********//

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void initOLED() {


  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    //for(;;); // tODO: DO NOT TRAP AN ERROR.SET A FLAG.
  }
}

void splashOLED() {
  int16_t rowPosition = 0;
  int16_t columnPosition = 0;
  int16_t rowHeight = 10;  //just a guess

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  // display.setCursor( SCREEN_WIDTH/3,  SCREEN_HEIGHT/3);
  // OLED Splash Screen
  display.setCursor(0, rowPosition);
  display.println("Hello, I am a PMD!");
  rowPosition = rowPosition + rowHeight;
  display.setCursor(0, rowPosition);
  display.print(PROG_NAME);
  rowPosition = rowPosition + rowHeight;
  display.setCursor(0, rowPosition);
  display.print(VERSION);
  rowPosition = rowPosition + rowHeight;
  display.setCursor(0, rowPosition);
  display.print(F("Compiled at:"));
  rowPosition = rowPosition + rowHeight;
  display.setCursor(0, rowPosition);
  display.print(F(__DATE__ " " __TIME__));

  display.display();
}
