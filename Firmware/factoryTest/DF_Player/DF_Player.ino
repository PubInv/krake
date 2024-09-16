#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

// Create a SoftwareSerial object with RX and TX pins
SoftwareSerial mySerial(16, 17); // RX, TX

DFRobotDFPlayerMini dfPlayer;

void setup() {
  // Start serial communication
  Serial.begin(115200);
  mySerial.begin(9600);

  // Initialize DFPlayer Mini
  if (!dfPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Mini not detected or not working.");
    while (true); // Stop execution
  }

  Serial.println("DFPlayer Mini detected!");

  // Set volume (0 to 30)
  dfPlayer.volume(20);

  // Play a test track (e.g., track 1)
  dfPlayer.play(1);
}

void loop() {
  // Check if the DFPlayer Mini is playing
  if (!dfPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Mini is playing.");
  } else {
    Serial.println("DFPlayer Mini is not playing.");
  }

  delay(1000); // Wait for 1 second
}
