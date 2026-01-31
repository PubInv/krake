/*
 * main.cpp selector
 * Define one of the USE_<MODULE> macros (e.g., -DUSE_Krake_MQTT)
 * to build/run that converted module's setup/loop.
 */
#include <Arduino.h>

#include "DFPlayerTD5580ATest.h"
#include "Krake_MQTT.h"
#include "LCD_Test_2.h"
#include "LCD_nI2C_scanner.h"
#include "OTA.h"
#include "RotaryEncoderTest.h"
#include "Serial.h"
#include "WiFiAccessPointWithMAC.h"
void setup() {

#ifdef USE_DFPlayerTD5580ATest
  DFPlayerTD5580ATest_setup();
#endif
#ifdef USE_Krake_MQTT
  Krake_MQTT_setup();
#endif
#ifdef USE_LCD_Test_2
  LCD_Test_2_setup();
#endif
#ifdef USE_LCD_nI2C_scanner
  LCD_nI2C_scanner_setup();
#endif
#ifdef USE_OTA
  OTA_setup();
#endif
#ifdef USE_RotaryEncoderTest
  RotaryEncoderTest_setup();
#endif
#ifdef USE_Serial
  Serial_setup();
#endif
#ifdef USE_WiFiAccessPointWithMAC
  WiFiAccessPointWithMAC_setup();
#endif
}

void loop() {
#ifdef USE_DFPlayerTD5580ATest
  DFPlayerTD5580ATest_loop();
#endif
#ifdef USE_Krake_MQTT
  Krake_MQTT_loop();
#endif
#ifdef USE_LCD_Test_2
  LCD_Test_2_loop();
#endif
#ifdef USE_LCD_nI2C_scanner
  LCD_nI2C_scanner_loop();
#endif
#ifdef USE_OTA
  OTA_loop();
#endif
#ifdef USE_RotaryEncoderTest
  RotaryEncoderTest_loop();
#endif
#ifdef USE_Serial
  Serial_loop();
#endif
#ifdef USE_WiFiAccessPointWithMAC
  WiFiAccessPointWithMAC_loop();
#endif
  delay(1);
}
