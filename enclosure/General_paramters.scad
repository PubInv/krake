AC_button_x = 38.77;
AC_button_y = 44.44;

KrakeEnclosureVersion = 3.1; // change this with each rev

/* Project Selector */
Krake= 0;      // [0:Off, 1:On] //on // for the 28 mm speaker
GPAD  = 0;      // [0:Off, 1:On]
Krake_rev2_76mmSPK= 1;  // 76 mm / 3 inch speaker

if (Krake + GPAD > 1)
echo("WARNING: More than one project mode active!!!");
////////////////////////////////////////////////////////////////////
// Export Options
////////////////////////////////////////////////////////////////////
GPAD_TShell          = 0;
GPAD_BShell          = 1; //2 w/LCD 
GPAD_FPanL           = 0;//3 bottom
GPAD_BPanL           = 0; //4 top
sd_door_on_off       = 0;
recessed_bottom_on_off = 0;// turn on/off the recess moduel only
recessed_module_on_off =0;
//// recess sub modules when recess module is on///


////////////////////////////////////////////////////////////////////
BButton              = 0 ;
RotaryEncoder        = 0;  // change to a real rotary encoder 
T_BShellScrew        = 0;
BOSSScrew            = 0;
PCB_SIMPLE           = 0;
PWA_GPAD             = 0;
PWA_KRAKE            = 0;//pcb
LED_Standoff         = 0;
LED_Standoff_Single  = 0;
PWA                  = 0;
SPK                  = 0;
HEAT_SET_INSERTS     = 0;
SPKLid               = 0;
Krake_76mmSPK_56h    = 1;  // turn off if using Cricklewood Speaker 40 mm height and if using 28 mm speaker 
GPAD_TshellDoorRecess = 1; // turn on/off door when krake Tshell is on 
GPAD_TShellWithVESA  = 1; // Krake TShell 
recessed_wall_on_off = 0;
//////////////////////////////
Krake_76mmSPK_56h    = 1;  // turn off if using Cricklewood Speaker 40 mm height and if using 28 mm speaker  
////////////////////////////////////////////////////////////////////
speaker_ring = 1;
// Connectors Modifications specific port logic
//if krake true x: y; i.e if krake on, usbbon =0, aka off
USBbOn    = Krake ? 0 : 1;
USBcOn    = Krake ? 1 : 0;
DE9On     = Krake ? 1 : 0;
I2COn     = Krake ? 0 : 1;
RJ12On    = Krake ? 1 : 1;
DCOn      = Krake ? 1 : 1;
//DE9SquareHole = Krake ? 1 : 1;
speaker_clamp = Krake ? 0 : 1;
grill_pattern = Krake ? 0 : 1;
SpeakerEnclosure  = Krake ? 0 : 1;
SpeakerCutOut  = Krake ? 0 : 1;
////////////////////////////////////////////////////////////////////
USBbOn    = Krake_rev2_76mmSPK ? 0 : 1;
USBcOn    = Krake_rev2_76mmSPK ? 1 : 0;
DE9On     = Krake_rev2_76mmSPK ? 1 : 0;
I2COn     = Krake_rev2_76mmSPK ? 0 : 1;
RJ12On    = Krake_rev2_76mmSPK ? 1 : 1;
DCOn      = Krake_rev2_76mmSPK ? 1 : 1;
speaker_clamp = Krake_rev2_76mmSPK ? 1 : 0;
grill_pattern = Krake_rev2_76mmSPK ? 1 : 0;
SpeakerEnclosure  = Krake_rev2_76mmSPK ? 1 : 0;
SpeakerCutOut  = Krake_rev2_76mmSPK ? 1 : 0;
SPKBOSS62 = Krake_rev2_76mmSPK ? 1 : 0;
SPKBOSSpunch62 = Krake_rev2_76mmSPK ? 1 : 0;
// Common Parameters - Base settings shared by all configurations
////////////////////////////////////////////////////////////////////
// speaker parameters
SpeakerDiameter_mm= Krake_rev2_76mmSPK? 78 : 27; 
SpeakerRadius_mm=SpeakerDiameter_mm/2;
SpeakerHeight_mm = Krake_rev2_76mmSPK ? 40 : 0; 
////////////////////////////////////////////////////////////////////
translationVariable = Krake_rev2_76mmSPK ? 2.2 * SpeakerHeight_mm -5: 0 ;
////////////////////////////////////////////////////////////////////
Length         = 83.82 + 13 +translationVariable;  //x axis    
Width          = 138 + 13; //y axis axis            
Height         = Krake_76mmSPK_56h? 48 + SpeakerHeight_mm/2 : 28 + SpeakerHeight_mm/2; //z axis was 40
Thick          = 2;                    // Wall thickness
Filet          = 2;                    // Corner rounding
Resolution     = 10;                   // Filet smoothness
m              = 0.9;                  // Panel/rail tolerance
PCBFeet        = 1;                    // Enable PCB feet
Vent           = 1;                    // Enable vents
Vent_width     = 1.5;
////////////////////////////////////////////////////////////////////
// PCB Parameters - Common
PCBPosX        = 0 + translationVariable;
PCBPosY        = 0;
PCBLength      = 83.82;
PCBWidth       = 137.16;
PCBThick       = 1.6;
////////////////////////////////////////////////////////////////////
// BOSS/Foot Parameters 
FootHeight     = 15;
FootDia        = 7;
FootHole       = 3.0;
FootPosX       = 5.08;
FootPosY       = 5.08;
////////////////////////////////////////////////////////////////////
// Decorative Parameters
Couleur1       = "Orange";
Couleur2       = "OrangeRed";
Couleur3       = "Green";
Dec_Thick      = Vent ? Thick * 2 : Thick;
Dec_size       = Vent ? Thick * 2 : 0.8;
////////////////////////////////////////////////////////////////////
// Speaker location parameters
SpeakerPositionX = Length/4;
SpeakerPositionY =Width/2;
SpeakerPositionZ = 0-Thick;
SpeakerHolePosX =Thick+m/2 -FootHeight;
SpeakerHolePosY = SpeakerPositionY;
SpeakerHolePosZ = SpeakerPositionZ+ SpeakerDiameter_mm/2;
Angle_deg =90;
////////////////////////////////////////////////////////////////////
//Modifications for Display
DisplayXpos = PCBLength-50.8  + translationVariable;
DisplayYpos = 71.12;
DisplayLenght = 26;
DisplayWidth = 76;
DisplayFilet = 0;
////////////////////////////////////////////////////////////////////
//Modifications for muteButton
muteButtonXpos = PCBLength-10  + translationVariable;
muteButtonYpos = 31.75;
muteButtonDiameter = 15;
////////////////////////////////////////////////////////////////////
//Modifications for SpeakerHole
SpeakerHoleY = Krake ? PCBWidth*.769 : 15.24;
SpeakerHoleX = translationVariable + ( Krake ? PCBLength*.815 :PCBLength - 15.24 );
////////////////////////////////////////////////////////////////////
//Parameters for LEDHole
LEDspacing = 12.7 ;
LEDYposOffset = 15.24  ; // offset from the Encoder edge of PCB
LEDXposOffset = 27.94  ; // offset from the connector edge of PCB
////////////////////////////////////////////////////////////////////
// Krake Modifications for RotaryEncodeHole
i=-1; // Encoder is one LEd spacing below LEDs
RotaryEncoderXpos      = Krake ? PCBLength-(LEDXposOffset+LEDspacing*i) : translationVariable + PCBLength-(LEDXposOffset+LEDspacing*i);
RotaryEncoderYpos      = 15.24 ;
RotaryEncoderDiameter  = 16 ;
//For instructions on mute Button inscription go to line 594.
///////// recess paramters //////////
R_height = Krake_76mmSPK_56h? Height - 40: 9;
button_pins_height = Krake_76mmSPK_56h? 12.8  : 6.85;
Door_recess_x_offset = Krake_rev2_76mmSPK? translationVariable - PCBLength+28 : -6.3;
Door_recess_y_offset = Krake_rev2_76mmSPK? PCBWidth  -93.5: 36.5;
///////////////////////////////////////
