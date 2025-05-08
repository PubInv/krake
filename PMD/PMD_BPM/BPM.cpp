#include "BPM.h"
#include "OLEDDisplay.h"

PulseCounter::PulseCounter(int sensorPin, int ledPin, int threshold, unsigned long intervalMs)
  : _sensorPin(sensorPin), _ledPin(ledPin), _threshold(threshold), _interval(intervalMs),
    _startTime(0), _count(0) {}

void PulseCounter::begin() {
  pinMode(_ledPin, OUTPUT);
  //Serial.begin(9600);
  _startTime = millis();
}


/*
Count 10mS periods at which the signal is above the threshold during the intervale.

*/
int PulseCounter::update() {
  int signal = analogRead(_sensorPin);

  if (signal > _threshold) {
    digitalWrite(_ledPin, HIGH);
    _count++;
    //    _count = _count;

    if (millis() - _startTime >= _interval) {
      Serial.print(_count / 10);  // rough BPM estimation if sampling every 10ms
      Serial.println(" BPM");
      // int16_t rowPosition = 7;
      // display.setCursor(0, rowPosition);
      // display.print(_count  / 10);  // rough BPM estimation if sampling every 10ms
      // display.println(" BPM");
      // Reset for the next minute
      _count = 0;
      _startTime = millis();
    }
  } else {
    digitalWrite(_ledPin, LOW);
  }

  if ((_count / 10) < 1) {
    Serial.println("No signal");
  }

  delay(10);  // Sampling interval
  return (_count / 10);
}
