#include "BPM.h"

void PulseCounter::setPotentiometerPin(int potPin) {
  _potPin = potPin;
}

void PulseCounter::updateThresholdFromPot() {
  int raw = analogRead(_potPin);
  int threshold = map(raw, 0, 4095, 1524, 2524);  // Map to suitable threshold range
  _pulseSensor.setThreshold(threshold);

  Serial.print("Potentiometer: ");
  Serial.print(raw);
  Serial.print(" -> Mapped threshold: ");
  Serial.println(threshold);
}

PulseCounter::PulseCounter(int sensorPin, int ledPin, Adafruit_SSD1306* oled)
  : _sensorPin(sensorPin), _ledPin(ledPin), _display(oled), _lastBPM(0) {}

void PulseCounter::begin() {
  _pulseSensor.analogInput(_sensorPin);
  _pulseSensor.blinkOnPulse(_ledPin);
  _pulseSensor.begin();
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
    }
    return bpm;
  }
  return -1;
}

int PulseCounter::getLastBPM() {
  return _lastBPM;
}
