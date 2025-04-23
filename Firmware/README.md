# Libraries to Install

We normally build the Firmware with the Arduino IDE.

The board should be set to "ESP32 Dev Module".

A number of libraries must be installed.  This is an incomplete list:
* ElegantOTA v. 3.1.7 by Ayush Sharma
* ESP AsyncWebServer by ESP32Async
* Async TCP by ESP32Async

(There are many other more common libraries.)

Note: We use the Random Nerd Tutorials tutorial: [https://randomnerdtutorials.com/esp32-ota-elegantota-arduino/](https://randomnerdtutorials.com/esp32-ota-elegantota-arduino/) which gives instructions for changing 
the value of ELEGANTOTA_USE_ASYNC_WEBSERVER in ElegantOTA.h.

This value needs to be set to 1:
```
// WARNING! HACK FROM RANDOM NERD
#define ELEGANTOTA_USE_ASYNC_WEBSERVER 1


#ifndef ELEGANTOTA_USE_ASYNC_WEBSERVER
  #define ELEGANTOTA_USE_ASYNC_WEBSERVER 0
#endif
```
