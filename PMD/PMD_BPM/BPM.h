#ifndef PULSECOUNTER_H
#define PULSECOUNTER_H

#include <Arduino.h>

class PulseCounter {
public:
  PulseCounter(int sensorPin, int ledPin, int threshold, unsigned long intervalMs);
  void begin();
  void update();

private:
  int _sensorPin;
  int _ledPin;
  int _threshold;
  unsigned long _interval;
  unsigned long _startTime;
  int _count;
  int _BPM ;
};

#endif
