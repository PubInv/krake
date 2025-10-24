#include "InterruptRotator.h"
#include "GPAD_menu.h"

static RotaryEncoder* encoder = nullptr;

// This global variable represents the state of the menu;
// we are either running the menu (true) or displaying other 
// information (false)
extern bool running_menu;

void initRotator() {
  //  Serial.begin(115200);
   // while (!Serial);
    Serial.println("InterruptRotator example for the RotaryEncoder library.");

    encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

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
        // If we have rotated the encoder, then we enter the menu...
        if (!running_menu)
            reset_menu_navigation();        

        int d = (int)(encoder->getDirection());
  //      Serial.println(d);

   //     int d = (int)(encoder->getDirection());
        bool CW;
        if (d == (int) RotaryEncoder::Direction::CLOCKWISE) CW = true;
        else CW = false;
        // Serial.print("d : ");
        // Serial.println(d);
        // Serial.println((int) RotaryEncoder::Direction::CLOCKWISE);
        // Serial.println((int) RotaryEncoder::Direction::COUNTERCLOCKWISE);
        // Serial.println(CW);
        registerRotationEvent(CW);
        pos = newPos;
    }
}

void IRAM_ATTR checkPositionISR() {
    if (encoder != nullptr) {
        encoder->tick();
    }
}
