/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-uart-communication-serial-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

/** 
Modified by Robert L. Read to perform a loop back, Dec. 15, 2024
**/

// Define TX and RX pins for UART (change if needed)
// #define TXD1 19
// #define RXD1 21
// On a ESP32 Dev Kit: https://lastminuteengineers.com/esp32-pinout-reference/
// The GPIO7 and GPIO8 are NOT exposed, even though Lee has asked me to test 
// In this issue: https://github.com/PubInv/krake/issues/109
// I am therefore testing with GPIO12 and GPIO13 (also labeled D12 and D13 in the ESP32 Dev Kit)
#define TXD1 12
#define RXD1 13

// Use Serial1 for UART communication
HardwareSerial mySerial(1);

int counter = 0;

void setup() {
  Serial.begin(115200);
 // mySerial.begin(9600, SERIAL_8N1, RXD1, TXD1);  // UART setup
  mySerial.begin(115200, SERIAL_8N1, RXD1, TXD1);  // UART setup  
  Serial.println("ESP32 UART Transmitter");
}

void loop() {

  while (mySerial.available()) {
 //   Serial.println("Bytes Available! :");
    Serial.write(mySerial.read());
  }

  // Send message over UART
  mySerial.print("a");
  mySerial.println(String(counter % 6));
  mySerial.println("\n");
  
  Serial.println("Sent: " + String(counter));
  // Serial.println("RX1");
  // Serial.println(RX1);
  // Serial.println("TX1");
  // Serial.println(TX1);
  
  // increment the counter
  counter++;
  
  delay(1000); 
}