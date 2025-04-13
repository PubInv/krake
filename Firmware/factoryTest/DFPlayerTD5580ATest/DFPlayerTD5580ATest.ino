/*
   File: DFPlayerTD5580ATest
   Author: Forrest Lee Erickson
   Date: 20240929
*/

#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "DFPlayerTD5580ATest "
#define VERSION "V0.1 "
#define DEVICE_UNDER_TEST "PMD: LCD and DFPlayer"  //A PMD model number
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "USA"

#define BAUDRATE 115200
#define BAUD_DFPLAYER 9600


#include <DFRobotDFPlayerMini.h>

// Create a SoftwareSerial object with RX and TX pins
//SoftwareSerial mySerial(16, 17); // RX, TX

HardwareSerial mySerial1(2);  // Use UART2
DFRobotDFPlayerMini dfPlayer;

const int LED_PIN = 2;  // ESP32 LED
const int LED_D6 = 23;  // ESP32 GPIO23 pin 16 on kit.

const int trac1 = 1;
const int trac2 = 2;

//Functions
void setupDFPlayer() {
  //  Setup UART for DFPlayer
  Serial.println("UART2 Begin");
  mySerial1.begin(BAUD_DFPLAYER, SERIAL_8N1, 16, 17);
  while (!mySerial1) {
    ;  // wait for DFPlayer serial port to connect.
  }
  // Essential Initialize of DFPlayer Mini
  Serial.println("DFPlayer begin with ACK and Reset");
  dfPlayer.begin(mySerial1, true, true);  // (Stream &stream, bool isACK, bool doReset)

  //  delay(30);
  //  Serial.println("Reset DFPlayer.");
  dfPlayer.reset();
  delay(3000);
  Serial.println("Begin DFPlayer again.");
  if (!dfPlayer.begin(mySerial1, true, true)) {
    Serial.println("DFPlayer Mini not detected or not working.");
    while (true)
      ;  // Stop execution
  }
  Serial.println("DFPlayer Mini detected!");

  delay(3000);  //Required for volum to set
  // Set volume (0 to 30)
  dfPlayer.volume(30);  // Set initial volume max
                        //  dfPlayer.volume(20);       // Set initial volume low
  delay(3000);
  Serial.print("Volume is set to: ");
  digitalWrite(LED_D6, HIGH);             //Start of volume read.
  Serial.println(dfPlayer.readVolume());  //Causes program lock up
  digitalWrite(LED_D6, LOW);              //End of volume read.

  dfPlayer.setTimeOut(500);  // Set serial communictaion time out 500ms
  delay(100);


  //  dfPlayer.EQ(0);          // Normal equalization //Causes program lock up

}  //setupDFPLayer


void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  pinMode(LED_D6, OUTPUT);
  digitalWrite(LED_D6, LOW);  //Start out low


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

  setupDFPlayer();

  Serial.println("End of setup");
  digitalWrite(LED_PIN, LOW);
}  // end of setup()

void loop() {
  Serial.println("Play a track");
  dfPlayer.play(1);
  delay(2000);
  dfPlayer.play(2);
  Serial.println("Play MP3 folder");
  // dfPlayer.playMp3Folder(1);
  // delay(2000);
  // dfPlayer.playMp3Folder(2);
}  // end of loop()