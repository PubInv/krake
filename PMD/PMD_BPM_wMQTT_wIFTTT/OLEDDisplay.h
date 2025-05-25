#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Adafruit_SSD1306.h>

class CenteredBarsDisplay {
public:
  CenteredBarsDisplay(Adafruit_SSD1306* oled);
  void drawCenteredHorizontalBars(int centerX, int bottomY);

private:
  Adafruit_SSD1306* display;
};

#endif
