
 <img src="https://github.com/user-attachments/assets/0f4e3910-4cee-44f4-986c-50cb737b0a3f" width="70" alt="Krake logo" />
<h1 align="middle">
  Krake™
   </h1>
<h2 align="middle">
 A wireless alarm device purpose-built for life's critical moments
 </h2>
<h1 align="middle">
  <img src="https://github.com/user-attachments/assets/bddd7e47-1920-4bdd-b98a-c8f1bfb84fcb" width="290" /> 
 </h1>
 
The Krake, An Alarm Annunciator designed for easy integration into systems needing to bring to the attention of humans some condition. Can be used by professional system integrators and hobbyists.

This 150 x 180 x 67 mm device has a speaker, a 20x4 character display, five white LEDs. It is a peripheral which can be controlled over a WiFi network using MQTT interface, a wired RS232 interface or customisable audio and visual indications. SPI broker output is implemented separately; SPI alarm input is not enabled in the reviewed ESP32 firmware. 

#### How it works?
1. A condition is detected by an external sensor that relays this information to Krake to trigger and alarm event. Such as a pressure sensor detecting the opening of a gate.
2. The Krake receives the information about the event wirelessly via MQTT over local network or internet. A gate open sensor on a security system would publish a message to the Krake via MQTT or direct RS232 connection
3. Krake displays the alarm message, drives the severity LEDs, and plays configured audio when the alarm is audible and not muted.
4. The user is able to react to the alarm with {acknowledgement, dismissal, shelving or completion}, A mute button allows the user to temporarily mute the audio to prevent alarm fatigue.

## User Manual

A User Manual can be found [here](https://docs.google.com/document/d/1qrhc7Yi6PFmagl4gnvSoc0T4vC_uqu-NLrmWMKFnwA0/edit?usp=sharing).


## Table of Contents

- [Quick Start](#quick-start)
- [About Krake](#about-krake)
- [Example Use Cases](#example-use-cases)
- [Features](#features)
- [Hardware](#hardware)
- [Roadmap (Future Features)](#roadmap-future-features)
- [Contributing](#contributing)
- [Credits & Mentorship](#credits--mentorship)
- [Certifications](#certifications)
- [References & Further Reading](#references--further-reading)

  
## Quick Start
Once upon a time, we (Robert, Nagham and Lee) had envisioned that upon receiving a Krake the user would practice working with it by using the MQTT web page to publish messages to their device.

Purchasable from https://www.pubinv.org/shop/ 

User manuals & test and assembly procedure can be found in the references and further reading section 

#### LCD UI and local controls

The current firmware includes LCD status pages and rotary-encoder navigation for alarm actions and settings. Available actions include acknowledge, dismiss, shelve, and complete. These actions send GPAP responses; dismiss, shelve, and complete also clear the local displayed alarm.

Settings include volume, alarm repeat interval, mute duration, Wi-Fi status, broker selection, COM configuration, device information, and reset. See the [user manual](https://docs.google.com/document/d/1qrhc7Yi6PFmagl4gnvSoc0T4vC_uqu-NLrmWMKFnwA0/edit?usp=sharing) for operating instructions.

The proposed priority-queue display is described under [Roadmap](#roadmap-future-features).

#### Build your own
To build your own please see instructions for testing and assembly of Krake hardware:
[Krake Test and Assembly Procedure Document](https://github.com/PubInv/krake/wiki/Krake-Test-and-Assembly-Procedure-Document)


## About Krake
The hope was to develop the initial design for a wireless alarm device to alert nearby parties of a particular state. 
About the name, Krake, the inspiration for the invention name is based on a Crake: a bird with a distinctive, slightly alarming cry. We changed the spelling so as to have a unique name and still preserve the humor. 

## Example Use Cases
Care at home: Alert an elderly person living alone to a fall or blood pressure drop — giving family peace of mind from anywhere.

Medical Equipment: For ventilators and critical devices, Krake provides a dedicated alarm channel for life-threatening mechanical failures.

IT Infrastructure: Alert on-site staff instantly when a server overheats, a UPS fails, or network connectivity drops — a physical alarm that doesn't rely on the very infrastructure it's monitoring.

## Features

### Implemented features

The following capabilities are present in the current [GPAD_API firmware](Firmware/GPAD_API/GPAD_API). Availability on an individual device depends on its hardware revision, installed firmware, configuration, audio files, and web filesystem. Source review does not replace testing on the target device.

| Feature | Current implementation |
| --- | --- |
| GPAP alarm messages | Processes severity levels 0–5, optional message IDs and type designators, and alarm text. |
| Audio alarms | DFPlayer-based playback from an SD card, severity-based track selection, adjustable volume, repeat interval, and busy-line checks. MP3 alarm files are documented; WAV playback needs separate verification. |
| LCD and rotary encoder | 20 × 4 character LCD with alarm text, status icons, message paging, local settings, and alarm-action selection. |
| Severity LEDs | Drives the five alarm LEDs according to alarm level. Exact patterns and visibility should be checked on the target device. |
| Mute controls | Physical mute button, timed mute with automatic unmute, indefinite mute, manual unmute, and LCD mute status. |
| Wi-Fi setup and recovery | Setup portal, saved network credentials, multiple stored networks, and reconnection logic. |
| MQTT communication | Configurable broker and topics, alarm subscriptions, response publishing, connection status, and web configuration/monitoring routes. |
| Alarm actions | Acknowledge, dismiss, shelve, and complete actions publish GPAP responses. Dismiss, shelve, and complete clear the local alarm; they do not provide a persistent priority alarm queue or automatic reactivation of shelved alarms. |
| RS-232 communication | UART command handling and saved COM settings, with supported baud rates from 1200 to 115200, 8-N-1 format, and optional RTS/CTS flow control. |
| SPI broker output | Mirrors outgoing MQTT topic/payload pairs over a separate local SPI controller link, including when MQTT is unavailable. This is output, not SPI alarm reception. |
| Firmware updates | ElegantOTA integration for browser-based updates when enabled in the firmware build. Fleet-wide update management is a roadmap item. |
| Device information | Firmware/system information, connection status, and diagnostic output; some diagnostics depend on build settings. |

Implementation references: [main firmware](Firmware/GPAD_API/GPAD_API/GPAD_API.ino), [hardware and LCD logic](Firmware/GPAD_API/GPAD_API/GPAD_HAL.cpp), [local menus](Firmware/GPAD_API/GPAD_API/GPAD_menu.cpp), [audio](Firmware/GPAD_API/GPAD_API/DFPlayer.cpp), and [runtime connectivity/SPI output](Firmware/GPAD_API/README.md#runtime-connectivity-and-spi-broker-mirror).

### Hardware and licensing

- Power connections described for Krake include USB-C, a 2.1 mm center-positive barrel connector, and SPI-interface power jumpers. Follow the hardware revision's wiring and power instructions.
- The Rev. 2 hardware is described with a 3-inch, 10 W, 8-ohm speaker; Rev. 1 uses a smaller speaker.
- Firmware and hardware source files are available in this repository. See the applicable license files and source notices.

### Features requiring verification or further implementation

| Feature or claim | Current status |
| --- | --- |
| SPI alarm input on ESP32 Krake | Legacy GPAD input code exists, but SPI input setup is commented out and input servicing is guarded for GPAD. Do not describe it as enabled on the reviewed ESP32 Krake firmware. |
| HL7 compatibility | GPAP parsing is implemented. Direct HL7 message handling and interoperability have not been established by this review. |
| WAV alarm playback | Needs verification with the audio module, file preparation, and firmware used on the target device. |
| Priority alarm queue | The LCD includes PAQ-style indicators, but the reviewed alarm state stores one current alarm. Queue ordering, retention, and next-alarm selection remain roadmap work. |

## Hardware
| Krake™ rev2 | Krake™ rev1|
| -------- | ------- |
|<img  height="442" alt="image" src="https://github.com/user-attachments/assets/bddd7e47-1920-4bdd-b98a-c8f1bfb84fcb" /> | <img   height="242" alt="image" src="https://github.com/user-attachments/assets/3399ab9b-fd3b-418d-bdb9-2b72e172fa07" /> |
| 3 inch 10W Full Range Round 8 ohm Speaker with FR:12-16KHz.| 28mm Speaker|
|dimensions|dimensions|
|functionality?|functionality?|


## Roadmap (Future Features)

These are proposed directions, not capabilities promised for the current firmware or a scheduled release.

- Priority alarm queue: retain multiple alarms, order them by priority, and navigate between pending alarms.
- Extended alarm lifecycle: timed shelving, reactivation, and escalation workflows.
- Enhanced LCD layouts, animations, and icons beyond the existing status pages.
- Bluetooth and BLE alarm forwarding.
- Mesh networking.
- Remote firmware fleet management beyond single-device OTA updates.
- Persistent local event history.
- Expanded GPAP interoperability and defined HL7 integration.
- Power optimization modes.

### Proposed priority-queue LCD layout

This mockup illustrates a future queue view; it is not an exact description of the current screen.

```text
Q:+ NEXT        W B M ⚙
CRIT Pump Failure
ID:123 Temp High
Ack  Dismiss  Shelve
```

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
 

Brochure: [Copy of Krake Brochure.pdf](https://github.com/user-attachments/files/29981489/Copy.of.Krake.Brochure.pdf)

Logo: <img width="60" height="60" alt="KRAKE 2" src="https://github.com/user-attachments/assets/cd340751-52a8-465b-9b9a-d9dfb97265fd" />

Krake Repo: https://github.com/PubInv/krake

Wiki: https://github.com/PubInv/krake/wiki

HardwareX Article: [ TBD]

User Manual: [draft user manual](https://docs.google.com/document/d/1qrhc7Yi6PFmagl4gnvSoc0T4vC_uqu-NLrmWMKFnwA0/edit?usp=sharing) 

Developers Manual: [draft developers manual](https://docs.google.com/document/d/150WA6Mb1_SFOBmFc9qBpT5b-MqXb7ejaZbKIxUqaDBY/edit?usp=sharing).

OSHWA certification listing:
[OSHWA Certification Directory](https://certification.oshwa.org/list.html?utm_source=chatgpt.com)

The team behind Krake: [Mentorship and Teamwork: The Story of the Krake](https://www.pubinv.org/2025/03/17/mentorship-and-teamwork-the-story-of-the-krake/)

Krake workflow: [Krake Workflow Contribution Procedure](https://github.com/PubInv/krake/blob/main/WorkflowProcedure.md)

Build your own: [Krake Test and Assembly Procedure Document](https://github.com/PubInv/krake/wiki/Krake-Test-and-Assembly-Procedure-Document)


Global Open Source Quality Assuring System:[Asset History Records Public Invention Krake Rev. 2](https://gosqas.org/record/8CMgkfrS4ufevweKy1QowF)

GPAP:  https://github.com/PubInv/gpap

#### Public Invention


- Github repo: https://github.com/PubInv
- Website: https://www.pubinv.org/
- Products page: https://www.pubinv.org/shop/



