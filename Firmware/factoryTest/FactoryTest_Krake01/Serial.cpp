#include "Serial.h"
#include <Arduino.h>

// The serial command system is from: https://www.dfrobot.com/blog-1462.html?srsltid=AfmBOoqm5pHrLswInrTwZ9vcYdxxPC_zH2zXnoro2FyTLEL4L57IW3Sn

char command;
int pausa = 0;


void serialSplash() {
  Serial.println("===================================");
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier
  Serial.println("===================================");
  Serial.println();
}

void menu_opcoes() {
  Serial.println();
  Serial.println(F("=================================================================================================================================="));
  Serial.println(F("Commands:"));
  Serial.println(F(" [1-3] To select the MP3 file"));
  Serial.println(F(" [s] stopping reproduction"));
  Serial.println(F(" [p] pause/continue music"));
  Serial.println(F(" [+ or -] increases or decreases the volume"));
  Serial.println(F(" [< or >] forwards or backwards the track"));
  Serial.println();
  Serial.println(F("================================================================================================================================="));
}  //end menu_opcoes()


void checkSerial(void) {

  //Waits for data entry via serial
  while (Serial.available() > 0) {
    command = Serial.read();

    if ((command >= '1') && (command <= '9')) {
      Serial.print("Music reproduction");
      Serial.println(command);
      command = command - 48;
      dfPlayer.play(command);
      menu_opcoes();
    }

    //Reproduction
    //Stop

    if (command == 's') {
      dfPlayer.stop();
      Serial.println("Music Stopped!");
      menu_opcoes();
    }

    //Pausa/Continua a musica
    if (command == 'p') {
      pausa = !pausa;
      if (pausa == 0) {
        Serial.println("Continue...");
        dfPlayer.start();
      }

      if (pausa == 1) {
        Serial.println("Music Paused!");
        dfPlayer.pause();
      }

      menu_opcoes();
    }


    //Increases volume
    if (command == '+') {
      dfPlayer.volumeUp();
      Serial.print("Current volume:");
      Serial.println(dfPlayer.readVolume());
      menu_opcoes();
    }

    if (command == '<') {
      dfPlayer.previous();
      Serial.println("Previous:");
      Serial.print("Current track:");
      Serial.println(dfPlayer.readCurrentFileNumber() - 1);
      menu_opcoes();
    }

    if (command == '>') {
      dfPlayer.next();
      Serial.println("next:");
      Serial.print("Current track:");
      Serial.println(dfPlayer.readCurrentFileNumber() + 1);
      menu_opcoes();
    }

    //Decreases volume
    if (command == '-') {
      dfPlayer.volumeDown();
      Serial.print("Current Volume:");
      Serial.println(dfPlayer.readVolume());
      menu_opcoes();
    }
  }
}  // end checkSerial
