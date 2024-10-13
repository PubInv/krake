// File name: PMD_Nothing
// Author: Forrest Lee Erickson
// Date: 20241013
// A PMD sketch whitch does almost nothing. Has a serial splash and a wink no block LED.
// Hardware ESP32 kit

// Customized this by changing these defines
#define PROG_NAME "PMD_Nothingd "
#define VERSION "V0.0.x "
#define MODEL_NAME "Model: PMD_Nothing" 
#define DEVICE_UNDER_TEST "SN: xxxxx"  //A Serial Number
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "USA"

/* Add a detailed description here
 *  This device is the best PMD ever. 
 *  It does nothing except wink the build in LED On pin 2).
 */

#define BAUDRATE 115200 //Serial port

// Some PMD Hardware
// Pins for switches and LEDs and more
#define BOOT_BUTTON 0

//Some more functions


// The Setup
void setup() {
  const int LED_BUILTIN = 2;    // ESP32 Kit
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

  Serial.begin(BAUDRATE);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  delay(500);
  //Serial splash
  Serial.println(F("==================================="));
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.println(MODEL_NAME);
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(F("Compiled at: "));
  Serial.println(F(__DATE__ " " __TIME__) ); //compile date that is used for a unique identifier
  Serial.println(LICENSE);
  Serial.println(F("==================================="));
  Serial.println();

// More setup code here

  digitalWrite(LED_BUILTIN, LOW);  //Make built in LED low at end of setup.
}//end setup()

void loop() {
  // More loop code here

  wink();
}//end loop()
