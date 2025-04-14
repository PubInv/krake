#include "DFP.h"

// Define the global variables
HardwareSerial mySerial1(2);  // Using UART2 // definition of an instant variabl in a class
DFRobotDFPlayerMini dfPlayer;

void initDFP(void) { // definition 
  //  Setup UART for DFPlayer
  Serial.println("UART2 Begin");
  mySerial1.begin(BAUD_DFPLAYER, SERIAL_8N1, 16, 17);
  while (!mySerial1) {
    ;  // wait for DFPlayer serial port to connect.
  }
  // Essential Initialize of DFPlayer Mini
  //  Serial.println("DFPlayer begin with ACK and Reset");

  //  dfPlayer.begin(mySerial1, true, true);  // (Stream &stream, bool isACK, bool doReset)
  // dfPlayer.begin(mySerial1, true, false);  // (Stream &stream, bool isACK, bool doReset)
  // dfPlayer.reset();

  // delay(3000);

  Serial.println("Begin DFPlayer isACK true, doReset false.");
  //  if (!dfPlayer.begin(mySerial1, true, true)) {
  if (!dfPlayer.begin(mySerial1, true, false)) {
    Serial.println("DFPlayer Mini not detected or not working.");
    Serial.println("Check for missing SD Card.");
    availableDFPLAYER = false;
    ;  // Stop execution
  } else {
    Serial.println("DFPlayer Mini detected!");
    availableDFPLAYER = true;
  }

  dfPlayer.volume(20);  // Set initial volume max

  dfPlayer.setTimeOut(500);  // Set serial communictaion time out 500ms
  delay(100);

  Serial.print("dfPlayer State: ");
  Serial.println(dfPlayer.readState());  //read mp3 state
  Serial.print("dfPlayer Volume: ");
  Serial.println(dfPlayer.readVolume());  //read current volume
  Serial.print("dfPlayer EQ: ");
  Serial.println(dfPlayer.readEQ());                   //read EQ setting
  Serial.print("SD Card FileCounts: ");
  Serial.println(dfPlayer.readFileCounts());           //read all file counts in SD card
  Serial.print("Current File Number: ");
  Serial.println(dfPlayer.readCurrentFileNumber());    //read current play file number
  Serial.print("File Counts In Folder: ");
  Serial.println(dfPlayer.readFileCountsInFolder(3));  //read file counts in folder SD:/03

  //  dfPlayer.EQ(0);          // Normal equalization //Causes program lock up


}


void splashDFPlayer(void) {

  if (!availableDFPLAYER) {
    Serial.println("DFPlaywer NOT available");
    return;
  }
  Serial.println("Made it through DFPlaywer available");

  //  // Set volume (0 to 30)
  //  dfPlayer.volume(30);       // Set initial volume max
  dfPlayer.volume(15);  // Set initial volume low
  delay(3000);
  Serial.print("Volume is set to: ");
  // digitalWrite(LED_D6, HIGH);             //Start of volume read.
  Serial.println(dfPlayer.readVolume());  //Causes program lock up
  // digitalWrite(LED_D6, LOW);              //End of volume read.
  Serial.println("Play a track");
  dfPlayer.play(4);
}
