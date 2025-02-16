#define PROG_NAME "PMD_Multitasking"
// Author: Nagham Kheir
// Date: 20250216
// A PMD Multitasking sketch.Similar to Do Nothing but, It blinks 4 LEDs and Serial Splash.
// https://github.com/PubInv/krake/issues/158
// Hardware: Homework2 20240421

// Customized this by changing these defines

#define VERSION "V0.0.1 "
#define MODEL_NAME "Model: HW2_Multitasking"
#define DEVICE_UNDER_TEST "SN: 00001"  //A Serial Number
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "LB"

#define BAUDRATE 115200  //Serial port

// Some PMD Hardware

// Pins for switches and LEDs and more
#define BOOT_BUTTON 0
const int LED_BUILTIN = 2;
const int LED_1 = 15;
const int LED_2 = 4;
const int LED_3 = 5;
const int LED_4 = 18;
const int LED_5 = 19;

/* Add a detailed description here
 *  This device is the best PMD ever. 
 *  It does nothing except wink LEDs).
 */

//Some more functions

void serialSplash() {
  //Serial splash
  Serial.println(F("==================================="));
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.println(MODEL_NAME);
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(F("Compiled at: "));
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier
  Serial.println(LICENSE);
  Serial.println(F("==================================="));
  Serial.println();
}

// The Setup
void setup() {

  //setting GPIO to initial values
  pinMode(LED_BUILTIN, OUTPUT);     // set the LED pin mode
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  pinMode(LED_1, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_1, HIGH);        // turn the LED on (HIGH is the voltage level)
  pinMode(LED_2, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_2, HIGH);        // turn the LED on (HIGH is the voltage level)
  pinMode(LED_3, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_3, HIGH);        // turn the LED on (HIGH is the voltage level)
  pinMode(LED_4, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_4, HIGH);        // turn the LED on (HIGH is the voltage level)
  pinMode(LED_5, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_5, HIGH);        // turn the LED on (HIGH is the voltage level)

  Serial.begin(BAUDRATE);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB
  }
  delay(500);
  //Serial splash
  serialSplash();

  // More setup code here
  digitalWrite(LED_1, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_2, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_3, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_4, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_5, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_BUILTIN, LOW);  //Make built in LED low at end of setup.
}  //end setup()

void loop() {
  // More loop code here
  digitalWrite(LED_1, HIGH);
  wink();
}  //end loop()
