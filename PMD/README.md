## These files


## Hardware based
- [x] 1 ESP32: Do nothing sketch. A template with copywrite and license. It blinks an LED Only.  See: https://github.com/PubInv/krake/blob/main/PMD/PMD_Nothing/PMD_Nothing/PMD_Nothing.ino
- [x] 1.5 ESP32: Multitasking sketch.Similar to Do Nothing but, It blinks 4 LEDs and Serial Splash. found at: https://github.com/PubInv/krake/tree/main/PMD/PMD_Multitasking
- [x] 2 ESP32: BootUp, Similar to Do Nothing but Serial Splash, LCD Test, LCD Splash, DFPlayer Test, DFPlayer Splash, BootButton Test for stuck keys.  Loop() has only non blocking code. See  issue #160 
- [x] 2.5 ESP32: BootUp OLED, Similar to Do Nothing but Serial Splash, OLED Test, OLED Splash, DFPlayer Test, DFPlayer Splash, BootButton Test for stuck keys.  Loop() has only non blocking code.  
- [x] 3 ESP32: WiFiReady, Similar to BootUP but with WiFi Manager and OTA .  See issue #12  and issue #24  
- [x] 3.5 ESP32: WiFiReady_Elegance, Similar to BootUP but with WiFi Manager and OTA and littleFS .  See issue #12  and issue #24  
- [ ] 4 ESP32: MQTTReady, Similar to WiFiReady but with MQTT including WIT.  See issue #37, and #44 and #67 and #85 and #93 
- [ ] 5 ESP32: TempRSSI, Similar to MQTTReady but with IC_Temp and the RSSI value.  Publish alarms on IC temperature.
- [ ] 6 ESP32: AnalogTempRSSI, Similar to TempRSSI but with Analog measurement of one or more inputs and digital reporting of one or more inputs. Publish alarms on analog measurements.
- [ ] 7 Homework2: ButtonsAnaTempRSSI, Similar to AnalogTempRSSI. Based on "HOMEWORK 2" found at: https://github.com/nk25719/PMD___aka-KiCad-esp32-6leds-2switches-1pj-circuit-  Report, publish, pressing of buttons SW1-SW4.
- [ ] 8 MockingKrake: EncoderAnalTempRSSI Similar to ButtonsAnaTempRSSI but with buttons replaced by a rotary encoder. Navigate an LCD menu and choose alarm messages to publish on press of center button.

## Software Based
- [x] 1 Web page based.  See several at: https://github.com/PubInv/krake/tree/main/PMD/PMD_WebPage
- [x] 2 Processing: With keyboard or mouse user interface for a pseudo medical device which controls one or more Krake devices.  Reports to user the ID of the Krakes connected.  See Processing sketch at : https://github.com/PubInv/krake/tree/main/PMD/PMD_Processing_MQTTith 