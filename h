[1mdiff --git a/PMD/PMD_BPM/PMD_BPM.ino b/PMD/PMD_BPM/PMD_BPM.ino[m
[1mindex 9f8ec21..3fff81e 100644[m
[1m--- a/PMD/PMD_BPM/PMD_BPM.ino[m
[1m+++ b/PMD/PMD_BPM/PMD_BPM.ino[m
[36m@@ -58,7 +58,7 @@[m [mconst int LED_PINS[] = { LED_1, LED_2, LED_3, LED_4, LED_5 };[m
 // const int SWITCH_PINS[] = { SW1, SW2, SW3, SW4 };  // SW1, SW2, SW3, SW4[m
 const int LED_COUNT = sizeof(LED_PINS) / sizeof(LED_PINS[0]);[m
 // const int SWITCH_COUNT = sizeof(SWITCH_PINS) / sizeof(SWITCH_PINS[0]);[m
[31m-PulseCounter pulse(39, 15, 2041, 60000);  // (sensorPin, ledPin, threshold, interval)[m
[32m+[m[32mPulseCounter pulse(33, 15, 2041, 60000);  // (sensorPin, ledPin, threshold, interval)[m
 [m
 // //Claases defined below[m
 // class Flasher {[m
[1mdiff --git a/PMD/PMD_BPM_wMQTT/PMD_BPM_wMQTT.ino b/PMD/PMD_BPM_wMQTT/PMD_BPM_wMQTT.ino[m
[1mindex daa45ff..3e48cb5 100644[m
[1m--- a/PMD/PMD_BPM_wMQTT/PMD_BPM_wMQTT.ino[m
[1m+++ b/PMD/PMD_BPM_wMQTT/PMD_BPM_wMQTT.ino[m
[36m@@ -81,7 +81,7 @@[m [mconst int LED_PINS[] = { LED_1, LED_2, LED_3, LED_4, LED_5 };[m
 // const int SWITCH_PINS[] = { SW1, SW2, SW3, SW4 };  // SW1, SW2, SW3, SW4[m
 const int LED_COUNT = sizeof(LED_PINS) / sizeof(LED_PINS[0]);[m
 // const int SWITCH_COUNT = sizeof(SWITCH_PINS) / sizeof(SWITCH_PINS[0]);[m
[31m-PulseCounter pulse(39, 15, 2041, 60000);  // (sensorPin, ledPin, threshold, interval)[m
[32m+[m[32mPulseCounter pulse(33, 15, 2041, 60000);  // (sensorPin, ledPin, threshold, interval)[m
 int myBPM = pulse.update();[m
 [m
 //Elegant OTA Setup[m
