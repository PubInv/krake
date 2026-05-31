# Krake™

Based upon the Public Invention General Purpose Alarm Device (GPAD), the Krake is a wireless annunciator and alarm device designed to alert humans to conditions requiring attention. The Krake combines visual, audible, serial, and network alarm communication into an open-source hardware and firmware platform.

The Krake extends the original GPAD architecture with:

* ESP32-based networking
* MQTT communication
* Rotary encoder navigation
* 20x4 LCD interface
* DFPlayer audio playback
* OTA firmware updates
* Configurable COM port
* Alarm queue handling
* GPAP alarm messaging

[HardwareX Article](https://www.overleaf.com/project/6696aaaaa7299f34f83a5575)

---

# Open Source Hardware Certification

Krake hardware has been certified by the Open Source Hardware Association (OSHWA).

Certification UID:

```text
US002818
```

OSHWA certification listing:
[OSHWA Certification Directory](https://certification.oshwa.org/list.html?utm_source=chatgpt.com)

 <img width="549" height="442" alt="image" src="https://github.com/user-attachments/assets/b05176e9-930e-41b1-b16d-353f690c531b" /> 
 
---

# Krake™ rev2

 <img  height="442" alt="image" src="https://github.com/user-attachments/assets/bddd7e47-1920-4bdd-b98a-c8f1bfb84fcb" /> 

---

# Krake™ rev1

<img   height="242" alt="image" src="https://github.com/user-attachments/assets/3399ab9b-fd3b-418d-bdb9-2b72e172fa07" /> 
 
---

# Mentorship and Development

The Krake has been developed primarily by volunteer engineer Nagham Kheir, with mentorship and oversight from volunteer Inventional Coach Lee Erickson.

[Mentorship and Teamwork: The Story of the Krake](https://www.pubinv.org/2025/03/17/mentorship-and-teamwork-the-story-of-the-krake/)

---

# Use Cases

The Krake is intended to inform human operators of alarm conditions requiring attention.

## Healthcare and Assisted Living

One intended use case is monitoring elderly individuals or patients living independently. A Krake device mounted within a residence may announce:

* Falls
* Blood pressure abnormalities
* Sensor-triggered emergencies
* Medical equipment alarms

The Krake is intended to support interoperability with the HL7 medical standard and the open-source ADaM (Alarm Dialog Management) project.

## Medical Devices

The Krake may function as a dedicated annunciator for sophisticated medical equipment such as ventilators or patient monitoring systems.

Examples include:

* Ventilator hose disconnects
* Power failures
* Pressure abnormalities
* Mechanical faults

The Krake focuses on annunciation and communication, while upstream systems determine alarm conditions.

## Industrial and General Annunciation

Potential applications include:

* Schools
* Factories
* Industrial process monitoring
* Smart buildings
* Emergency notification systems
* Accessibility alerting systems

---

# Current Firmware Features

The current Krake firmware includes:

* Wi-Fi station + captive portal support
* MQTT alarm communication
* GPAP message parsing
* Rotary encoder navigation
* LCD menu system
* Alarm acknowledgement workflow
* Alarm queue handling
* DFPlayer audio annunciation
* Persistent COM configuration
* Mute timeout handling
* OTA firmware updates
* LittleFS configuration storage
* Alarm state persistence
* RS-232 controller interface
* Hardware flow control support
* LED annunciation patterns
* SPI alarm input support

---

# Smart LCD User Interface

The Krake includes a 20x4 I²C LCD interface integrated with a rotary encoder and custom menu system.

## LCD Features

* Real-time alarm display
* Alarm queue indication
* Wi-Fi status indication
* MQTT broker status indication
* Volume and mute display
* Alarm acknowledgement actions
* Rotary encoder navigation
* Settings configuration menu

## Planned LCD Layout

```text
Q:+ NEXT        W B M ⚙
CRIT Pump Failure
ID:123 Temp High
Ack  Dismiss  Shelve
```

## LCD Navigation

### Rotary Encoder

* Rotate:

  * Navigate alarms or menu entries
* Short Press:

  * Select item
* Long Press:

  * Open Settings Menu

### Alarm Actions

When an alarm is active:

* Rotate encoder to enter action selection
* Available actions:

  * Acknowledge
  * Dismiss
  * Shelve

### Settings Menu

The local settings menu includes:

* Volume Level
* Mute Duration
* COM Setup
* Device Reset
* Exit Menu

---

# Wi-Fi Connectivity

The Krake normally operates as a Wi-Fi station connected to a local network.

For setup and provisioning, the Krake can create a temporary Wi-Fi access point using WiFiManager and LittleFS credential storage.

## Features

* Captive portal setup
* Multiple stored Wi-Fi credentials
* Automatic reconnection
* OTA firmware updates
* MQTT connectivity monitoring
* LCD network status display

Credentials are stored locally using LittleFS.

---

# MQTT Protocol

The Krake communicates using MQTT for alarm distribution and acknowledgement.

## Features

* MQTT alarm subscriptions
* GPAP response publishing
* Alarm acknowledgements
* Alarm dismissal and shelving
* Device monitoring
* Configurable topic subscriptions
* MQTT status display on LCD

Each Krake subscribes to alarm topics and publishes alarm responses on dedicated ACK topics.

## Example GPAP Responses

```text
oa{1234}   -> Acknowledge
od{1234}   -> Dismiss
os{1234}   -> Shelve
```

## MQTT Testing Page

[MQTT Publishing Test Page](https://pubinv.github.io/krake/PMD_GPAD_API.html)

---

# GPAP Alarm Message Support

The Krake firmware supports GPAP alarm messaging and response handling.

Incoming alarm messages may contain:

* Alarm level
* Alarm ID
* Alarm type
* Alarm message text

Supported alarm actions:

| Action      | MQTT Response |
| ----------- | ------------- |
| Acknowledge | `oa{alarmId}` |
| Dismiss     | `od{alarmId}` |
| Shelve      | `os{alarmId}` |

---

# COM Port

The Krake interfaces to external controllers through a DB9 Female RS-232 DCE connection.

## COM Features

* Configurable baud rate
* RTS/CTS hardware flow control
* Persistent configuration storage
* LCD-based configuration menu

## Supported Baud Rates

* 1200
* 2400
* 4800
* 9600
* 19200
* 38400
* 57600
* 115200

## Current Serial Configuration

* 8-N-1
* Optional RTS/CTS flow control

---

# Arbitrary Sonic Alarms

The Krake uses a DFPlayer Mini MP3 module connected through UART2 on the ESP32.

## Audio Features

* WAV and MP3 playback
* Alarm-level-specific audio
* Adjustable volume
* SD-card-based multilingual audio
* Busy-line monitoring
* Runtime diagnostics

The SD card is removable so that alarm audio may be customized or localized for language and application.

---

# Mute Button

Pressing the local mute button toggles silencing of audio alarms.

The firmware supports:

* Configurable mute timeout
* Automatic unmute
* Manual mute override
* LCD mute indication

---

# Rotary Encoder Knob

The rotary encoder allows users to:

* Navigate menus
* Configure settings
* Control volume
* Respond to alarms
* Navigate queued alarms

---

# Configurable Power Options

The Krake may be powered using:

* 2.1 mm center-positive barrel connector
* USB-C
* RJ12 SPI interface power jumpers

---

# Firmware Architecture

| Module               | Purpose                            |
| -------------------- | ---------------------------------- |
| `alarm_api.*`        | Abstract alarm state machine       |
| `GPAD_HAL.*`         | Hardware abstraction layer         |
| `GPAD_menu.*`        | Rotary encoder and LCD menu system |
| `mqtt_handler.*`     | MQTT publishing and GPAP responses |
| `gpad_serial.*`      | Serial protocol parser             |
| `DFPlayer.*`         | Audio playback subsystem           |
| `WiFiManagerOTA.*`   | Wi-Fi management and OTA           |
| `InterruptRotator.*` | Rotary encoder interrupt handling  |

---

# Krake Test and Assembly Procedure

Instructions for testing and assembly of Krake hardware:

[Krake Test and Assembly Procedure Document](https://www.overleaf.com/project/691ca3def1fcd4e384b10919)

---

# Inventory

[Krake Factory Inventory](http://ec2-13-51-158-67.eu-north-1.compute.amazonaws.com/factory-form.html)

Database for Krake units and test registries.

---

# GDT Records

[Asset History Records Public Invention Krake Rev. 2](https://gosqas.org/record/8CMgkfrS4ufevweKy1QowF)

Global Open Source Quality Assuring System.

---

# Future Features

* Advanced alarm queue management
* Enhanced LCD UI animations and icons
* Bluetooth and BLE alarm forwarding
* Mesh networking support
* Remote firmware fleet management
* Local event history logging
* Expanded GPAP/HL7 interoperability
* Power optimization modes
* Alarm escalation workflows

---

# MockingKrake (Prototype Platform)

During development, a breadboard prototype called the MockingKrake was used to validate:

* Alarm functionality
* DFPlayer interoperability
* Wi-Fi communication
* LCD functionality
* Rotary encoder navigation
* MQTT messaging

## Components

* ESP32 DevKit V1
* DFPlayer Mini
* SD card
* Speaker
* Alarm LEDs
* Rotary encoder
* LCD display
* Breadboard prototype system

---

# Typical Current Consumption

| TEST NUMBER | Current     | LCD Condition | DFPlayer Condition |
| ----------- | ----------- | ------------- | ------------------ |
| 1           | 0.12–0.16 A | LCD ON        | DFPlayer OFF       |
| 2           | 0.08–0.11 A | LCD OFF       | DFPlayer OFF       |
| 3           | 0.23 A max  | LCD OFF       | DFPlayer ON        |
| 4           | 0.24 A max  | LCD ON        | DFPlayer ON        |

---

# ESP32-WROOM-32D

[ESP32-WROOM-32D Datasheet](https://www.lcsc.com/datasheet/lcsc_datasheet_2304140030_Espressif-Systems-ESP32-WROOM-32D-N4_C473012.pdf)

![esp32-pinout-chip-ESP-WROOM-32](https://github.com/user-attachments/assets/c536d734-6dd7-4b16-834e-3e3d778a77b9)

---

# Enhancements

1. HTTP server support
2. Web monitoring interface
3. Five-level LED annunciation
4. WAV/MP3 alarm playback
5. 20x4 LCD alarm display
6. Configurable mute handling
7. Wi-Fi captive portal setup
8. MQTT alarm workflow support
9. Rotary encoder menu navigation

---

# Workflow Contribution Procedure

[Krake Workflow Contribution Procedure](https://github.com/PubInv/krake/blob/main/WorkflowProcedure.md)

---

# References

Hollifield, Bill R., and Eddie Habibi. *Alarm Management: A Comprehensive Guide.* ISA, 2010.

---

# Firmware Development Resources

## Multi-tasking the Arduino

[https://learn.adafruit.com/multi-tasking-the-arduino-part-1](https://learn.adafruit.com/multi-tasking-the-arduino-part-1)

## Random Nerd Tutorials

1. ESP32 Wi-Fi Manager
   [https://randomnerdtutorials.com/esp32-wi-fi-manager-asyncwebserver/](https://randomnerdtutorials.com/esp32-wi-fi-manager-asyncwebserver/)

2. ESP32 OTA Updates
   [https://randomnerdtutorials.com/esp32-ota-elegantota-arduino/](https://randomnerdtutorials.com/esp32-ota-elegantota-arduino/)

3. ESP32 LittleFS Uploader
   [https://randomnerdtutorials.com/esp32-littlefs-arduino-ide/](https://randomnerdtutorials.com/esp32-littlefs-arduino-ide/)

---

# License

* Firmware: GNU Affero GPL 3.0
* Hardware: CERN Open Hardware Licence Version 2 - Strongly Reciprocal
* Krake™ is a trademark of Public Invention.
