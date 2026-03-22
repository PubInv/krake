
# Krake™: A General Purpose Wireless Alarm Device

Krake™ is a **wireless alarm device** designed to alert human operators to important conditions through both **visual** (flashing lights) and **auditory** (alarm sounds, music, or speech) cues. Developed by **Public Invention**, Krake adds functionality to the **General Purpose Alarm Device (GPAD)** by incorporating a **rotary encoder** for easy interaction and configuration.

Our latest, finished design:

![Full Krake](https://github.com/user-attachments/assets/bddd7e47-1920-4bdd-b98a-c8f1bfb84fcb)

---

## Key Features

- **Multi-signal Alarms**: Visual (LEDs) and auditory (speaker with custom sound files).
- **Wireless Connectivity**: Connects to local Wi-Fi networks for real-time alert management.
- **Customizable Alarms**: Plays pre-recorded or user-uploaded sounds in multiple languages.
- **Rotary Encoder for Easy Setup**: Quickly navigate device settings with the rotary encoder knob.
- **Flexible Power Options**: Supports USB-C, barrel jack, and SPI power via internal jumpers.
- **Mute Button**: Temporarily silence the audio alarms when needed.

Krake is nearing **completion as a final product**. It is primarily aimed at providing alerts for critical conditions that require human intervention.

---

## Table of Contents

1. [Use Cases](#use-cases)
2. [Technical Specifications](#technical-specifications)
3. [Quick Start Guide](#quick-start-guide)
4. [Hardware and Wiring](#hardware-and-wiring)
5. [Software and Configuration](#software-and-configuration)
6. [Licensing](#licensing)
7. [Contributing](#contributing)
8. [References](#references)

---

## Use Cases

Krake has been developed for several real-world applications, primarily aimed at alerting users to critical conditions:

### 1. **Health Monitoring for the Elderly**
Krake can monitor vital signs such as blood pressure and fall detection, providing auditory and visual alerts when a monitored condition is triggered. It is designed to integrate with health systems that adhere to the **HL7 medical standard** for seamless medical alerts.

### 2. **Medical Equipment Alarms**
Krake serves as an alarm system for medical devices like **ventilators** and **critical care machines**. It can alert users to mechanical failures (e.g., hose disconnections) and other emergencies, providing critical notifications in a timely manner.

---

## Technical Specifications

- **Connectivity**: Wi-Fi (Station mode), MQTT Protocol for communication with controllers.
- **Audio**: MP3 playback via **DFPlayer Mini** (customizable audio files).
- **Visual Indicators**: 5 bright white LEDs to indicate different alarm levels.
- **Rotary Encoder**: For navigating device settings.
- **Mute**: Local mute button to silence alarms temporarily.
- **Input/Output**: RS232 DB9 female port (DCE).
- **Power Options**: USB-C, 2.1mm barrel jack, and SPI interface (via jumpers).

---

## Quick Start Guide

### 1. **Setting Up Krake**

- **Powering Krake**: Use either the **USB-C** cable or the **2.1mm barrel jack** to power up the device. You can also configure Krake to receive power through the **SPI interface** by adjusting internal jumpers.
- **Wi-Fi Configuration**: Upon first boot, Krake creates its own Wi-Fi access point. Connect to the Krake access point and enter your local Wi-Fi credentials through the setup interface.
- **Test Alarm**: Once connected to Wi-Fi, you can send test alarms via the Controller (MQTT broker). Krake will respond with visual and auditory signals according to the configured alarm levels.

### 2. **Interacting with Krake**

- **Rotary Encoder**: Use the rotary encoder to adjust settings or mute/unmute alarms.
- **Mute Button**: Press to toggle silencing of alarms without affecting device functionality.

### 3. **Customization**

- **Audio Files**: Krake's audio files are stored on an SD card. You can easily swap or add your own audio files (e.g., voice alerts, music) to tailor the device to your needs.
- **LED Behavior**: Configure the LED blinking patterns to suit the severity of the alarm levels.

---

## Hardware and Wiring

Krake is powered by an **ESP32 DevKit V1**, interfaced with multiple components including the **DFPlayer Mini MP3 player**, **LED indicators**, and **rotary encoder**. The wiring diagram and detailed hardware connections are available for those interested in building their own Krake unit.

### Key Components:
- **ESP32 DevKit V1**: Main controller.
- **DFPlayer Mini**: For MP3 audio playback.
- **Speaker (8 Ohm)**: Connected to the DFPlayer for audio output.
- **5 LEDs**: Represent different alarm levels (emergency levels 1–5).
- **Rotary Encoder**: Allows users to control the Krake settings.

For more detailed wiring instructions and a complete list of components, refer to the [MockingKrake Prototype](#mockingkrake-prototype).

---


## Krake™  Specifications Documents
[The User/Operator Manual](https://docs.google.com/document/d/1qrhc7Yi6PFmagl4gnvSoc0T4vC_uqu-NLrmWMKFnwA0/edit?pli=1&tab=t.0#heading=h.wxbfbyr25edt)  
[The Alarm Developer's Technical Manual](https://docs.google.com/document/d/150WA6Mb1_SFOBmFc9qBpT5b-MqXb7ejaZbKIxUqaDBY/edit?pli=1&tab=t.0#heading=h.qygltmza7ufl)  

## Software and Configuration

Krake uses the **MQTT protocol** for communication with external devices (the "Controller"). It subscribes to topics for receiving alarm levels and publishes status updates on separate channels.

### Features:
- **MQTT Messaging**: Subscribe to topics using Krake’s unique MAC address suffixed with `-ALM`. Publish acknowledgment messages to `-ACK` topics.
- **HTTP Server**: Krake can be configured as an HTTP server to receive GET and PUT requests, providing a web interface for real-time status updates.
- **API**: Krake extends the **GPAD API**, allowing developers to interact with the device programmatically.

---

## Licensing

- **Firmware**: Licensed under the **Affero GPL 3.0** License.
- **Hardware**: Licensed under the **CERN Open Hardware Licence Version 2 - Strongly Reciprocal**.

For more details on the licenses and usage rights, please refer to the [LICENSE](./LICENSE) file.

---

## Contributing

Krake is an open-source project, and we welcome contributions! If you’d like to help improve Krake, here are a few ways to get involved:

1. **Report Bugs**: If you encounter issues with Krake, please report them on our [GitHub Issues page](https://github.com/PubInv/krake/issues).
2. **Submit Code**: Fork the repository, make improvements, and submit a pull request.
3. **Help with Documentation**: Contributions to documentation are highly appreciated!

Please refer to our [CONTRIBUTING.md](./CONTRIBUTING.md) for more information on how to contribute.

---


### Acknowledgments
- Developed by **Nagham Kheir** with mentorship from **Lee Erickson**.
- **Public Invention** for its commitment to open-source innovation.

---

## About the name: Krake™
Krake™ is a trademark of **Public Invention** and is an open-source project aimed at providing a simple, customizable alarm device for use in critical situations. It is designed to be easy to set up, reliable in operation, and flexible in its application across various industries.

## References
- Hollifield, Bill R., and Eddie Habibi. Alarm management: A comprehensive guide. Isa, 2010.
- [GPAD API Documentation](https://github.com/PubInv/general-purpose-alarm-device/tree/main/Firmware/GPAD_API)
- [MQTT Protocol](https://mqtt.org/)
### Firmware Development Resources:
We found the following articles very helpfull for our development
  - [Multi-tasking with Arduino](https://learn.adafruit.com/multi-tasking-the-arduino-part-1)
  - [ESP32 Wi-Fi Manager Tutorial](https://randomnerdtutorials.com/esp32-wi-fi-manager-asyncwebserver/)
  - Multi-tasking the Arduino, three parts the first of which is at: https://learn.adafruit.com/multi-tasking-the-arduino-part-1: 
  - Random Nerd Tutorials
    - 1. ESP32: Create a Wi-Fi Manager (AsyncWebServer library) https://randomnerdtutorials.com/esp32-wi-fi-manager-asyncwebserver/
    - 2. ESP32 OTA (Over-the-Air) Updates – ElegantOTA Library with Arduino IDE:  https://randomnerdtutorials.com/esp32-ota-elegantota-arduino/#comment-1018613
    - 3. Arduino IDE 2: Install ESP32 LittleFS Uploader (Upload Files to the Filesystem) 

# License

* [Firmware: Affero GPL 3.0](https://www.gnu.org/licenses/agpl-3.0.en.html#license-text)
* [CERN Open Hardware Licence Version 2 - Strongly Reciprocal](https://ohwr.org/cern_ohl_s_v2.txt)
* Krake™ is a trade mark for this device designed by Public Invention.


