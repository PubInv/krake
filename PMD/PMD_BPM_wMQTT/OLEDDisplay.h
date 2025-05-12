#ifndef OLEDDISPLAY_H
#define OLEDDISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define PROG_NAME "PMD_BPM"
#define VERSION " V.0.2 "
//#define MODEL_NAME "Model: BUTTON"
#define MODEL_NAME "Model: HW2_WiFiReady_Elegant"
#define DEVICE_UNDER_TEST "SN: 00001"
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "LB"

extern Adafruit_SSD1306 display;

void initOLED();
void splashOLED();

#endif  // OLEDDISPLAY_H
