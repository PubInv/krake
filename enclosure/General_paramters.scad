KrakeEnclosureVersion = 3.1; // change this with each rev

/* Project Selector */
Krake= 0;      // [0:Off, 1:On] //on // for the 28 mm speaker
GPAD  = 0;      // [0:Off, 1:On]
Krake_rev2_76mmSPK= 1;  // 76 mm / 3 inch speaker

if (Krake + GPAD > 1)
echo("WARNING: More than one project mode active!!!");

//////////////////////////////
Krake_76mmSPK_56h    = 1;  // turn off if using Cricklewood Speaker 40 mm height and if using 28 mm speaker  
////////////////////////////////////////////////////////////////////
// Connectors Modifications specific port logic
//if krake true x: y; i.e if krake on, usbbon =0, aka off

//DE9SquareHole = Krake ? 1 : 1;
speaker_clamp = Krake ? 0 : 1;
grill_pattern = Krake ? 0 : 1;
SpeakerEnclosure  = Krake ? 0 : 1;
SpeakerCutOut  = Krake ? 0 : 1;
/////////////////////////////////////////////////speaker_clamp = Krake_rev2_76mmSPK ? 1 : 0;
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
///////////////////
// Decorative Parameters
Couleur1       = "Orange";
Couleur2       = "OrangeRed";
Couleur3       = "Green";
Dec_Thick      = Vent ? Thick * 2 : Thick;
Dec_size       = Vent ? Thick * 2 : 0.8;
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// BOSS/Foot Parameters 
FootHeight     = 15;
FootDia        = 7;
FootHole       = 3.0;
FootPosX       = 5.08;
FootPosY       = 5.08;
/////////////////////////////////////////////////
///////////////////

// Speaker location parameters
SpeakerPositionX = Length/4;
SpeakerPositionY =Width/2;
SpeakerPositionZ = 0-Thick;
SpeakerHolePosX =Thick+m/2 -FootHeight;
SpeakerHolePosY = SpeakerPositionY;
SpeakerHolePosZ = SpeakerPositionZ+ SpeakerDiameter_mm/2;
Angle_deg =90;
//Modifications for muteButton
muteButtonXpos = PCBLength-10  + translationVariable;
muteButtonYpos = 31.75;
muteButtonDiameter = 15;
