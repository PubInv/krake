# Summary 
These files are for firmware (for the most part into ESP32).
The GPAD_API is the firmware which will ship in the Krake.
Other files are for factory test / troubele shooting or were developed during to evaluate and understand hardware.

## Userfull link for compairing firmware branches
Jump to compareing on Github various commits.
https://github.com/PubInv/krake/compare

## Arduino IDE Settings and Libraries to Install
The IDE setting for board should be set to "ESP32 Dev Module".

Although we are migrating to VSCode and PlatformIO, For developing Firmware with the Arduino IDE the following libraries are required.
A number of libraries must be installed.  This is an incomplete list:
* ElegantOTA v. 3.1.7 by Ayush Sharma
* ESP AsyncWebServer by ESP32Async
* Async TCP by ESP32Async

(There are many other more common libraries.)

## Caution, Must Modify a Library
We use the Random Nerd Tutorials tutorial: [https://randomnerdtutorials.com/esp32-ota-elegantota-arduino/](https://randomnerdtutorials.com/esp32-ota-elegantota-arduino/) which gives instructions for changing 
the value of ELEGANTOTA_USE_ASYNC_WEBSERVER in ElegantOTA.h.

This value needs to be set to 1:
```
// WARNING! HACK FROM RANDOM NERD
#define ELEGANTOTA_USE_ASYNC_WEBSERVER 1


#ifndef ELEGANTOTA_USE_ASYNC_WEBSERVER
  #define ELEGANTOTA_USE_ASYNC_WEBSERVER 0
#endif
```
