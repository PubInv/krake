#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>


// Initialize software serial on pins 16 and 17
SoftwareSerial mySoftwareSerial(16, 17);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;
String line;
char command;
bool trackPlaying = false;



void DFP_Test() {

  // Serial communication with the module
  mySoftwareSerial.begin(9600);

  // Check if the module is responding and if the SD card is found
  Serial.println(F("Initializing DFPlayer module ..."));
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    while (true);
  }
  Serial.println(F("DFPlayer Mini module initialized!"));
  // Initial settings
  myDFPlayer.setTimeOut(500);  // Serial timeout 500ms
  myDFPlayer.volume(25);       // Volume 5
  myDFPlayer.EQ(0);            // Normal equalization
}









