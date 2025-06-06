#ifndef BPM_H
#define BPM_H

#include <Adafruit_SSD1306.h>
#include <PulseSensorPlayground.h>

class PulseCounter {
  public:
    PulseCounter(int sensorPin, int ledPin, Adafruit_SSD1306* oled);
    void begin();
    int update();
    int getLastBPM();
    void setPotentiometerPin(int potPin);
    void updateThresholdFromPot();

  private:
    int _sensorPin;
    int _ledPin;
    int _potPin;
    int _threshold;
    int _lastBPM;

    Adafruit_SSD1306* _display;
    PulseSensorPlayground _pulseSensor;
};

#endif
