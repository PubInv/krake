#include "BPMLogger.h"

#define BPM_LOG_FILE "/bpm.\csv"

void initBPMLogger() {
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount Failed");
  } else {
    Serial.println("LittleFS mounted successfully.");
  }
}

void logBPM(int bpm) {
  if (bpm <= 50 || bpm > 120) return;

  File file = LittleFS.open(BPM_LOG_FILE, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open BPM log file for appending.");
    return;
  }

  String entry = String(millis()) + ": BPM=" + String(bpm) + "\r\n";
  file.print(entry);
  file.close();

  Serial.println("BPM logged: " + entry);
}

void printBPMLog() {
  File file = LittleFS.open(BPM_LOG_FILE, FILE_READ);
  if (!file || file.isDirectory()) {
    Serial.println("Failed to open BPM log file for reading.");
    return;
  }

  Serial.println("BPM Log Contents:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}