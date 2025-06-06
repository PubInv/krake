#include "BPM.h"

PulseCounter::PulseCounter(int sensorPin, int ledPin, int threshold, Adafruit_SSD1306* oled)
  : _sensorPin(sensorPin), _ledPin(ledPin), _threshold(threshold), _display(oled), _lastBPM(0) {}

void PulseCounter::begin() {
  _pulseSensor.analogInput(_sensorPin);
  _pulseSensor.blinkOnPulse(_ledPin);
  _pulseSensor.setThreshold(_threshold);
  _pulseSensor.begin();

  // _display->clearDisplay();
  // _display->setTextSize(2);
  // _display->setTextColor(SSD1306_WHITE);
  // _display->setCursor(0, 0);
  // _display->println("BPM: --");
  // _display->display();
}

int PulseCounter::update() {
  if (_pulseSensor.sawStartOfBeat()) {
    int bpm = _pulseSensor.getBeatsPerMinute();
    if (bpm != _lastBPM) {
      _lastBPM = bpm;
      Serial.print("Analog: ");
      Serial.print(analogRead(_sensorPin));
      Serial.print("  BPM: ");
      Serial.println(bpm);


      // _display->clearDisplay();
      // _display->setCursor(0, 0);
      // _display->print("BPM: ");
      // _display->println(bpm);
      // _display->display();
    }
    return bpm;
  }

  // return -1;  // No new BPM yet
}

int PulseCounter::getLastBPM() {
  return _lastBPM;
}
