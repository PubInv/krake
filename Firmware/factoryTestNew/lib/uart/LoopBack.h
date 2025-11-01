#ifndef LOOP_BACK_H
#define LOOP_BACK_H

#include <Arduino.h>

class LoopBack
{
public:
    LoopBack(int8_t uart1ReceivePin, int8_t uart1TransmitPin, int8_t uart2ReceivePin, int8_t uart2TransmitPin, unsigned long baudRate = DEFAULT_BAUD_RATE);
    ~LoopBack();

    void run(HardwareSerial &serial);

private:
    static const unsigned long DEFAULT_BAUD_RATE = 115200;

    HardwareSerial uart1;
    HardwareSerial uart2;

    int counter;
};

#endif