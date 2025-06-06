#include "OLEDDisplay.h"

#define NUM_BARS 5
#define BAR_HEIGHT 2
#define BAR_SPACING 1
#define BAR_MAX_WIDTH 24

CenteredBarsDisplay::CenteredBarsDisplay(Adafruit_SSD1306* oled) {
  display = oled;
}

void CenteredBarsDisplay::drawCenteredHorizontalBars(int centerX, int bottomY) {
  for (int i = 0; i < NUM_BARS; i++) {
    int barWidth = (BAR_MAX_WIDTH / NUM_BARS) * (i + 1);
    int y = bottomY - i * (BAR_HEIGHT + BAR_SPACING);
    display->fillRect(centerX - barWidth / 2, y, barWidth, BAR_HEIGHT, SSD1306_WHITE);
  }
}
