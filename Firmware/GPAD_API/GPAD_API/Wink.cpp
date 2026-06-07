//
// Author: Forrest Lee Erickson
// Date: 20241013
// LICENSE "GNU Affero General Public License, version 3 "

// Heart beat aka activity indicator LED, plus queued connection indicators.
#include <Arduino.h>
#include "gpad_utility.h"

namespace
{
  const uint8_t KRAKE_LED_BUILTIN = 13;
  const unsigned long HEARTBEAT_ON_MS = 1400;
  const unsigned long HEARTBEAT_OFF_MS = 500;
  const unsigned long PATTERN_ON_MS = 120;
  const unsigned long PATTERN_OFF_MS = 140;
  volatile uint8_t pendingPatternPulses = 0;
}

void queueWinkPattern(uint8_t pulseCount)
{
  if (pulseCount > pendingPatternPulses)
  {
    pendingPatternPulses = pulseCount;
  }
}

void wink(void)
{
  static bool initialized = false;
  static bool ledOn = false;
  static bool patternActive = false;
  static uint8_t patternPulsesRemaining = 0;
  static unsigned long lastLedChangeMs = 0;
  static unsigned long nextLedChangeMs = HEARTBEAT_OFF_MS;
  const unsigned long now = millis();

  if (!initialized)
  {
    pinMode(KRAKE_LED_BUILTIN, OUTPUT);
    digitalWrite(KRAKE_LED_BUILTIN, LOW);
    initialized = true;
  }

  if (!patternActive && pendingPatternPulses > 0)
  {
    patternPulsesRemaining = pendingPatternPulses;
    pendingPatternPulses = 0;
    patternActive = true;
    ledOn = true;
    digitalWrite(KRAKE_LED_BUILTIN, HIGH);
    lastLedChangeMs = now;
    nextLedChangeMs = PATTERN_ON_MS;
    return;
  }

  if (!millisIntervalElapsed(now, lastLedChangeMs, nextLedChangeMs))
  {
    return;
  }

  if (patternActive)
  {
    if (ledOn)
    {
      ledOn = false;
      digitalWrite(KRAKE_LED_BUILTIN, LOW);
      if (--patternPulsesRemaining == 0)
      {
        patternActive = false;
        nextLedChangeMs = HEARTBEAT_OFF_MS;
      }
      else
      {
        nextLedChangeMs = PATTERN_OFF_MS;
      }
    }
    else
    {
      ledOn = true;
      digitalWrite(KRAKE_LED_BUILTIN, HIGH);
      nextLedChangeMs = PATTERN_ON_MS;
    }
  }
  else
  {
    ledOn = !ledOn;
    digitalWrite(KRAKE_LED_BUILTIN, ledOn ? HIGH : LOW);
    nextLedChangeMs = ledOn ? HEARTBEAT_ON_MS : HEARTBEAT_OFF_MS;
  }
  lastLedChangeMs = now;
}
