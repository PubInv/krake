void DFPoutputControl(void) {

  // Serial communication with the module
  mySoftwareSerial.begin(9600);

  // Waits for data entry via serial
  while (Serial.available() > 0) {
    command = Serial.read();

    // Play from first 9 files
    if ((command >= '1') && (command <= '5')) {
      Serial.print("Music reproduction: ");
      Serial.println(command);
      command = command - '0';
      myDFPlayer.play(command);
      myDFPlayer.enableLoop();
      trackPlaying = true;
      menu_options();
    }

    // Toggle repeat mode
    if (command == 'r') {
      myDFPlayer.enableLoop();
      Serial.println("Repeat mode enabled.");
    }

    // Pause or resume
    if (command == 'p') {
      if (trackPlaying) {
        myDFPlayer.pause();
        trackPlaying = false;
        Serial.println("Paused. Press 'p' again to resume or select another track.");
      } else {
        myDFPlayer.start();
        trackPlaying = true;
        Serial.println("Resumed.");
      }
      menu_options();
    }

    // Set volume
    if (command == 'v') {
      if (Serial.available() > 0) {
        int myVolume = Serial.parseInt();
        if (myVolume >= 0 && myVolume <= 30) {
          myDFPlayer.volume(myVolume);
          Serial.print("Current Volume: ");
          Serial.println(myDFPlayer.readVolume());
        } else {
          Serial.println("Invalid volume level, choose a number between 0-30.");
        }
      }
    }

    // Increases volume
    if (command == '+') {
      myDFPlayer.volumeUp();
      Serial.print("Current Volume: ");
      Serial.println(myDFPlayer.readVolume());
      menu_options();
    }

    // Decreases volume
    if (command == '-') {
      myDFPlayer.volumeDown();
      Serial.print("Current Volume: ");
      Serial.println(myDFPlayer.readVolume());
      menu_options();
    }
  }

  // Check if the track is still playing and restart if it's not
  if (trackPlaying && !myDFPlayer.available()) {
    myDFPlayer.play();
  }
}

void menu_options() {
  Serial.println();
  Serial.println(F("=================================================================================================================================="));
  Serial.println(F("Commands:"));
  Serial.println(F(" [1-5] To select the MP3 file"));
  Serial.println(F(" [p] pause/resume"));
  Serial.println(F(" [vX] set volume to X"));
  Serial.println(F(" [+ or -] increases or decreases the volume"));
  Serial.println(F(" [r] enable repeat mode"));
  Serial.println();
  Serial.println(F("================================================================================================================================="));
}