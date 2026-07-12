#include "InterruptRotator.h"
#include "GPAD_HAL.h"
#include "GPAD_menu.h"
#include "debug_macros.h"
#include "setup_status.h"
#include <new>

static RotaryEncoder *encoder = nullptr;
volatile unsigned long rotaryEncoderEventCount = 0;

// This global variable represents the state of the menu;
// we are either running the menu (true) or displaying other
// information (false)
extern bool running_menu;

bool initRotator()
{
    DBG_PRINTLN(F("InterruptRotator init"));

    encoder = new (std::nothrow) RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
    if (encoder == nullptr)
    {
        setSetupError(SETUP_ERROR_ROTARY_ENCODER);
        DBG_PRINTLN(F("Rotary encoder allocation failed; continuing without encoder input."));
        return false;
    }

    attachInterrupt(digitalPinToInterrupt(PIN_IN1), checkPositionISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_IN2), checkPositionISR, CHANGE);
    return true;
}

void updateRotator()
{
    static int pos = 0;

    if (encoder == nullptr)
    {
        return;
    }
    encoder->tick();
    int newPos = encoder->getPosition();

    if (pos != newPos)
    {
        rotaryEncoderEventCount++;

        DBG_PRINT(F("pos: "));
        DBG_PRINT(newPos);
        DBG_PRINT(F(" dir: "));
        int d = (int)(encoder->getDirection());
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
