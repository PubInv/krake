
<p align="middle">
  <img src="https://github.com/user-attachments/assets/0f4e3910-4cee-44f4-986c-50cb737b0a3f" width="500" />
  <img src="https://github.com/user-attachments/assets/bddd7e47-1920-4bdd-b98a-c8f1bfb84fcb" width="500" /> 
</p>

# Krake™

## A wireless alarm device purpose-built for life's critical moments
The Krake, An Alarm Annunciator designed for easy integration into systems needing to bring to the attention of humans some condition. Can be used by professional system integrators and hobbyists.

This ?150? x ?180? x ?67? mm device has a speaker, a 20x4 character display, five white LEDs. It is a peripheral which can be controlled over a WiFi network using MQTT interface, a wired RS232 interface or a wired SPI interface with fully customiseable audio visual capabilitites. 

#### How it works?
1. Condition detected- A sensor triggers an alarm event
2. Krake Receives it- Alert arrives wirelessly via MQTT over local network or internet
3. You're alerted- Lights flash, audio plays and acknowledgement is sent back


## Quick Start
Purchasable from https://www.pubinv.org/shop/ 

User manuals can be found in the references and further reading section 

#### LCD UI + navigation
 
Planned LCD Layout:

```text
Q:+ NEXT        W B M ⚙
CRIT Pump Failure
ID:123 Temp High
Ack  Dismiss  Shelve
```

Rotary Encoder: 
```text
Rotate: Navigate alarms or menu entries
Short Press: Select item
Long Press: Open Settings Menu
 ```

Alarm Actions:
```text
Rotatary encoder to enter action selection
Acknowledge, Dismiss or Shelve
 ```

Settings Menu: 
```text
The local settings menu includes:
* Volume Level
* Mute Duration
* COM Setup
* Device Reset
* Exit Menu
 ```
#### Build your own
To build your own please see instructions for testing and assembly of Krake hardware:
[Krake Test and Assembly Procedure Document](https://www.overleaf.com/project/691ca3def1fcd4e384b10919)

## Table of Contents
- About Krake
- Example Use Cases
- Features
- Hardware
- Firmware
- Roadmap
- Contributing
- Credits & Mentorship
- Trust & Credentials
- References & Further Reading

## About Krake
The hope was to develop the initial design for a wireless alarm device to alert nearby parties of a particular state. 
About the name, Krake, the inspiration for the invention name is based on a Crake: a bird with a distinctive, slightly alarming cry. We changed the spelling so as to have a unique name and still preserve the humor. 

## Example Use Cases
Care at home: Alert an elderly person living alone to a fall or blood pressure drop — giving family peace of mind from anywhere.

Medical Equipment: For ventilators and critical devices, Krake provides a dedicated alarm channel for life-threatening mechanical failures.

IT Infrastructure: Alert on-site staff instantly when a server overheats, a UPS fails, or network connectivity drops — a physical alarm that doesn't rely on the very infrastructure it's monitoring.

## Features

| Feature    | Details |
| -------- | ------- |
| Customiseable audio  |WAV and MP3 playback, Alarm-level-specific audio, Adjustable volume,SD-card-based audio storage, Busy-line monitoring and Runtime diagnostics   |
| LCD User Interface & Rotary Knob    | 20 x 4 I²C LCD interface integrated with a rotary encoder and custom menu system to navigate settings without the need for an external device.  Real-time alarm display, Alarm queue indication, Wi-Fi status indication, MQTT broker status indication, Volume and mute display and Alarm acknowledgement actions |
| LED Annunciation  | Variety of LED levels  including steady or blinking deisgned to be visible across a room    |
| Flexible Power    | USB-C, 2.1mm center-positive barrel connector or RJ12 SPI interface power jumpers    |
| Wi-Fi Connectivity    | Acts as a Wi-Fi station connected to a local network enabling alerts locally or via internet, captive portal set up, storage of multiple Wi-Fi credentials, automatic reconnection, OTA firmware updates and Network status display   |
| MQTT Protocol    | MQTT alarm subscriptions, GPAP response publishing, Alarm acknowledgements, Alarm dismissal and shelving, Device monitoring, Configurable topic subscriptions and MQTT status display on LCD    |
| Mute Button & time out handling   | Configurable mute timeout, Automatic unmute, Manual mute override and LCD mute indication    |
| HL7 Compatible    | Designed to work with medical data standards    |
| Fully Open Source    | AGPL firmware, CERN OHL Hardware    |
| Persistent COM configuration    | Interfaces to external controllers through a DB9 Female RS-232 DCE connection. Configurable baud rate (1200,2400,4800, 9600, 19200, 38400, 57600, 115200), RTS/CTS hardware flow control and LCD-based configuration menu    |
| Speaker   |    3 inch 10W Full Range Round 8 ohm Speaker with FR:12-16KHz  |
| GPAP message parsing   |  Alarm message are structured, message type, severity digit (0-5), optional message id, optional alarm type deisgnator and optional content up to 80 characters  |
| SPI alarm input support    | ALlows processing of alert signals communicated or wired through SPI data lines to recognise fault or warning flags in connected sensors |  

## Hardware
| Krake™ rev2 | Krake™ rev1|
| -------- | ------- |
|<img  height="442" alt="image" src="https://github.com/user-attachments/assets/bddd7e47-1920-4bdd-b98a-c8f1bfb84fcb" /> | <img   height="242" alt="image" src="https://github.com/user-attachments/assets/3399ab9b-fd3b-418d-bdb9-2b72e172fa07" /> |
| 3 inch 10W Full Range Round 8 ohm Speaker with FR:12-16KHz.| 28mm Speaker|
|dimensions|dimensions|
|functionality?|functionality?|


## Firmware
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

- Example GPAP Responses

```text
oa{1234}   -> Acknowledge
od{1234}   -> Dismiss
os{1234}   -> Shelve
??{1234}   -> Complete     
```

## Roadmap (Future Features)
* Advanced alarm queue management
* Enhanced LCD UI animations and icons
* Bluetooth and BLE alarm forwarding
* Mesh networking support
* Remote firmware fleet management
* Local event history logging
* Expanded GPAP/HL7 interoperability
* Power optimization modes
* Alarm escalation workflows

## Contributing
[Krake Workflow Contribution Procedure](https://github.com/PubInv/krake/blob/main/WorkflowProcedure.md)

## Credits & Mentorship
The Krake has been developed primarily by volunteer engineer Nagham Kheir, with mentorship and oversight from volunteer Inventional Coach Lee Erickson and a group of volunteers from around the globe. 

[Mentorship and Teamwork: The Story of the Krake](https://www.pubinv.org/2025/03/17/mentorship-and-teamwork-the-story-of-the-krake/)

Nagham Kheir: Invention Coach

Forrest Lee Erickson: Invention Coach

Robert L. Read: Invention Coach

Yehia Shalaby: Public Inventor

Courtney Ludick: Public Inventor

Yukti: Public Inventor

Juhandre Knoetze: Public Inventor


## Certifications

<img width="100" alt="image" src="https://github.com/user-attachments/assets/b05176e9-930e-41b1-b16d-353f690c531b" />

Krake hardware has been certified by the Open Source Hardware Association (OSHWA):
 OSHWA certification listing:
[OSHWA Certification Directory](https://certification.oshwa.org/list.html?utm_source=chatgpt.com)

Certification UID: ``` US002818 ```
 
## References & Further Reading
Website: https://pubinv.github.io/krake/

Brochure: [Copy of Krake Brochure.pdf](https://github.com/user-attachments/files/29981489/Copy.of.Krake.Brochure.pdf)

Logo: <img width="100" height="100" alt="KRAKE 2" src="https://github.com/user-attachments/assets/cd340751-52a8-465b-9b9a-d9dfb97265fd" />

Krake Repo: https://github.com/PubInv/krake

Wiki: https://github.com/PubInv/krake/wiki

HardwareX Article: [HardwareX Article](https://www.overleaf.com/project/6696aaaaa7299f34f83a5575)

User Manual: [draft user manual](https://docs.google.com/document/d/1qrhc7Yi6PFmagl4gnvSoc0T4vC_uqu-NLrmWMKFnwA0/edit?usp=sharing) 

Developers Manual: [draft developers manual](https://docs.google.com/document/d/150WA6Mb1_SFOBmFc9qBpT5b-MqXb7ejaZbKIxUqaDBY/edit?usp=sharing).

OSHWA certification listing:
[OSHWA Certification Directory](https://certification.oshwa.org/list.html?utm_source=chatgpt.com)

The team behind Krake: [Mentorship and Teamwork: The Story of the Krake](https://www.pubinv.org/2025/03/17/mentorship-and-teamwork-the-story-of-the-krake/)

Krake workflow: [Krake Workflow Contribution Procedure](https://github.com/PubInv/krake/blob/main/WorkflowProcedure.md)

Build your own: [Krake Test and Assembly Procedure Document](https://www.overleaf.com/project/691ca3def1fcd4e384b10919)

Database for Krake units and test registries: [Krake Factory Inventory](http://ec2-13-51-158-67.eu-north-1.compute.amazonaws.com/factory-form.html)

Global Open Source Quality Assuring System:[Asset History Records Public Invention Krake Rev. 2](https://gosqas.org/record/8CMgkfrS4ufevweKy1QowF)

GPAP:  https://github.com/PubInv/gpap

#### Public Invention


- Github repo: https://github.com/PubInv
- Website: https://www.pubinv.org/
- Products page: https://www.pubinv.org/shop/


## License
Firmware: GNU Affero GPL 3.0

Hardware: CERN Open Hardware Licence Version 2 - Strongly Reciprocal

Krake™ is a trademark of Public Invention.
