#include "IFTTTNotifier.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char* IFTTT_KEY = "duiXj0q8tWc_BYJritfpEq";

static bool buttonPressed = false;

void sendIFTTTRequest(const char* eventName) {
  String url = "https://maker.ifttt.com/trigger/";
  url += eventName;
  url += "/with/key/";
  url += IFTTT_KEY;

  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();
  http.end();

  Serial.print("Sent IFTTT event '");
  Serial.print(eventName);
  Serial.print("'. HTTP Response: ");
  Serial.println(httpResponseCode);
}

void IFTTTloop() {
  if (digitalRead(SW1) == LOW && !buttonPressed) {
    delay(50);
    if (digitalRead(SW1) == LOW) {
      buttonPressed = true;
      sendIFTTTRequest("PMD_NOTIF");
      Serial.println("PMD_NOTIF");
      sendIFTTTRequest("S2UrgentMeeting");
      Serial.println("S2 Urgent Meeting");
      sendIFTTTRequest("PMD_NOTIF_TELEGRAM");
      Serial.println("S3 Sent to TELEGRAM PMD NOTIF");
    }
  }

  // if (digitalRead(SW2) == LOW && !buttonPressed) {
  //   delay(50);
  //   if (digitalRead(SW2) == LOW) {
  //     buttonPressed = true;
  //     sendIFTTTRequest("S2UrgentMeeting");
  //     Serial.println("S2 Urgent Meeting");
  //   }
  // }


  //  if (digitalRead(SW3) == LOW && !buttonPressed) {
  //     delay(50);
  //     if (digitalRead(SW3) == LOW) {
  //       buttonPressed = true;
  //       sendIFTTTRequest("PMD_NOTIF_TELEGRAM");
  //       Serial.println("S3 Sent to TELEGRAM PMD NOTIF");
  //     }
  //   }


  // if (digitalRead(SW1) == HIGH && digitalRead(SW2) == HIGH && digitalRead(SW3) == HIGH) {
  if (digitalRead(SW1) == HIGH) {
    buttonPressed = false;
  }
}
