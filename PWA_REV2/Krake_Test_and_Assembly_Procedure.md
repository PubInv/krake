---
title: "Krake Test and Assembly Procedure Document"
project: "Krake Project"
document_type: "Test and Assembly procedure"
document_id: "KRK-DOC-001"
version: "v0.1.0"
date: "2025-11-30"
status: "Draft"
authors:
  - "Forrest Lee Erickson (BS of Science, Physics)"
  - "Nagham Kheir (BE in Electric and Electronics, emphasis on Biomedical)"
---

# ISO Title Page

```
------------------------------------------------------------------------------
iso title page>>

Document Title:       Krake Test and Assembly procedure Document
Project:              Krake Project
Document Type:        Test and Assembly procedure
Document ID:          KRK-DOC-001
Version:              v0.1.0
Date:                 2025 - 11 - 30
Author(s):            Forrest Lee Erickson BS of Science, Physics.
                      Nagham Kheir BE in Electric and Electronics emphasis on Biomedical
Status:               Draft
------------------------------------------------------------------------------
```

## Revision History

| Version | Date       | Author        | Description                      |
|--------:|------------|---------------|----------------------------------|
| v0.1.0  | 2025-11-30 | Nagham Kheir  | Initial draft                    |
| v0.2.0  | YYYY-MM-DD | [Author Name] | Added architecture diagram       |
| v1.0.0  | YYYY-MM-DD | [Author Name] | Finalized for publication        |

## 1. Abstract
Instructions to test the sub assembly units when received from the manufacturer. And instructions to add additional components to the sub assembly `PCB_KRAKE_REV2.0` to build the final assembly of PWA and speaker into enclosure.

## 2. Purpose
This document is written for a technician and describes the steps for testing and finishing the assembly of the Krake PWA (Printed Wiring Assembly) Rev 2.

## 3. Scope
This document describes how to take the sub assembly received from the contract manufacturer, and add the additional components (Mini MP3 player, SD card, LCD and the LEDs with their spacers) and electrical testing and functional testing. This document describes how to load factory test firmware and finaly product production firmware (the firmware the end user gets). This document also describes the final assembly of the PWA and a speaker into an enclosure.
This document does not describe troubleshooting.

## 4. Definitions and Abbreviations

| Term | Definition |
|------|------------|
| Krake | [Definition] |
| API | Application Programming Interface |
| DFP layer | A Mini MP3 player, DFP layer is by DF Robot, a more generic term is Mini MP3 Player.|
| ESD | Electro Static Discharge, damage caused by over voltage. |
| LCD | Liquid Crystal Display |
| PCB | Printed Circuit Board, typically a fiberglass panel with copper traces but to which electronic comments are NOT yet attached.  |
| PWA | Printed Wiring Assembly, a PCB with components soldered / attached |
| Sketch | the Arduino vocabulary for source code file of the type used in the Arduino IDE and typically ending with the extension ".ino". |
| ... | ... |

## 5. Background
This document was developed from the similar document of the GPAD (General Purpous Alarm Device) found at: https://github.com/PubInv/general-purpose-alarm-device/blob/main/Hardware/Documentation/ManufacturingUnitTestTroubleshootingRev2.md
Additional notes from the Krake issue: ?TBD?

## 6. Technical Description / Methodology
Detailed description, architecture, procedures, protocols, etc.

## 7. Results or Discussion
Data, analysis, or discussion if applicable.

## 8. References
List of documents, papers, links, GitHub issues, etc.

## 9. Appendix (Optional)
Supplementary figures, tables, or data.

```
<<< end of iso title page
------------------------------------------------------------------------------
```

# Krake: PCB Rev 2.1

## Hardware in context

Krake is a network-connected alarm annunciator built around an ESP32 microcontroller, an LCD display, DFPlayer-based audio output, status LEDs, and a rotary encoder plus mute button for user interaction. It is intended as a flexible, open-source option for proprietary alarm panels and annunciators in clinical and research environments, particularly where alarms must be aggregated, prioritized, and presented across multiple beds or devices.

The device is designed to:

- Integrate with open communication protocols (e.g., Wi-Fi and MQTT) for alarm and status messages.
- Provide audible and visual alarms with configurable patterns and volumes.
- Serve as a testbed for alarm logic and human–machine interface experiments, especially in open, collaborative research.

This article focuses on the Rev 2.1 PCB manufactured by PCBWAY and the corresponding assembly, electrical testing and factory test procedure for a batch of twenty units (US0006–US00015, LB0006–LB00011, LDN0001–LDN0005).

## Hardware description

Krake Rev 2.1 consists of:

- A custom ESP32-based PCB with power conversion, level shifting, and IO conditioning.
- A 20x4 character LCD for displaying alarm state and device information.
- An audio subsystem built around a DFPlayer-compatible MP3 module and an external speaker.
- User interface components, including a rotary encoder (with push-switch) and a dedicated mute button.
- Status and alarm LEDs to indicate power, connection state, and alarm categories.

## Bill of materials

*If your bill of materials is long or complex, you can upload the details in an editable spreadsheet (e.g., ODS, Excel, or PDF file) to an open access online location, such as the [Open Science Framework](https://osf.io/) repository.*

## Bill of materials summary

Start with receiving the subassembly KRK-PL-0002 (Krake Rev2.1 PCB, plus BOM-TBD, plus placement

| Qty | Reference(s) | Value | Datasheet | Cost | Description |
|---:|---|---|---|---:|---|
| 1 | KRK-PL-0001 | Subassembly, Krake Contract Manufacture | ~ | 1,000 | As built from parts list PCBWay |
| 1 | BZ601 | Speaker | ~ | 1 | 8 Ohm 2W Speaker, 28mm |
| 1 | J603 | DFPlayermini | [DFPlayer Mini Manual](https://image.dfrobot.com/image/data/DFR0299/DFPlayer%20Mini%20Manul.pdf) | 3 | Mini MP3 Player |
| 1 | SD601 | Micro_SD_Card_16GB | -- | 7 | SanDisk 16GB SDHC SDSDB-016G |
| 1 | D105 | LED_RED | [Lite-On RED LED](http://optoelectronics.liteon.com/upload/download/DS20-2005-253/LTW-2R3D7.pdf) | 0 | LED RED CLEAR T-1 3/4 |
| 5 | D201–D205 | LED_WHITE | [Lite-On WHITE LED](http://optoelectronics.liteon.com/upload/download/DS20-2005-253/LTW-2R3D7.pdf) | 1 | LED WHITE CLEAR T-1 3/4 |
| 4 | MF403, MF406, MF409, MF412 | Nut_4-40_0.1875 | [Keyelco Datasheet](https://www.keyelco.com/userAssets/file/M65p135.pdf) | $0.10 | #4-40 Hex Nut, 3/16\" Steel |
| 1 | U302 | LCD_20x4_Character | -- | 5 | LCD 2004 20x4 Module |
| 4 | MF401, MF404, MF407, MF410 | Screw_4-40_0.375 | [McMaster-Carr](https://www.mcmaster.com/catalog/128/3306) | 0 | 4-40 Phillips Pan Head, 3/8\" |
| 4 | MF402, MF405, MF408, MF411 | Spacer_0.1875x0.125 | [McMaster-Carr](https://www.mcmaster.com/catalog/128/3306) | 0 | Nylon Spacer, 1/8\" Length |
| 6 | MF103, MF601–MF605 | LED_Standoff | -- | NA | General Alarm LED Standoffs |
| 1 | MF801 | ENCLOSURE_KRAKE_VER1 | [McMaster-Carr](https://www.mcmaster.com/catalog/128/3306) | 1000000 | Plastic Enclosure |
| 1 | MF802 | PCB_KRAKE_VER1 | Gerbers2501181555.zip | 1000000 | PCB for Krake Rev 2.1 |
| 2 | MF701, MF702 | Jackscrews_4-40 | [McMaster-Carr](https://www.mcmaster.com/catalog/128/3306) | 0 | Male–Female Cylinder Jackscrew |
| 1 each | ENC801–ENC806 | Enclosure Parts | -- | -- | Top/bottom/front/rear panels, knob, mute cap |
| 4 | MF807–MF810 | Screw_#6x1/2 | -- | -- | PanTorx Self-Tapping |
| 4 | MF803–MF806 | Screw_2-32x3/8 | -- | -- | Self-Tapping Phillips Pan Head |
| 1 | LB801 | Krake_Model_Serial | -- | -- | Serial Number Label |

*Additional details may be found in the source design files. Material types and categories can be added in extended tables as needed.*

# Build instructions

This section documents the procedure used to complete assembly of Krake Rev 2.0 units from partially assembled boards from a contract manufacturer.

## Tools required

The following tools are required for through-hole assembly and rework:

- Sharpie or other indelible pen.
- Box cutter or similar.
- Soldering station with appropriate ventilation.
- 3/16 or 7/32 inch nut driver for the nuts on the LCD screws.
- #1 Phillips screwdriver.
- Diagonal or other flush-cutting hand tool for trimming leads.
- Basic ESD protection (wrist strap, mat) appropriate for handling microcontrollers and logic ICs.
- Assembly fixture or simple mechanical support to hold the PCB during soldering (optional but recommended).

### An Assembly Fixture
For the GPAD, We made an assembly assistant / fixture by using a raw PCB with some long #6 screws and nuts to hold at the PCB mounting points.  This same test fixture can be used for the Krake.
![Assembly Fixture View 1, no LCD](https://github.com/PubInv/general-purpose-alarm-device/blob/main/Hardware/Documentation/AssemblyFixtureView1.jpg)  

In addition four 4.40 screws with washers and 4-40 nuts to make a 1/8" spacer for holding the LCD and header for soldering.
![Assembly Fixture View 2, With LCD](https://github.com/PubInv/general-purpose-alarm-device/blob/main/Hardware/Documentation/AssemblyFixtureView2.jpg)  

An alternative is to make a 3D printed test bench
<img width="939" height="598" alt="image" src="https://github.com/user-attachments/assets/b8495141-0fc9-4d42-8993-b6f9b8f04e6c" />
The file for which is _TBD_

### LCD Bezel Grounding
On the LCD module locate the J1 and J2 solder pads which if soldered ground the bezel.  
NOTE, not all LCD modules have J1.
 ![View of J1 and J2.](https://github.com/PubInv/general-purpose-alarm-device/blob/main/Hardware/Documentation/LocateJ1J2.jpg)
Solder them.  
 ![Solder on of J1 and J2.  ](https://github.com/PubInv/general-purpose-alarm-device/blob/main/Hardware/Documentation/SolderJ1J2.jpg)  
As soldered.

## Unpacking procedure and Serial Numbering

The contract manufacturer order (approximately September 2025) provided partially assembled PCBs with all SMT components and some through-hole parts installed. These are referred to as sub-assemblies KRK-PL-0001.

Remove unit from shipping carton.

On the first unit, confirm that the PCB is marked as Rev 2.0 (or the revision for which this procedure is valid).

Create the unit’s serial number using the factory form at:  
https://nk25719.github.io/krakeFactory/factory-form.html

Print the QR Model-Serial Number label, quantity TBD.

Each PCB has a designated silkscreen area for the serial number. The assembler should:

1. Attach the model - serial number label at the provided location on the PCB.
2. Record the serial number in the manufacturing log along with the date, assembler, and test status.

The Public Invention test and assembly records database is at:  
https://nk25719.github.io/krakeFactory/records.html

Make a photograph of the unit, front and back with the model-serial QR code applied.

## Incoming Sub-assembly Inspection

Before any additional soldering:

1. Visually inspect each PCB for:
   - Obvious mechanical damage (cracks, severe warping).
   - Bent, missing, or misaligned through-hole parts.
   - Solder bridges on SMT components or incomplete joints.
   - Test 976: Inspect for correct polarity on polarized parts (Electrolytic capacitors, Integrated Circuits, Diodes). See Figure 1 below image: Inspection1, component locator.

2. Confirm that the PCB is marked as Rev 2.1 (or the revision for which this procedure is valid).

![Inspect for polarity](Inspection1.png)

### Time to start soldering

At this point install the MP3 Player sockets and the MP3 Player.

### DFPlayer module in sockets

Solder at the MP3 Player, two 1×8 socket headers at J603.

The MP3 player audio module is installed using sockets to simplify rework and replacement.

#### MP3 Header Install Steps

1. Install the DFPlayer header or socket into the designated footprint on the PCB.
2. Solder the header pins from the bottom side of the PCB.
3. On the LCD side of the PCB, trim the DFPlayer header leads so that they are flush or slightly below the surface and cannot contact the LCD module when installed.

Test the MP3 Player.

# Electrical Testing

This section describes the validation of Krake units at the end of assembly. It includes electrical safety checks (unpowered and powered tests).

## Unpowered resistance tests

Before applying power, the following resistance measurements are taken between test points and ground (TP101). The goal is to detect shorts that could endanger the power supply or the DUT.

### Measurement procedure

1. Use a digital multimeter capable of measuring resistance into the megaohm range.
2. Record the make, model, and serial number of the test instrument in the manufacturing log (for example, ASTRO DM6000AR, SSD card/N 2327610969 for units assembled in the USA).
3. For each device under test (DUT), measure:
   - TP102–TP101 (Vin to GND).
   - TP103–TP101 (+5 V to GND).
   - TP201–TP101 (VBus to GND).
   - TP202–TP101 (V3 to GND).
   - TP100–TP101 (Controller V\(_\mathrm{CC}\) to GND).
   - TP302–TP101 (+12in).

Test point locator:

![Test Points for Unpowered Tests](Inspection2.png)

### Acceptance criteria

- None of the rails should be a hard short to ground.
- Vin (TP102) to GND should typically measure greater than 10 MΩ and appear capacitive.
- +5 V (TP103) to GND should also appear non-shorted and capacitive, often in the kiloohm range due to load network.
- V3 and Controller V\(_\mathrm{CC}\) should be non-shorted and may show a few hundred ohms due to on-board circuitry.

Example measurements for units US0001–US0005 showed:

- Vin to GND: >10–14 MΩ.
- +5 V to GND: approximately 1.09–1.12 kΩ.
- V3 to GND: approximately 280–350 Ω.
- Controller V\(_\mathrm{CC}\) to GND: >10–15 MΩ.

If any unit shows significantly lower resistance than these ranges, the board should be inspected and reworked before power is applied.

Here is the end of the tests before any soldering is done.

## Testing BEFORE LCD is assembled to the PWA.

Because the sockets for the Mini MP3 Player are covered by the LCD (once it is installed), we must therefore solder the sockets and test the Mini MP3 Player BEFORE we cover the solder connections with the LCD.

Steps:

- Solder in the Mini MP3 Player at J603
- Add speaker at J601 or J604
- Apply power at USB
- Make quick powered test. **STOP IF OVER VOLTAGE** on TP103 (5V) and TP100 (3.3V).
- Program with `FactoryTest_wMenu.ino`
- Test Mini MP3 Player and Speaker (with test items: 4 DFPlayer, 5 SD (DFPlayer card), 6 Speaker)

Finish Assembly: Solder the LCD and LEDs and continue powered testing.

## LCD, ESD ground connections

To improve ESD performance and noise immunity, the LCD module includes two pads (J1 and J2) intended to be shorted together.

1. On the back of the LCD module, locate pads J1 and J2.
2. Form a small solder bridge (“ball” of solder) connecting J1 and J2.

This step ensures the LCD frame and internal reference have an appropriate ground reference.

## LCD mounting

1. Install the mechanical spacers and bolts that mount the LCD above the PCB.
2. Place the LCD module onto its header footprint, ensuring correct orientation.
3. Solder the LCD header pins to the PCB while maintaining the LCD flat and level.
4. Install the nuts on the bolts and torque them to approximately 2 in·lb (light hand-tight torque using the nut driver).

## LED installation

The front panel LEDs provide visual alarm and status indication.

1. Identify the LED footprints on the PCB and the corresponding LED colors (e.g., white vs. red).
2. For each LED:
   - Note the flat edge on the plastic body indicating the cathode.
   - Align the flat edge with the flat indicator on the PCB silkscreen.
   - Insert the LED and ensure consistent height above the PCB (a spacer may be used).
   - Solder the leads.
   - Trim the leads carefully, taking care not to damage nearby transistors or other components.
3. To avoid mixing LED colors:
   - Install and solder all white LEDs first, then close and set aside their packaging.
   - Open the red LED packaging only when needed; install, solder, and trim them as a separate step.

## Powered DC measurements

After passing the unpowered tests, each unit is powered from a current-limited bench supply.  
The following tests are to confirm that the 5V and 3.3V regulators provide the correct output voltage.  
Additional tests verify that voltage is going through the decoupling resistors to the loads on the system (usually ICs).

### Measurement procedure

1. Connect the bench supply to J101 and set:
   - Voltage: between 8 V and 12 V (test values around 8.3–8.4 V are typical).
   - Current limit: less than 1 A, with 200–300 mA used as an initial safety limit.
2. Power on the device and note the current draw.
3. Measure:
   - TP102, Vin into the +5V regulator U101.
   - TP103, +5 V from the +5V regulator U101.
   - Test point "5esp32" the input to the 3.3 V regulator (U103 input).
   - TP100, 3.3 V at U103 regulator output.
   - TP202, V3 on U501.
   - TP109, 3v3Controller supply to U102.
   - TP401, VccLCD supply for U301 and U302.
   - TP501 Mini MP3 Player supply 5VDFP at C505+.
   - TP306, +5Vraw for SPI interface.
   - TP405, Vcontrast for the LCD.
   - TP201, VBus power in on USB connector.
   - TP205, 3V3USB power to CH340 USB to UART chip.
   - J701 pin 2 (DB9), charge pump output negative.

Test points:

![Test Points for Powered Tests](Inspection3A.png)

### Typical results and criteria

Typical passing units in the first batch showed:

- Current draw at J101: approximately 150–190 mA at ~8.3–8.4 V.
- TP100, 3.3 V at U103 regulator output: approximately 3.29–3.32 V.
- TP202, V3 on U501: approximately 3.18–3.31 V.
- TP401, VccLCD supply for U301 and U302: approximately 5.0 V.
- TP501 Mini MP3 Player supply 5VDFP at C505+: approximately 5.0 V.
- J701 pin 2 (DB9), charge pump output negative: approximately –5.7 V.

Units that deviate significantly from these ranges require investigation (e.g., reflow of regulators, inspection of shorts or opens on the relevant nets).

# Operation instructions

This section covers basic procedures for powering Krake and verifying that the assembled hardware operates safely under firmware control.

## Power-up procedure

1. Connect a current-limited DC bench supply to connector J101 (Vin).
2. Set the supply to:
   - Voltage: between 8 V and 12 V (typical test value: 8.3 V).
   - Current limit: less than 1 A (typical safe upper bound: 200–300 mA for an unmuted system at idle).
3. Ensure the DFPlayer module, LCD, and all LEDs are installed.
4. Power on the bench supply and observe the current draw:
   - If current immediately rises to the current limit, power off and investigate for shorts.
   - Typical assembled boards draw on the order of 150–190 mA at 8.3–8.4 V during idle tests.
5. Once current is within an expected range, observe:
   - 5 V regulator output (~5.0 V).
   - 3.3 V regulator output (~3.3 V).
   - LCD backlight and any power/status LEDs.

## DFPlayer functional check

With the DFPlayer test firmware loaded (see below), the operator can perform a quick functional audio test:

1. Load a microSD card with the standard Krake audio files. Audio file names must start with a three- or four-digit number:
   - **Prefix:** *DFPlayer Mini* only looks at the file name prefix, which must be numerical. The remainder of the file name is ignored.
   - **Number of Digits:** *DFPlayer Mini* expects either **three** or **four** digits, depending on where you store the file.
   - **Extension:** *DFPlayer Mini* automatically looks for the file extensions `mp3`, `wav`, and `wma`.  
     Examples: `0001.mp3`, `0023.wav`, `1097 my song.mp3`, `0345 another song.wma`.

2. Insert the card into the DFPlayer module.
3. Connect the external test speaker to the Krake speaker output.
4. Load the firmware `DFPlayerTD5580ATest` onto the ESP32 (via USB–UART or OTA).
5. Reset the device.
6. Confirm that:
   - The test audio files play in the expected sequence.
   - The volume and sound quality are appropriate and free from severe distortion at nominal test levels.

## Basic user interface check

Once a final or near-final firmware build (e.g., `GPAD_API`) is loaded:

1. Confirm that the LCD powers on and displays legible text.
2. Rotate the rotary encoder and verify that menu items or values change accordingly.
3. Press the encoder push-switch and verify that selections or actions are triggered.
4. Press the mute button and verify that:
   - Audible alarms are silenced or attenuated according to the intended logic.
   - Any mute indicator LED behaves as specified.

## Final firmware validation

After completing electrical and audio tests, each unit is loaded with the intended final firmware (e.g., `GPAD_API`) for field or system testing.

For each serial number:

1. Load the final firmware.
2. Record:
   - Firmware version string.
   - MCU MAC address.
   - Date of final test and tester initials.
3. Perform a brief smoke test of:
   - Power-up behavior and LCD initialization.
   - Encoder and mute button operation.
   - At least one network interaction (e.g., connection to test Wi-Fi and MQTT broker if available).

Units that pass all steps above are considered ready for integration into larger Krake deployments or experimental setups.

**Ethics statements**  
*To be completed if human or animal data are involved (likely not applicable for this purely hardware manufacturing article).*

**CRediT author statement**  
*To be completed (e.g., Conceptualization, Methodology, Software, Validation, Writing, Supervision, etc.).*

**Acknowledgements**  
*To be completed with collaborators, institutions, and funding information.*

**References**  
*To be completed with references to related open hardware, alarm standards, and the Krake/GPAD repositories or previous publications.*
