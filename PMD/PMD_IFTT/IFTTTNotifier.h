#ifndef IFTTT_NOTIFIER_H
#define IFTTT_NOTIFIER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

extern const int SW1;
extern const int SW2;
extern const int SW3;
extern const char* ssid;
extern const char* password;

void setupIFTTT();
void IFTTTloop();

#endif
