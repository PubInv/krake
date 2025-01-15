#  View KiCad files with on line viewer 
View both cchematic and PCB files with Kicanvas
These are Rev 2 files.

##  View Schematic on Line

[PCBa_KRAKE%2FPCBa_KRAKE.sch](https://kicanvas.org/?github=https%3A%2F%2Fgithub.com%2FPubInv%2Fkrake%2Fblob%2Fmain%2FPCBa_KRAKE%2FPCBa_KRAKE.sch)

[AlarmAudio.kicad_sch](https://kicanvas.org/?github=https%3A%2F%2Fgithub.com%2FPubInv%2Fgeneral-alarm-device%2Fblob%2Fmain%2FHardware%2FGeneralPurposeAlarmDevicePCB%2FAlarmAudio.kicad_sch)  

[AlarmLights5.kicad_sch](https://kicanvas.org/?github=https%3A%2F%2Fgithub.com%2FPubInv%2Fgeneral-alarm-device%2Fblob%2Fmain%2FHardware%2FGeneralPurposeAlarmDevicePCB%2FAlarmLights5.kicad_sch)  

[LCD And I2C Interface.kicad_sch](https://kicanvas.org/?https%3A%2F%2Fgithub.com%2FPubInv%2Fgeneral-alarm-device%2Fblob%2Fmain%2FHardware%2FGeneralPurposeAlarmDevicePCB%2FLCD+And+I2C+Interface.kicad_sch=&github=https%3A%2F%2Fgithub.com%2FPubInv%2Fgeneral-alarm-device%2Fblob%2Fmain%2FHardware%2FGeneralPurposeAlarmDevicePCB%2FLCD%2520And%2520I2C%2520Interface.kicad_sch)  

[SPI Peripherial.kicad_sch](https://kicanvas.org/?https%3A%2F%2Fgithub.com%2FPubInv%2Fgeneral-alarm-device%2Fblob%2Fmain%2FHardware%2FGeneralPurposeAlarmDevicePCB%2FSPI+Peripherial.kicad_sch=&github=https%3A%2F%2Fgithub.com%2FPubInv%2Fgeneral-alarm-device%2Fblob%2Fmain%2FHardware%2FGeneralPurposeAlarmDevicePCB%2FSPI%2520Peripherial.kicad_sch)  

[USB_UART.kicad_sch](https://kicanvas.org/?github=https%3A%2F%2Fgithub.com%2FPubInv%2Fgeneral-alarm-device%2Fblob%2Fmain%2FHardware%2FGeneralPurposeAlarmDevicePCB%2FUSB_UART.kicad_sch)  


##  View PCB on Line
[GPAD Rev 2 PCB](https://kicanvas.org/?github=https%3A%2F%2Fgithub.com%2FPubInv%2Fgeneral-alarm-device%2Fblob%2Fmain%2FHardware%2FGeneralPurposeAlarmDevicePCB%2FGeneralPurposeAlarmDevicePCB.kicad_pcb)

##  SPI Peripherial Pins  
The connector is JR12 6P6C and pin 1 is on the left when the connector is held with the contact pins up and facing the observer.  
Note the GPAD is a 5V logic level peripheral device. Level shifters must be used when controlled by a 3.3V device.  
| **Pin#** 	| **Signal Name** 	| **Note**                                            	|
|----------	|-----------------	|-----------------------------------------------------	|
| 1        	| nCS             	| Active low                                          	|
| 2        	| COPI            	| Input to GPAD                                       	|
| 3        	| GND             	|                                                     	|
| 4        	| SCK             	| Input to GPAD                                       	|
| 5        	| ControllerVcc   	| 5 Volts, an optional way to put power into the GPAD 	|
| 6        	| CIPO            	| Output from GPAD, 5V.                               	|
