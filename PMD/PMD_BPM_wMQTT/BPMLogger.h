#ifndef BPM_LOGGER_H
#define BPM_LOGGER_H

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

void initBPMLogger();
void logBPM(int bpm);
void printBPMLog();

#endif