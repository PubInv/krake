#ifndef PULSECOUNTER_H
#define PULSECOUNTER_H

#include <Arduino.h>
#include <PulseSensorPlayground.h>
#include <Adafruit_SSD1306.h>


class PulseCounter {
public:
  PulseCounter(int sensorPin, int ledPin, int threshold, Adafruit_SSD1306* oled);
  void begin();
  int update();
  int getLastBPM();

private:
  int _sensorPin;
  int _ledPin;
  int _threshold;
  Adafruit_SSD1306* _display;
  PulseSensorPlayground _pulseSensor;
  int _lastBPM;
};

#endif
