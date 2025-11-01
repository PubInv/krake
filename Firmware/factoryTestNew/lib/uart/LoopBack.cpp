#include "LoopBack.h"

LoopBack::LoopBack(int8_t uart1ReceivePin, int8_t uart1TransmitPin, int8_t uart2ReceivePin, int8_t uart2TransmitPin, unsigned long baudRate)
    : uart1(HardwareSerial(1)),
      uart2(HardwareSerial(2)),
      counter(0)
{
    uart1.begin(baudRate, SERIAL_8N1, uart1ReceivePin, uart1TransmitPin);
    uart2.begin(baudRate, SERIAL_8N1, uart2ReceivePin, uart2TransmitPin);
}

LoopBack::~LoopBack()
{
    uart1.end();
    uart2.end();
}

void LoopBack::run(HardwareSerial &serial)
{

    while (uart1.available())
    {
        Serial.println("Bytes Available UART1! :");
        Serial.write(uart1.read());
    }

    while (uart2.available())
    {
        //   Serial.println("Bytes Available! :");
        Serial.write(uart2.read());
    }

    // Send message over UART
    uart1.print("UART1: ");
    uart1.println(String(counter % 6));
    // mySerialUART1.println("\n");

    Serial.println("Sent: " + String(counter % 6));

    counter++; // increment the counter so the message changes.

    delay(1000); // Lets not got too fast to read.
}