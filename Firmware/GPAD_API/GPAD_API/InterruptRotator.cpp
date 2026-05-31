#include "InterruptRotator.h"
#include "GPAD_HAL.h"
#include "GPAD_menu.h"
#include "debug_macros.h"

static RotaryEncoder *encoder = nullptr;
volatile unsigned long rotaryEncoderEventCount = 0;

// This global variable represents the state of the menu;
// we are either running the menu (true) or displaying other
// information (false)
extern bool running_menu;

void initRotator()
{
    //  Serial.begin(115200);
    // while (!Serial);
    DBG_PRINTLN(F("InterruptRotator init"));

    encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

    attachInterrupt(digitalPinToInterrupt(PIN_IN1), checkPositionISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_IN2), checkPositionISR, CHANGE);
}

void updateRotator()
{
    static int pos = 0;

    encoder->tick();
    int newPos = encoder->getPosition();

    if (pos != newPos)
    {
        rotaryEncoderEventCount++;

        DBG_PRINT(F("pos: "));
        DBG_PRINT(newPos);
        DBG_PRINT(F(" dir: "));
        int d = (int)(encoder->getDirection());
        //      Serial.println(d);

        //     int d = (int)(encoder->getDirection());
        bool CW;
        if (d == (int)RotaryEncoder::Direction::CLOCKWISE)
            CW = true;
        else
            CW = false;

        if (!running_menu)
        {
            alarmActionSelectorHandleRotation(CW);
            pos = newPos;
            return;
        }

        // Serial.print("d : ");
        // Serial.println(d);
        // Serial.println((int) RotaryEncoder::Direction::CLOCKWISE);
        // Serial.println((int) RotaryEncoder::Direction::COUNTERCLOCKWISE);
        // Serial.println(CW);
        registerRotationEvent(CW);
        pos = newPos;
    }
}

void IRAM_ATTR checkPositionISR()
{
    if (encoder != nullptr)
    {
        encoder->tick();
    }
}
