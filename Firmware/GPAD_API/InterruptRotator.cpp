#include "InterruptRotator.h"

static RotaryEncoder* encoder = nullptr;

void initRotator() {
  //  Serial.begin(115200);
   // while (!Serial);
    Serial.println("InterruptRotator example for the RotaryEncoder library.");

    encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR0);

    attachInterrupt(digitalPinToInterrupt(PIN_IN1), checkPositionISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_IN2), checkPositionISR, CHANGE);
}

void updateRotator() {
    static int pos = 0;

    encoder->tick();
    int newPos = encoder->getPosition();

    if (pos != newPos) {
        Serial.print("pos: ");
        Serial.print(newPos);
        Serial.print(" dir: ");
        Serial.println((int)(encoder->getDirection()));
        pos = newPos;
    }
}

void IRAM_ATTR checkPositionISR() {
    if (encoder != nullptr) {
        encoder->tick();
    }
}
