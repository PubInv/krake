#include "ID.h"


void splashserial() {
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

}