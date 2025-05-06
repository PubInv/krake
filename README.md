# Krake
Based upon the Public Invention, General Purpose Alarm Device (aka GPAD), the Krake is a wireless alarm device. It flashes lights, and on a speaker plays audio files such as speech, music and alarm sounds. The Krake adds to the GPAD functionality a rotary encoder.

Krake is an **annunciator** meant to alert a human to something needing attention.

The Krake has been developed primarily by volunteer Nagham Kheir, with oversight from volunteer Inventional Coach Lee Erickson.

# Wi-Fi enablement
It will connect as a station to a Wi-Fi local area network.
The Krake receives alarm messages from another device, the Controller.
The Controller may be on the same local area network or the wider internet.

Our [test page is here](https://pubinv.github.io/krake/PMD_GPAD_API.html).

# Arbitrary Sonic Alarms
These sounds are stored on an SD card and can be voice and can be multilingual.
The SD card is removable so that the Krake alarms can be customized (localized) for language.


# About the Name

A Crake is a bird with a distinctive, slighly alarming cry. We changed the spelling as a joke.
"Flaycrake" is an old term for a scarecroew.


# Project Development Outline:

1. Prototype on a solderless board:
   - Connect the components on a solderless breadboard: ESP32, DFplayer, Speaker, 5 lamps, voltage divider circuit acting as an input for pin 34 of the ESP32.

2. Develop firmware for the following features, as of 20240427 we have deleveloped:
   - Connect to WiFi
   - Fetch the input value through WiFi
   - Send an emergency level to the device through WiFi
   - Illuminate a lamp corresponding to the received emergency level with a certain blinking rate
   - Play prerecorded audio output corresponding to the emergency level.

3. Future features both firmware and hardware:
   - Store the received input in a database (save it to SD card or to a cloud based server)
   - Add an LCD to display the input value and the emergency level
   - Include a mute button to silence the audio of the Krake device.

4. Create a soldered prototype of the device.

5. Design a PCB, compliant with JLCPCB.

6. Design a 3D enclosure.

7. Generate gerber files and order the PCB.

8. Assemble all the parts together: enclosure, PCB, and upload the program.

9. Begin testing the device.


# MockingKrake (Krake's Prototype)

## Components:
- ESP32 DevKit V1.  For schematics of DevKit V1 and other information See: https://embedded-systems-design.github.io/overview-of-the-esp32-devkit-doit-v1/
- DFplayer module.  The DFPlayer Mini MP3 Player For Arduino is a small and low cost MP3 module.  For example wiring diagrams of of DFPlayer module for stand alone and to Arduino UNO and other information See:  https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299
- SD card 16 GB
- speaker 8 ohm
- 5 lamps (emergency level ind)
- 8 resistors
- breadboard (solderless )
- connecting wires
- USB cable.  Type A (for connection to PC) to type B micro (to connection on ESP32 Devkit V1)

:fast_forward:  `Newly added:
  -  3 lamps:
     -  mute ind
     -  PWR ind
     -  ON - OFF ind
   - rotary encoder

## connections ft. ESP32

lamps
+ve --- lamp 1 -- resistor 330 ohm -- pin #2
+ve --- lamp 2 -- resistor 330 ohm -- pin #4
+ve --- lamp 3 -- resistor 330 ohm -- pin #5
+ve --- lamp 4 -- resistor 330 ohm -- pin #18
+ve --- lamp 5 -- resistor 330 ohm -- pin #19

input value
ciruit devider or any sensor
circuit devider connections:
-ve -- resistor --- resistor -- +ve
               |
           pin #34

Vin of the ESP32 connected to 3.3 V
GND of ESP32 connected to -ve

DFplayer

RX -- resistor 10 k -- pin #16 (TX2 of the ESP32)
TX -- pin #17 (RX2 of the ESP32)
VCC -- +ve
GND -- -ve


DFplayer ft. speaker
SPK1 -- +ve of the speaker
SPK2 -- -ve of the speaker

<img width="584" alt="Screenshot 2024-05-04 142512" src=https://github.com/PubInv/krake/assets/133608369/39036a1a-d32f-441c-ad68-157c7b0dcb62>


  <!-- > KRAKE 20240511

<img width="626" alt="Screenshot 2024-06-02 093318" src="https://github.com/PubInv/krake/assets/133608369/825dbfa6-d195-47b9-9c5f-870838daa358">

>3D Frontview of the KRAKE PCBA

<img width="628" alt="image" src="https://github.com/PubInv/krake/assets/133608369/96d52ea8-1e95-4f58-8d42-c5fcf8986b38">

>3D backview of the KRAKE PCBA

### Kicanvas live view of current schematic:

[Schematic Link](https://kicanvas.org/?github=%22C%3A%5CUsers%5CLaptop+Pro%5COneDrive%5CDesktop%5CPPF%5CPbF%5C4pi%5Ckrake%5CKrake_V1%5CKrake_V1.kicad_sch%22)

### Kicanvas live view of current PCBA:

[PCBA LINK](https://kicanvas.org/?github=https%3A%2F%2Fgithub.com%2FPubInv%2Fkrake%2Fblob%2Fmain%2FKrake_V1%2FKrake_V1.kicad_pro) " -->
## Typical Current Consumption of MockingKrake - DUT: Homework2 USA1 
* LCD is wired to 3.3 V
* DF Player is wired to 3.3 V and connected to 8 Ohm speaker

| TEST NUMBER | Amp | LCD TEST CONDITION | DFP TEST CONDITION | 
|---|---------------------------------------|----------------------------------------|---|
| 1 |  0.12 to 0.16 | LCD backlight ON, Alarm a1 | DF Player OFF | 
| 2 |  0.08 to 0.11 | LCD backlight OFF, Alarm a0 | DF Player OFF | 
| 3 |  0.23 max | LCD backlight OFF, Alarm a0 | DF Player ON | 
| 4 |  0.24 max | LCD backlight ON, Alarm A5 | DF Player ON | 


# Buidling PCBa of Krake 

## ESP32-WROOM-32D

[esp32-WROOM-32D Datasheet](https://www.lcsc.com/datasheet/lcsc_datasheet_2304140030_Espressif-Systems-ESP32-WROOM-32D-N4_C473012.pdf)

![esp32-pinout-chip-ESP-WROOM-32](https://github.com/user-attachments/assets/c536d734-6dd7-4b16-834e-3e3d778a77b9)


### ESP32-WROOM-32D module pin functions:

- GPIO 0: Controls the boot mode, so avoid using it if you’re connecting LEDs that may be powered at boot.
- GPIO 1 (TX) and GPIO 3 (RX): Used for UART0 communication, so connecting LEDs here might interfere with serial debugging.
- GPIO 6 to GPIO 11: Used for flash memory on ESP32 modules with external flash, so they should not be used for other purposes.
- GPIO 15: May have restrictions based on the board design; avoid using it for LEDs if it conflicts with boot or other functions.
- GPIO 34 to GPIO 39: These are input-only pins, so they cannot drive LEDs directly.

- GPIO 2: Commonly used for the onboard LED on some ESP32 development boards.
- GPIO 4, GPIO 5: Frequently available and safe for IO.
- GPIO 12, GPIO 13, GPIO 14: Commonly used for IO and don’t interfere with boot functions.
- GPIO 16, GPIO 17: Good for general-purpose outputss.
- GPIO 18, GPIO 19, GPIO 21: These are usually free for use and work well with IO.


# Enhancements

1. The Krake is an HTTP Server and receives alarm level requests by an HTTP Client Put or Get message.
2. The Krake is an HTTP Server and returns to a client browser a web page with the system alarm state.
3. The Krake has 5 bright white LEDs which indicate alarm states. State can be indicated by steady or blinking LEDs.
4. Plays five different audio levels corresponding to triggered emergency levels" To "The Krake can play WAV files which are stored at program time (manufacturing time) The size , ie duration, and number of messages is only limited by the size of an SDI card.
5. The Krake Liquid Crystal can display four rows of up to 20 character to further explain an alarm state.
6. Includes a mute button.
7. The Krake in nromal use connects as a Station to a WiFi Access Point. For set up, a user interface is provided to set (Manage) WiFi credentials by the user of a smart device and the Krake provides a WiFi Access Point WiFi.


# References
Hollifield, Bill R., and Eddie Habibi. Alarm management: A comprehensive guide. Isa, 2010.

## Firmware Development Resournces
We found the following articles very helpfull for our development

Multi-tasking the Arduino, three parts the first of which is at: https://learn.adafruit.com/multi-tasking-the-arduino-part-1: 

## Random Nerd Tutorials
1. ESP32: Create a Wi-Fi Manager (AsyncWebServer library) https://randomnerdtutorials.com/esp32-wi-fi-manager-asyncwebserver/
2. ESP32 OTA (Over-the-Air) Updates – ElegantOTA Library with Arduino IDE:  https://randomnerdtutorials.com/esp32-ota-elegantota-arduino/#comment-1018613
3. Arduino IDE 2: Install ESP32 LittleFS Uploader (Upload Files to the Filesystem) 

# License

* [Firmware: Affero GPL 3.0](https://www.gnu.org/licenses/agpl-3.0.en.html#license-text)
* [CERN Open Hardware Licence Version 2 - Strongly Reciprocal](https://ohwr.org/cern_ohl_s_v2.txt)
