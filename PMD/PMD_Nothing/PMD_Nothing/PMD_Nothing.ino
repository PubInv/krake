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
 *  This device is the best PMD ever. It does nothing except wink the build in LED On pin 2).
 */


// Some PMD Hardware
// Pins for switches and LEDs and more
#define BOOT_BUTTON 0



// Heart beat aka activity indicator LED.
//Set LED for Uno or ESP32 Dev Kit on board blue LED.
const int LED_BUILTIN = 2;    // ESP32 Kit
//const int LED_BUILTIN = 13;    //Not really needed for Arduino UNO it is defined in library
const int HIGH_TIME_LED = 900;
const int LOW_TIME_LED = 100;
unsigned long lastLEDtime = 0;
unsigned long nextLEDchange = 100; //time in ms.


#define BAUDRATE 115200


//Some more functions

//Wink the LED
void wink(void) {
  if (((millis() - lastLEDtime) > nextLEDchange) || (millis() < lastLEDtime)) {
    if (digitalRead(LED_BUILTIN) == LOW) {
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = HIGH_TIME_LED;
    } else {
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = LOW_TIME_LED;
    }
    lastLEDtime = millis();
  }
}//end LED wink


// The Setup
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

  Serial.begin(BAUDRATE);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  delay(500);
  //Serial splash
  Serial.println("===================================");
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.println(MODEL_NAME);
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__) ); //compile date that is used for a unique identifier
  Serial.println(LICENSE);
  Serial.println("===================================");
  Serial.println();

// More setup code here

  digitalWrite(LED_BUILTIN, LOW);

}//end setup()

void loop() {
  // More loop code here

  wink();
}//end loop()
