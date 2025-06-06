#include "BPM.h"

void PulseCounter::setPotentiometerPin(int potPin) {
  _potPin = potPin;
}

void PulseCounter::updateThresholdFromPot() {
  static int smoothed = 0;
  int raw = analogRead(_potPin);

  // Smooth the value (simple low-pass filter)
  smoothed = (smoothed * 7 + raw) / 8;

  // Map to PulseSensor threshold range (example range: 300–700)
  int threshold = map(smoothed, 0, 4095, 1500, 2500);
  threshold = constrain(threshold, 1500, 2500);
  _pulseSensor.setThreshold(threshold);
 _currentThreshold = threshold; 
  // Optional: print to Serial
  Serial.print("Pot raw: ");
  Serial.print(raw);
  Serial.print(" | Smoothed: ");
  Serial.print(smoothed);
  Serial.print(" | Threshold: ");
  Serial.println(threshold);

  // // Optional: display on OLED
  // if (_display) {
  //   _display->setCursor(0, 48);
  //   _display->print("Threshold: ");
  //   _display->println(threshold);
  //   _display->display();
  // }
}

int PulseCounter::getThreshold() {
  return _currentThreshold;
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
