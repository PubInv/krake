# Krake™
## A wireless alarm device purpose-built for life's critical moments
 <img  height="442" alt="image" src="https://github.com/user-attachments/assets/bddd7e47-1920-4bdd-b98a-c8f1bfb84fcb" />
Based on the Public Invention, General Purpose Alarm Device (GPAD) , the Krake is a wireless alarm device with audio visual capabilitites deisgned to alert a human to something needing attention.

## Quick Start
How to get one running — buy/build/flash — in as few steps as possible

## Table of Contents

## Why Krake (merged: origin story + use cases, trimmed)

## Features (single consolidated list)

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

| GPAP message parsing   |   ?  |
| Alarm Acknowledgement workflow, queue handlings and state persistence    |    ? |
| LittleFS Configuration storage    | ?    |
| SPI alarm input support    | ? |  

## Hardware
- Photos (rev1, rev2, captions explaining what changed)
- Power options
- COM port specs

## Firmware
- Architecture table
- MQTT / GPAP protocol reference
- LCD UI + navigation

## Building Your Own
- BOM / test & assembly procedure (as a public link)
- MockingKrake prototype notes

## Roadmap (Future Features)

## Contributing
(link to WorkflowProcedure.md)

## Credits & Mentorship
The Krake has been developed primarily by volunteer engineer Nagham Kheir, with mentorship and oversight from volunteer Inventional Coach Lee Erickson.

[Mentorship and Teamwork: The Story of the Krake](https://www.pubinv.org/2025/03/17/mentorship-and-teamwork-the-story-of-the-krake/)

Nagham Kheir: Invention Coach
Forrest Lee Erickson: Invention Coach
Robert L. Read: Invention Coach
Yehia Shalaby: Public Inventor
Courtney Ludick: Public Inventor
Yukti: Public Inventor
Juhandre Knoetze: Public Inventor


## Trust & Credentials

<img width="100" alt="image" src="https://github.com/user-attachments/assets/b05176e9-930e-41b1-b16d-353f690c531b" />

Krake hardware has been certified by the Open Source Hardware Association (OSHWA):
- OSHWA certification listing:
[OSHWA Certification Directory](https://certification.oshwa.org/list.html?utm_source=chatgpt.com)
- Certification UID: ``` US002818 ```
 
## References & Further Reading
Website: https://pubinv.github.io/krake/
 
HardwareX Article: [HardwareX Article](https://www.overleaf.com/project/6696aaaaa7299f34f83a5575)

User Manual: [draft user manual](https://docs.google.com/document/d/1qrhc7Yi6PFmagl4gnvSoc0T4vC_uqu-NLrmWMKFnwA0/edit?usp=sharing) 

Developers Manual: [draft developers manual](https://docs.google.com/document/d/150WA6Mb1_SFOBmFc9qBpT5b-MqXb7ejaZbKIxUqaDBY/edit?usp=sharing).

## License
