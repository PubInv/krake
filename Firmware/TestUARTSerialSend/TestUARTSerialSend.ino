/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-uart-communication-serial-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

// Define TX and RX pins for UART (change if needed)
// #define TXD1 19
// #define RXD1 21

// Use Serial1 for UART communication
HardwareSerial mySerial(1);

int counter = 0;

void setup() {
  Serial.begin(115200);
 // mySerial.begin(9600, SERIAL_8N1, RXD1, TXD1);  // UART setup
  mySerial.begin(115200, SERIAL_8N1, RX1, TX1);  // UART setup  
  Serial.println("ESP32 UART Transmitter");
}

void loop() {
  
  // Send message over UART
  mySerial.print("a");
  mySerial.println(String(counter % 6));
  mySerial.println("\n");
  
  Serial.println("Sent: " + String(counter));
  Serial.println("RX1");
  Serial.println(RX1);
  Serial.println("TX1");
  Serial.println(TX1);
  
  // increment the counter
  counter++;
  
  delay(1000); 
}