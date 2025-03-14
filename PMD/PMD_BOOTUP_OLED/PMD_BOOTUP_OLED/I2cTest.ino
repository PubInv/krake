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

void test() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    //for(;;); // tODO: DO NOT TRAP AN ERROR.SET A FLAG.
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  // display.setCursor( SCREEN_WIDTH/3,  SCREEN_HEIGHT/3);
  display.setCursor(0, 0);
  // Display static text
  display.println("Hello, I am a PMD!");
  display.setCursor(0, SCREEN_HEIGHT / 5);
  display.print(PROG_NAME);
  display.setCursor(0, SCREEN_HEIGHT / 3);
  display.print(VERSION);
  display.setCursor(0, SCREEN_HEIGHT / 2);
  display.print(F("Compiled at:" ));
  display.setCursor(0, SCREEN_HEIGHT / 1.5);
  display.print(F(__DATE__ " " __TIME__));

  display.display();
}
