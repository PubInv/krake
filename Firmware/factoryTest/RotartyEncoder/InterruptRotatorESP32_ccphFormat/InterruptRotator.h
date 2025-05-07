#ifndef INTERRUPT_ROTATOR_H
#define INTERRUPT_ROTATOR_H

#include <Arduino.h>
#include <RotaryEncoder.h>

// Metadata
#define COMPANY_NAME       "Public Invention "
#define PROG_NAME          "InterruptRotatorESP32 "
#define MODEL_NAME         "KRAKE_"
#define DEVICE_UNDER_TEST  "20240421_USA1"  // A Serial Number  
#define LICENSE            "GNU Affero General Public License, version 3 "
#define ORIGIN             "LB"

// GPIO definitions for ESP32
#if defined(ESP32)
constexpr int CLK = 39;  // Rotary encoder CLK pin
constexpr int DT  = 36;  // Rotary encoder DT pin
constexpr int SW  = 34;  // Rotary encoder Switch pin
#define PIN_IN1 CLK
#define PIN_IN2 DT
#endif

// Encoder setup and logic functions
void initRotator();
void updateRotator();
void IRAM_ATTR checkPositionISR();

#endif // INTERRUPT_ROTATOR_H
