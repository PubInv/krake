/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-uart-communication-serial-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

/**
This program demonstrates use of all three UARTs on the ESP32.

Modified by Robert L. Read to perform a loop back, Dec. 15, 2024
Modified to make UART 1 pins GPIO 2 and 5 by Forrest Lee Erickson, Dec. 16, 2024
Modified to add UART 2 on pins GPIO 16 and 17 by Forrest Lee Erickson, Dec. 16, 2024
**/

// Define TX and RX pins for UART (change if needed)
// On a ESP32 Dev Kit: https://lastminuteengineers.com/esp32-pinout-reference/

// The GPIO7 and GPIO8 are NOT exposed, even though Lee has asked me to test
// In this issue: https://github.com/PubInv/krake/issues/109
// I am therefore testing with GPIO12 and GPIO13 (also labeled D12 and D13 in the ESP32 Dev Kit)

// Tested and works !!!
#define TXD1 2
#define RXD1 15

// Tested and works !!!
#define TXD2 17
#define RXD2 16

// Tested and found to fail on 7 and 9. because of ESP32 flash memory conflict.
// #define TXD1 7
// #define RXD1 8

#include <Arduino.h>

#include <LoopBack.h>

LoopBack loopBackTest(RXD1, TXD1, RXD2, TXD2);

void setup()
{
    Serial.begin(115200); // UART 0 setup

    delay(500);
    Serial.println("ESP32 UART0 Transmitter");
    // mySerialUART1.println("ESP32 UART1 Transmitter");
    // mySerialUART2.println("ESP32 UART2 Transmitter");
}

void loop()
{
    loopBackTest.run(Serial);
}