#include "RotaryEncoderTest.h"
#include <Arduino.h>
#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "RotaryEncoderTest"
#define VERSION "V0.2 "
#define DEVICE_UNDER_TEST "Krake: Rotary Encoder"  //A PMD model number
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "USA"

#define BAUDRATE 115200




// Define the GPIO pins for CLK, DT, and SW
const int CLK = 39;  // Connected to the CLK pin of the encoder
const int DT = 36;   // Connected to the DT pin of the encoder
const int SW = 34;   // Connected to the SW (Switch) pin of the encoder

int currentStateCLK;
int previousStateCLK;
int encoderPos = 0;
bool isButtonPressed = false;

void RotaryEncoderTest_setup() {
  // Initialize serial communication for debugging
  // Start serial communication
  Serial.begin(BAUDRATE);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB
  }
  delay(500);
  Serial.println("===================================");
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier
  Serial.println("===================================");
  Serial.println();

  
  // Set CLK and DT as inputs
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);

  // Set SW (Switch) as input with internal pull-up resistor
  pinMode(SW, INPUT_PULLUP);

  // Read the initial state of CLK
  previousStateCLK = digitalRead(CLK);
}

void RotaryEncoderTest_loop() {
  // Read the current state of the CLK pin
  currentStateCLK = digitalRead(CLK);

  // If the current state of CLK is different from the last state
  // then the encoder has been rotated
  if (currentStateCLK != previousStateCLK) {

    // If the DT state is different than the CLK state, the encoder
    // is rotating clockwise, otherwise it's counterclockwise
    if (digitalRead(DT) != currentStateCLK) {
      encoderPos++;
      Serial.println("Rotating clockwise");
    } else {
      encoderPos--;
      Serial.println("Rotating counterclockwise");
    }

    Serial.print("Encoder position: ");
    Serial.println(encoderPos);
  }

  // Update the previous state of the CLK with the current state
  previousStateCLK = currentStateCLK;

  // Check if the switch is pressed
  if (digitalRead(SW) == LOW) {
    if (!isButtonPressed) {
      isButtonPressed = true;
      Serial.println("Button Pressed");
    }
  } else {
    if (isButtonPressed) {
      isButtonPressed = false;
      Serial.println("Button Released");
    }
  }

  // Small delay to debounce the switch and encoder
  //  delay(300);
}
