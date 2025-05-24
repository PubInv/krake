#include "IFTTTNotifier.h"
const int SW3 = 34;
const int SW1 = 36;
const int SW2 = 39;
const char* ssid = "Home";
const char* password = "adt@1963#";

void setup() {
  Serial.begin(115200);
  pinMode(SW1, INPUT);  // External pull-up is used
  pinMode(SW2, INPUT);

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    //delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
}

void loop() {
  IFTTTloop();
}
