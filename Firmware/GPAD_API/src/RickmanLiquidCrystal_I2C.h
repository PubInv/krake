/* -*- C++ -*- */

// Note: this file copied and slightly modified from

#ifndef RSITE_ARDUINO_MENU_RICKMAN_OUT
#define RSITE_ARDUINO_MENU_RICKMAN_OUT

// #ifndef ARDUINO_SAM_DUE
#include <menuDefs.h>

#include <Wire.h>
//    #include <LiquidCrystal_PCF8574.h>
#include <LiquidCrystal_I2C.h>

namespace Menu
{

  class lcdOut : public cursorOut
  {
  public:
    LiquidCrystal_I2C *device;
    inline lcdOut(LiquidCrystal_I2C *o, idx_t *t, panelsList &p, menuOut::styles s = menuOut::minimalRedraw)
        : cursorOut(t, p, s), device(o) {}
    size_t write(uint8_t ch) override { return device->write(ch); }
    void clear() override
    {
      device->clear();
      panels.reset();
    }
    void setCursor(idx_t x, idx_t y, idx_t panelNr = 0) override
    {
      const panel p = panels[panelNr];
      device->setCursor(p.x + x, p.y + y);
    }
    idx_t startCursor(navRoot &root, idx_t x, idx_t y, bool charEdit, idx_t panelNr = 0) override { return 0; }
    idx_t endCursor(navRoot &root, idx_t x, idx_t y, bool charEdit, idx_t panelNr = 0) override { return 0; }
    idx_t editCursor(navRoot &root, idx_t x, idx_t y, bool editing, bool charEdit, idx_t panelNr = 0) override
    {
      trace(MENU_DEBUG_OUT << "lcdOut::editCursor " << x << "," << y << endl);
      // text editor cursor
      device->noBlink();
      device->noCursor();
      if (editing)
      {
        device->setCursor(x, y);
        if (charEdit)
          device->cursor();
        else
          device->blink();
      }
      return 0;
    }
  };

} // namespace Menu

#endif
// #endif