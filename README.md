# Krake
A wireless alarm device which makes loud noises and flashes lights to alert a human

# WiFi enablement

# Arbirtrary Sonic Alarms


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




# Components: 
ESP32
DFplayer 
SD card 16 GB 
speaker 8 ohm 
5 lamps 
8 resistors 
breadboard (solderless )
connecting wires
USB cable

# connections ft. ESP32

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


  > KRAKE 20240511


# References
