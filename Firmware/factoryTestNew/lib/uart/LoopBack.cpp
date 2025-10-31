#include "LoopBack.h"

LoopBack::LoopBack(int8_t uart1ReceivePin, int8_t uart1TransmitPin, int8_t uart2ReceivePin, int8_t uart2TransmitPin, unsigned long baudRate)
    : uart1(HardwareSerial(1)),
      uart2(HardwareSerial(2))
{
    uart1.begin(baudRate, SERIAL_8N1, uart1ReceivePin, uart1TransmitPin);
    uart2.begin(baudRate, SERIAL_8N1, uart2ReceivePin, uart2TransmitPin);
}

LoopBack::~LoopBack()
{
    uart1.end();
    uart2.end();
}