
# Onboarding: “Help Us Help You”.  

## Track 1 — Electronics Foundations (Basic + Digital)

### Important note
Digital electronics still contains **analog behavior** inside real components. Please learn **Basic Electronics first (or alongside Digital Electronics)** before working on digital circuits.

### Digital Electronics (start here)
- Digital Electronics tutorials hub: https://www.build-electronic-circuits.com/  
- Start page: https://www.build-electronic-circuits.com/binary-number-system/
  - The Binary Number System
  - Logic Gates: AND, OR, NOT, NAND, NOR, XOR, XNOR
  - The S-R Latch
  - The D Latch
  - The D Flip-Flop
  - The JK Flip-Flop
  - The T Flip-Flop
  - The Shift Register
  - Binary Adders: Half Adder
  - Binary Adders: Full Adder
  - Open Collector Outputs
  - 4000 Series IC Tutorials
  - 7400 Series IC Tutorials

### Basic Electronics (start here)
- Start page: https://www.build-electronic-circuits.com/basic-electronics/
  - Understanding Basic Electronics
  - Basic Components Used in Circuits
  - Current, Voltage, Resistance
  - What Is Electric Current?
  - Ohm’s Law
  - Series and Parallel Circuits
  - Voltage Divider (Beginners)
  - Kirchhoff’s Voltage/Current Laws
  - Thevenin’s Theorem
  - AC vs DC
  - Short Circuits
  - Negative Voltage
  - RC Delay Element
  - Tips + Circuit simulator guidance

---

## Track 2 — Embedded/Firmware Checklist (condensed, de-duplicated)


When asking for help, provide:

* Exact board/module + purchase link + correct schematic
* Exact part numbers + datasheets for all parts
* What test equipment you have (multimeter, scope, PSU, etc.)
* Full project or minimal reproducible example

Learning path reminders:

* Start with UNO/ATmega328 basics, then ESP32 (3.3V differences)
* Learn “no delay()” multitasking style:
  [https://learn.adafruit.com/multi-tasking-the-arduino-part-1](https://learn.adafruit.com/multi-tasking-the-arduino-part-1)
* Interrupts (UNO) repo example:
  [https://github.com/ForrestErickson/ManyInterupts](https://github.com/ForrestErickson/ManyInterupts)
* Random Nerd Tutorials (Arduino + ESP32 projects):
  [https://randomnerdtutorials.com/projects-arduino/](https://randomnerdtutorials.com/projects-arduino/)
  [https://randomnerdtutorials.com/projects-esp32/](https://randomnerdtutorials.com/projects-esp32/)

MQTT essentials (HiveMQ):

* [https://www.hivemq.com/info/mqtt-essentials/#essential-guide](https://www.hivemq.com/info/mqtt-essentials/#essential-guide)
* Topics & wildcards: [https://www.hivemq.com/blog/mqtt-essentials-part-5-mqtt-topics-best-practices/](https://www.hivemq.com/blog/mqtt-essentials-part-5-mqtt-topics-best-practices/)
  Key features: Last Will, Retained, QoS




  
## NTP for ESP32 (time stamping messages)

More for you to learn. Network Time Protocol, aka NTP.
 
The NTP is a feature you could add to the PMD so that it could time stamp medical messages.

Using the NTPClient library:
Install the NTPClient library: Download the library from &lt;&lt;Link: a GitHub repository https://github.com/arduino-libraries/NTPClient&gt;&gt; or via the Arduino Library Manager.


-  https://lastminuteengineers.com/esp32-ntp-server-date-time-tutorial/
-  https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
-  https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/
-  https://forum.arduino.cc/t/which-library-for-time-operations/1016669
-  https://forum.arduino.cc/t/getting-time-from-ntp-service-using-nodemcu-1-0-esp-12e/702333
-  https://esp32.com/viewtopic.php?t=5188&start=10
-  https://forum.arduino.cc/t/yet-another-esp32-ntp-question-looping-update-interval/1367219
-  https://randomnerdtutorials.com/esp32-weather-station-pcb/
-  https://docs.arduino.cc/libraries/esp32time/
-  https://basic-starter-kit-for-arduino-uno-r4-wifi.readthedocs.io/en/latest/Extension_Project/Real-time_Weather_OLED.html

 

## p5.js + ml5.js voice commands 

* p5.js sketch link:
  [https://editor.p5js.org/ForrestErickson/sketches/UC5tnT2si](https://editor.p5js.org/ForrestErickson/sketches/UC5tnT2si)
* ml5 sound classifier reference:
  [https://docs.ml5js.org/#/reference/sound-classifier](https://docs.ml5js.org/#/reference/sound-classifier)

---
 
 

## Track 3 — PCB Design (KiCad + Review + Fabrication)

### Useful tool
- Wokwi (quick prototyping/simulation): http://wokwi.com/

### KiCad learning path (recommended)
1. Fast overview (older KiCad v5, still excellent):
   - Robert Feranec: https://youtu.be/maaBkw7IRUc
2. KiCad 7 ESP32 full tutorial (longer, practical end-to-end):
   - https://youtu.be/b-7bMl6fJio?si=NmS-i1ScGdcC0x-T&sfnsn=mo
3. Hierarchical schematics (best conceptual tutorial I’ve seen, older v5 but applicable):
   - John’s Basement: https://youtu.be/m_HTBurAJn4
4. More elaborate project tutorial:
   - Phil’s Lab #65: https://youtu.be/aVUqaB0IMh4

### Design review (must read)
- PCB design & review checklist:
  - https://resources.altium.com/p/pcb-design-and-review-checklist

### Fabricator requirements (example)
- JLCPCB capabilities:
  - https://jlcpcb.com/capabilities/pcb-capabilities

### Help-us-help-you (KiCad)
- Don’t make reviewers guess:
  - Share the **exact project** or a **simplified reproduction** of the problem.

### Optional: conversion utility
- EasyEDA → KiCad converter (schematic + footprint workflow):
  - https://youtu.be/inf5ETqLLGA?si=kv8HlxyqNoEC_-tb

---

## Track 4 — OpenSCAD

[Openscad cheatsheet](https://openscad.org/cheatsheet/index.html?version=2021.01)
 
OpenSCAD preview navigation (“three types of movement”):
[https://en.wikibooks.org/wiki/OpenSCAD_User_Manual/First_Steps/Opening_an_existing_example_model](https://en.wikibooks.org/wiki/OpenSCAD_User_Manual/First_Steps/Opening_an_existing_example_model)

---
## Track 5 — Documentation (Markdown + Diagrams + LaTeX)

### Github Syntax
[Basic writing and formatting syntax](https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax)

### Markdown tools
- Notepad++ download: https://notepad-plus-plus.org/downloads/
- Chrome Markdown Viewer: https://chrome.google.com/webstore/detail/markdown-viewer/ckkdlimhmcjmikdlpkmbgfkaikojcbjk

### Markdown image + tables patterns
**Image with width**
```html
<img width="300" alt="image2" src="https://github.com/user-attachments/assets/ce356f75-8d84-49ad-80ae-3bf60bab5398">
````

**Table with images (example pattern)**

```md
| A | B | C |
|---|---|---|
| 1 | <img width="300" alt="image1" src=""> | <img width="300" alt="image2" src=""> |
| 2 | <img width="300" alt="image3" src=""> | <img width="300" alt="image4" src=""> |
```

### LaTeX / Overleaf

* TeX StackExchange (search answers quickly):

  * [https://tex.stackexchange.com/questions](https://tex.stackexchange.com/questions)
* Overleaf help:

  * [https://www.overleaf.com/learn/how-to/How_do_I_use_Overleaf%3F](https://www.overleaf.com/learn/how-to/How_do_I_use_Overleaf%3F)
* Example YouTube resource:

  * [https://www.youtube.com/watch?v=xcTN4F3l9Ds](https://www.youtube.com/watch?v=xcTN4F3l9Ds)


### Diagrams (Mermaid)

* GitHub Mermaid docs:

  * [https://docs.github.com/en/get-started/writing-on-github/working-with-advanced-formatting/creating-diagrams](https://docs.github.com/en/get-started/writing-on-github/working-with-advanced-formatting/creating-diagrams)
* Typora diagram reference (optional):

  * [https://support.typora.io/Draw-Diagrams-With-Markdown/](https://support.typora.io/Draw-Diagrams-With-Markdown/)

* Workflow Procedure
  * [help us help you contribute](https://github.com/PubInv/krake/blob/main/WorkflowProcedure.md)





