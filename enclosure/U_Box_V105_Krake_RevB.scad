//File: U_Box_V105_Krake_RevB.scad
//Modified by Public Invention to make enclosures for:
// GPAD
// KRAKE (TM)

////////////////////////////////////////////////////////////////////
//  Heartman/Hearty OpenSCAD Parametric Box - MODIFIED BY NAGHAM //
//  Krake & GPAD Configurable Enclosure Generator                //
//  License: CC BY-NC 3.0                                        //
////////////////////////////////////////////////////////////////////




// Thanks to RonRN18 for this Creative Commons Attribution set of Heat-set insert:
// https://www.thingiverse.com/thing:5849866
include <StudModules.scad>




KrakeEnclosureVersion = 2.1; // change this with each rev

/* Project Selector */
Krake= 1;      // [0:Off, 1:On] //on // for the 28 mm speaker
GPAD  = 0;      // [0:Off, 1:On]
Krake_rev2_76mmSPK= 1;  // 76 mm / 3 inch speaker
////////////////////////////////////////////////////////////////////
// Export Options
////////////////////////////////////////////////////////////////////

//

GPAD_TShell          = 0;
GPAD_TShellWithVESA  = 0; //1 back of unit
GPAD_BShell          = 1; //2 w/LCD 
GPAD_FPanL           = 0;//3 bottom
GPAD_BPanL           = 0; //4 top
BButton              = 0 ;
RotaryEncoder        = 0;  // change to a real rotary encoder 
T_BShellScrew        = 0;
BOSSScrew            = 0;
PCB_SIMPLE           = 0;
PWA_GPAD             = 0;
PWA_KRAKE            = 1;//pcb
LED_Standoff         = 0;
LED_Standoff_Single  = 0;
PWA                  = 0;
SPK                  = 0;
HEAT_SET_INSERTS     = 0;
SPKLid               = 0;
Krake_76mmSPK_56h    = 1;  // turn off if using Cricklewood Speaker 40 mm height and if using 28 mm speaker 

///////////////// speakers //////////
//SoundLabSPK = 1;


////////////////////////////////////////////////////////////////////
// Common Parameters - Base settings shared by all configurations
////////////////////////////////////////////////////////////////////
// speaker parameters
SpeakerDiameter_mm= Krake_rev2_76mmSPK? 78 : 27; 
SpeakerRadius_mm=SpeakerDiameter_mm/2;
SpeakerHeight_mm = Krake_rev2_76mmSPK ? 40 : 0; 
 
 


translationVariable = Krake_rev2_76mmSPK ? 4 * SpeakerHeight_mm -5: 0 ;
////////////////////////////////////////////////////////////////////

Length         = 83.82 + 13 +translationVariable;  //x axis    
Width          = 138 + 13; //y axis axis            
Height         = Krake_76mmSPK_56h? 90 + SpeakerHeight_mm/2 : 40 + SpeakerHeight_mm/2; //z axis was 40
Thick          = 2;                    // Wall thickness
Filet          = 2;                    // Corner rounding
Resolution     = 50;                   // Filet smoothness
m              = 0.9;                  // Panel/rail tolerance
PCBFeet        = 1;                    // Enable PCB feet
Vent           = 1;                    // Enable vents
Vent_width     = 1.5;


////////////////////////////////////////////////////////////////////
// PCB Parameters - Common
////////////////////////////////////////////////////////////////////

PCBPosX        = 0 + translationVariable;
PCBPosY        = 0;
PCBLength      = 83.82;
PCBWidth       = 137.16;
PCBThick       = 1.6;

////////////////////////////////////////////////////////////////////
// BOSS/Foot Parameters 
////////////////////////////////////////////////////////////////////

FootHeight     = 15;
FootDia        = 7;
FootHole       = 3.0;
FootPosX       = 5.08;
FootPosY       = 5.08;

////////////////////////////////////////////////////////////////////
// Decorative Parameters
////////////////////////////////////////////////////////////////////

Couleur1       = "Orange";
Couleur2       = "OrangeRed";
Couleur3       = "Green";

Dec_Thick      = Vent ? Thick * 2 : Thick;
Dec_size       = Vent ? Thick * 2 : 0.8;

////////////////////////////////////////////////////////////////////
// Speaker location parameters
////////////////////////////////////////////////////////////////////
SpeakerPositionX = Length/4;
SpeakerPositionY =Width/2;
SpeakerPositionZ = 0-Thick;

SpeakerHolePosX =Thick+m/2 -FootHeight;
SpeakerHolePosY = SpeakerPositionY;
SpeakerHolePosZ = SpeakerPositionZ+ SpeakerDiameter_mm/2;
Angle_deg =90;

//Modifications for Display
DisplayXpos = PCBLength-50.8  + translationVariable;
DisplayYpos = 71.12;
DisplayLenght = 26;
DisplayWidth = 76;
DisplayFilet = 0;

//Modifications for muteButton
muteButtonXpos = PCBLength-10  + translationVariable;
muteButtonYpos = 31.75;
muteButtonDiameter = 15;

//Modifications for SpeakerHole
SpeakerHoleY = Krake ? PCBWidth*.769 : 15.24;
SpeakerHoleX = translationVariable + ( Krake ? PCBLength*.815 :PCBLength - 15.24 );

//Parameters for LEDHole
LEDspacing = 12.7 ;
LEDYposOffset = 15.24  ; // offset from the Encoder edge of PCB
LEDXposOffset = 27.94  ; // offset from the connector edge of PCB


// Krake Modifications for RotaryEncodeHole
i=-1; // Encoder is one LEd spacing below LEDs
RotaryEncoderXpos      =  translationVariable +( Krake ? PCBLength-(LEDXposOffset+LEDspacing*i) : 0 );
RotaryEncoderYpos      = Krake ? 15.24 : 0;
RotaryEncoderDiameter  = Krake ? 16 : 0;

// Connectors Modifications specific port logic
//if krake true x: y; i.e if krake on, usbbon =0, aka off
USBbOn    = Krake ? 0 : 1;
USBcOn    = Krake ? 1 : 0;
DE9On     = Krake ? 1 : 0;
I2COn     = Krake ? 0 : 1;
RJ12On    = Krake ? 1 : 1;
DCOn      = Krake ? 1 : 1;
//DE9SquareHole      = Krake ? 1 : 0;


speaker_clamp = Krake ? 0 : 1;
grill_pattern = Krake ? 0 : 1;
SpeakerEnclosure  = Krake ? 0 : 1;
SpeakerCutOut  = Krake ? 0 : 1;
 SPKBOSS62 = Krake ? 0 : 0;
SPKBOSSpunch62 = Krake ? 0 : 0;

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

 

if (Krake + GPAD > 1)
    echo("WARNING: More than one project mode active!!!");

//For instructions on mute Button inscription go to line 594.

// Import 3mm Plastite Screw
if(BOSSScrew ==1){
    translate ([PCBLength-70.74,12.24,25]) //is this just to reposition the screw to the correct place?, where are these numbers from?
    //translate ([0,0,FootHeight+ScrewLenght+1])
    import("MCMaster_Carr_Pan_Head_Screw_99461a941.stl");
}
/*
  if(PWAKrake ==1){ 
  translate([-10,-50,10])
  rotate ([0,0,90])
  color([1, 0, 0]) {
  import("KRAKE_PWArev1.stl");
  }}
*/

/////////// - Boitier générique bord arrondis - Generic rounded box - //////////

module RoundBox($a=Length, $b=Width, $c=Height){// Cube bords arrondis
    $fn=Resolution;            
    translate([0,Filet,Filet]){  
        minkowski (){                                              
            cube ([$a-(Length/2),$b-(2*Filet),$c-(2*Filet)], center = false);
            rotate([0,90,0]){    
                cylinder(r=Filet,h=Length/2, center = false);
            } 
        }
    }
}// End of RoundBox Module
//strange cube sizing to mitigate the fact after minkowski the box would get larger as addition of cylinder size plus cube size thus desired length/2, desired width - 2 times the extra added because of the cylinder
//confused on the translate and rotate requirements , when you rotate it it drops below the line so you need to translate it up to keep it on the right plane, the rotate means the cylinder is on the longest edge off the cube


////////////////////////////////// - Module Coque/Shell - //////////////////////////////////         

module Coque(){//Coque - Shell  
    Thick = Thick*2;  
    difference(){    
        difference(){//sides decoration
            union(){    
                difference() {//soustraction de la forme centrale - Substraction Fileted box
                //??unsure why subtracting a fileted box after subtracting a cube
                    difference(){//soustraction cube median - Median cube slicer
                        union() {//union               
                            difference(){//Coque    
                                RoundBox();
                                translate([Thick/2,Thick/2,Thick/2]){     
                                    RoundBox($a=Length-Thick, $b=Width-Thick, $c=Height-Thick);
                                }
                            }//Fin diff Coque                            
                            difference(){//largeur Rails        
                                translate([Thick+m,Thick/2,Thick/2]){// Rails
                                    RoundBox($a=Length-((2*Thick)+(2*m)), $b=Width-Thick, $c=Height                             -(Thick*2));
                                }//fin Rails
                                translate([((Thick+m/2)*1.55),Thick/2,Thick/2+0.1]){ // +0.1 added to avoid the artefact
                                    RoundBox($a=Length-((Thick*3)+2*m), $b=Width-Thick, $c=Height-Thick);
                                }           
                            }//Fin largeur Rails
                        }//Fin union                                   
                        translate([-Thick,-Thick,Height/2]){// Cube à soustraire
                            cube ([Length+100, Width+100, Height], center=false);
                        }                                            
                    }//fin soustraction cube median - End Median cube slicer
                    translate([-Thick/2,Thick,Thick]){// Forme de soustraction centrale 
                        RoundBox($a=Length+Thick, $b=Width-Thick*2, $c=Height-Thick);       
                    }                          
                }                                          


                difference(){// wall fixation box legs
                    union(){
                        translate([3*Thick +5,Thick,Height/2]){
                            rotate([90,0,0]){
                                $fn=6;
                                cylinder(d=16,Thick/2);
                            }   
                        }

                        translate([Length-((3*Thick)+5),Thick,Height/2]){
                            rotate([90,0,0]){
                                $fn=6;
                                cylinder(d=16,Thick/2);
                            }   
                        }

                    }
                    translate([4,Thick+Filet,Height/2-57]){   
                        rotate([45,0,0]){
                            cube([Length,40,40]);    
                        }
                    }
                    translate([0,-(Thick*1.46),Height/2]){
                        cube([Length,Thick*2,10]);
                    }
                } //Fin fixation box legs
            }

            union(){// outbox sides decorations

                for(i=[0:Thick:Length/4]){

                    // Ventilation holes part code submitted by Ettie - Thanks ;) 
                    translate([10+i,-Dec_Thick+Dec_size,1]){
                        cube([Vent_width,Dec_Thick,Height/4]);
                    }
                    translate([(Length-10) - i,-Dec_Thick+Dec_size,1]){
                        cube([Vent_width,Dec_Thick,Height/4]);
                    }
                    translate([(Length-10) - i,Width-Dec_size,1]){
                        cube([Vent_width,Dec_Thick,Height/4]);
                    }
                    translate([10+i,Width-Dec_size,1]){
                        cube([Vent_width,Dec_Thick,Height/4]);
                    }


                }// fin de for
                // }
            }//fin union decoration
        }//fin difference decoration


        union(){ //sides holes
            $fn=50;
            translate([3*Thick+5,20,Height/2+4]){
                rotate([90,0,0]){
                    cylinder(d=2,20);
                }
            }
            translate([Length-((3*Thick)+5),20,Height/2+4]){
                rotate([90,0,0]){
                    cylinder(d=2,20);
                }
            }
            translate([3*Thick+5,Width+5,Height/2-4]){
                rotate([90,0,0]){
                    cylinder(d=2,20);
                }
            }
            translate([Length-((3*Thick)+5),Width+5,Height/2-4]){
                rotate([90,0,0]){
                    cylinder(d=2,20);
                }
            }
        }//fin de sides holes

    }//fin de difference holes
}// fin coque 

////////////////////////////// - Experiment - ///////////////////////////////////////////


//Speaker Grill//

if(SPK==1){
    color(c=[0,0,2.8])
    translate([PCBLength*.91,PCBWidth*.82,3])

    import("Speaker2W-SpeakerOutline.stl");
}
module grill_pattern() {

translate ([SpeakerPositionX,SpeakerPositionY,SpeakerPositionZ]){
wall_thickness = 2;
grill_bar_width = 1;
    // The pattern consists of intersecting cubes (bars)
  scale (1.2){ 
   union() {
        // Horizontal bars (along X-axis)
        for (i = [-SpeakerDiameter_mm/2 : (grill_bar_width + wall_thickness) : SpeakerDiameter_mm/2]) {
            translate([i, 0, 0])
                cube([grill_bar_width, SpeakerDiameter_mm, Thick + 1], center=true);
        }
        // Vertical bars (along Y-axis)
        for (j = [-SpeakerDiameter_mm/2 : (grill_bar_width + wall_thickness) : SpeakerDiameter_mm/2]) {
            translate([0, j, 0])
                cube([SpeakerDiameter_mm, grill_bar_width, Thick + 1], center=true);
        }
    }
}
}
}

module SpeakerHexGrill(Krake_rev2_76mmSPK, Cx, Cy, Cdia=SpeakerDiameter_mm*1.5, h=Thick, cell=6, wall=1){
   
   if (Krake_rev2_76mmSPK == 1)
    translate([Cx, Cy, 0])
    intersection(){
        cylinder(d=Cdia*0.95, h=h, $fn=100);

        for (x=[-Cdia/2:cell:Cdia/2])
            for (y=[-Cdia/2:cell:Cdia/2])
                translate([x + (y%2)*cell/2, y*0.866, 0])
                    difference(){
                        cylinder(r=cell/2, h=h, $fn=6);
                        cylinder(r=(cell/2)-wall, h=h+0.2, $fn=6);
                    }
    }
}


module SpeakerHole(Krake_rev2_76mmSPK,Cx,Cy,Cdia,Ccenter=false){
    //difference(){
    if(Krake_rev2_76mmSPK==1)
                  translate([Cx ,Cy ,-1]){
            for(j = [1  : 3]){

                rotate(a = 360*j/3,v = [0,0,1])
                for(i = [1.8 : Cdia/60 : Cdia/1.5]){
                    rotate(a = 1*(i/Cdia)*360,v = [0,0,1])
                    translate([i,i,0])

                    cylinder(d=i*.65,h = 10, $fn=100,center=Ccenter);
                    //echo(i);
               }
            }
          
        }
       
        }
    
  
       
{// Parameters
    speaker_diameter = 25.5; // mm
    speaker_thickness = 5; // mm (adjust if needed)
    clamp_width = 6;       // Hieght of clamp ring
    flexure_thickness = 1; // Thickness of flexure arms
    flexure_gap = 1.5;     // Gap to allow flex movement
    base_thickness = 0;    // Base plate thickness
    overlap = -1;           // Small overlap to hold speaker
    clamp_Z = 1.7 ; // Zpos of the clamp, interesecting with the enclosure shell
    $fn = 100; // Smoothness

    module speaker_clamp() {
        difference() {
            // Outer clamp ring
            translate([PCBLength*.91,PCBWidth*.82,clamp_Z])
            #cylinder(h = clamp_width-.001, r = (speaker_diameter/2) + flexure_gap + flexure_thickness);

            // Inner clearance for speaker
            translate([PCBLength*.91,PCBWidth*.82,clamp_Z])
            translate([0,0,overlap])
            #cylinder(h = clamp_width +0.2 - overlap, r = (speaker_diameter/2) + flexure_gap);

            // Create flexure cuts
            translate([PCBLength*.91,PCBWidth*.82,clamp_Z])
            for (angle = [0,90,180,270]) {
                rotate([0,0,angle])
                translate([speaker_diameter/2, -flexure_thickness/2, 0])
                #cube([flexure_gap*2, flexure_thickness +7, clamp_width +4]);
            }
        }
        
        // Base platform
        translate([PCBLength*.87,PCBWidth*.82,clamp_Z])
        translate([0,0,-base_thickness])
        cylinder(h = base_thickness , r = (speaker_diameter/2) + flexure_gap  + flexure_thickness + 3);
    }

//    // Build the model
//    speaker_clamp();
}


//LED Spacer
module LedSpacer(OnOff,Cx,Cy,Cdia,Cpitch,Cheight,Ccenter=false){ 

    translate([Cx,Cy,0])
    difference(){
        cylinder(d=Cdia,Cheight, $fn=50,center=Ccenter); //spacer body
        union(){
            hull(){
                translate([Cpitch/2,0,-0.5])
                cylinder(d=1.3,h=Cheight+1, $fn=50,center=Ccenter); //leg1
                translate([Cdia/2,0,-0.5])
                cylinder(d=1.3,h=Cheight+1, $fn=50,center=Ccenter); //leg2
            }
            hull(){
                translate([-Cpitch/2,0,-0.5])
                cylinder(d=1.3,h=Cheight+1, $fn=50,center=Ccenter); //leg1
                translate([-Cdia/2,0,-0.5])
                cylinder(d=1.3,h=Cheight+1, $fn=50,center=Ccenter); //leg2
            }
        }
    }
}

//Speaker Holder
module SpeakerHolder(Krake_rev2_76mmSPK,Cx,Cy,Cdia,Ccenter=false){ 
    if(Krake_rev2_76mmSPK==1){
        translate([Cx,Cy,-Thick])
        difference(){
            cylinder(d=Cdia+Thick,10, $fn=50,center=Ccenter);
            cylinder(d=Cdia,10, $fn=50,center=Ccenter);
        }
    }
}
//Button
module ButtonSwitch(OnOff,Cx,Cy,Cdia,Ccenter=false){
    //difference(){
    if(OnOff==1)
        translate([Cx,Cy,-1]){
            //rotate([180,0,0])



            ThreadThick = 0.20;
            //ThreadWidth = 0.40;

            Protrusion = 0.9;

            HoleWindage = 0.25;

            //------
            // Dimensions

            Post = [3.86,3.86,3.0];

            OD = 0;
            HEIGHT =  FootHeight;
            DOMEHEIGHT = 1;

            Button = [12,0+Post[2],6*ThreadThick];

            NumSides = 8*4;

            //----------------------
            //- Build it

            difference() {
                union() {
                    //retension ring
                    translate([0,0,4*Thick])
                    cylinder(d=Cdia+Thick,h=Thick,$fn=NumSides);
                    //dome 
                    translate([0,0,0])
                    resize([0,0,2*Button[DOMEHEIGHT]])
                    sphere(d=(Cdia-m/2)/cos(180/NumSides),$fn=NumSides);
                    //Post
                    cylinder(d=Cdia-m/2,h=FootHeight,$fn=NumSides);

                }
                //retension hole
                translate([0,0,(FootHeight-Post[2]/2) ]){
                    cube(Post + [HoleWindage,HoleWindage,Protrusion],center=true);
                }
            }
        }
}


/////////////////////// - Foot with base filet - /////////////////////////////
module foot(FootDia,FootHole,FootHeight){
    Filet=2;
    color(Couleur1)   
    // translate([3*Thick+2,Thick+5,0]+[-FootPosX,-FootPosY,Filet])
    difference(){

        difference(){
            translate ([0,0,0.1]){
                cylinder(d=FootDia+Filet,FootHeight+Thick, $fn=100,center=true);
            }
            rotate_extrude($fn=100){
                translate([(FootDia+Filet*2)/2,Filet,0]){
                    minkowski(){
                        square(10);
                        circle(Filet, $fn=100);
                    }
                }
            }
        }
        cylinder(d=FootHole,FootHeight+1, $fn=100);
    }          
}// Fin module foot

module Feet(){     


    ////////////////////////////// - 4 Feet - //////////////////////////////////////////   
    //    
    //    translate([3*Thick+7,Thick+10,Thick/2]){
    //        foot(FootDia,FootHole,FootHeight);
    //    }
    //    translate([(3*Thick)+PCBLength+7,Thick+10,Thick/2]){
    //        foot(FootDia,FootHole,FootHeight);
    //        }
    //    translate([(3*Thick)+PCBLength+7,(Thick)+PCBWidth+10,Thick/2]){
    //        foot(FootDia,FootHole,FootHeight);
    //        }        
    //    translate([3*Thick+7,(Thick)+PCBWidth+10,Thick/2]){
    //        foot(FootDia,FootHole,FootHeight);
    //    }   


    translate([3*Thick+2,Thick+5,Thick/2-.01]){
        translate([FootPosX,FootPosY,FootHeight/2]){
            foot(FootDia,FootHole,FootHeight);
        }
        translate([(PCBLength-FootPosX),FootPosY,FootHeight/2]){
            foot(FootDia,FootHole,FootHeight);
        }
        translate([(PCBLength-FootPosX),(PCBWidth-FootPosY),FootHeight/2]){
            foot(FootDia,FootHole,FootHeight);
        }        
        translate([FootPosX,PCBWidth-(FootPosY),FootHeight/2]){
            foot(FootDia,FootHole,FootHeight);
        }
        //  translate([(PCBLength-FootPosX),(PCBWidth-68.58),FootHeight/2]){
        //   foot(FootDia,FootHole,FootHeight);
        // }
    }
} // Fin du module Feet




////////////////////////////////////////////////////////////////////////
////////////////////// <- Holes Panel Manager -> ///////////////////////
////////////////////////////////////////////////////////////////////////

//                           <- Panel ->  
module Panel(Length,Width,Thick,Filet){
    scale([0.5,1,1])
    minkowski(){
        cube([Thick,Width-(Thick*2+Filet*2+m),Height-(Thick*2+Filet*2+m)]);
        translate([0,Filet,Filet])
        rotate([0,90,0])
        cylinder(r=Filet,h=Thick, $fn=100);
    }
}
//                          <- Circle Spacer -> 
// Cx=Cylinder X position | Cy=Cylinder Y position | Cdia= Cylinder dia | Cheight=Cyl height
module CylinderSpacer(OnOff,Cx,Cy,Cdia,Ccenter=false){
    if(OnOff==1)
        translate([Cx,Cy,Thick])
    cylinder(d=Cdia,10, $fn=50,center=Ccenter);
}

//This is for all the circular holes.
//                          <- Circle hole -> 
// Cx=Cylinder X position | Cy=Cylinder Y position | Cdia= Cylinder dia | Cheight=Cyl height
module CylinderHole(OnOff,Cx,Cy,Cdia,Ccenter=false){
    if(OnOff==1)
        translate([Cx,Cy,-1])
    cylinder(d=Cdia,10, $fn=50,center=Ccenter);
}
//                          <- Square hole ->  
// Sx=Square X position | Sy=Square Y position | Sl= Square Length | Sw=Square Width | Filet = Round corner
module USBbSquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
    if(OnOff==1)
        minkowski(){
            translate([Sx+Filet/2,Sy+Filet/2,-1])
            cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
            cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
        }
}
module USBcSquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
    if(OnOff==1)
        minkowski(){
            translate([Sx+Filet/2,Sy+Filet/2,-1])
            cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
            cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
        }
}
module I2CSquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
    if(OnOff==1)
        minkowski(){
            translate([Sx+Filet/2,Sy+Filet/2,-1])
            cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
            cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
        }
}

module DE9SquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
    if(OnOff==1)
        minkowski(){
            translate([Sx+Filet/2,Sy+Filet/2,-1])
            cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
            cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
        }
}

module RJ12SquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
    if(OnOff==1)
        minkowski(){
            translate([Sx+Filet/2,Sy+Filet/2,-1])
            cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
            cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
        }
}
module DCSquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
    if(OnOff==1)
        minkowski(){
            translate([Sx+Filet/2,Sy+Filet/2,-1])
            cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
            cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
        }
}

module SquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
    if(OnOff==1)
        minkowski(){
            translate([Sx+Filet/2,Sy+Filet/2,-1])
            cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
            cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
        }
}
//                      <- Linear text panel -> 
module LText(OnOff,Tx,Ty,Font,Size,Content,_valign="baseline",_halign="left"){

    if(OnOff==1)
        translate([Tx,Ty,Thick+.5])
    linear_extrude(height =0.7){
        text(Content, size=Size, font=Font, halign = _halign,valign=_valign);
    }
}
//                     <- Circular text panel->  
module CText(OnOff,Tx,Ty,Font,Size,TxtRadius,Angl,Turn,Content,_valign="baseline",_halign="center"){ 
    if(OnOff==1) {
        Angle = -Angl / len(Content);
        translate([Tx,Ty,Thick+.5])
        for (i= [0:len(Content)-1] ){   
            rotate([0,0,i*Angle+90+Turn])
            translate([0,TxtRadius,0]) {
                linear_extrude(height = 0.7){
                    text(Content[i], font = Font, size = Size,   halign = _halign,valign=_valign);
                }
            }   
        }
    }
}

include <dsub.scad>
    ////////////////////// <- New module Panel -> //////////////////////
module FPanL(){

    centerDB9X = -25;
    centerDB9Y = -80;
    centerDB9Z = -5;

    difference(){
        color(Couleur2)
        Panel(Length,Width,Thick,Filet);

        rotate([90,0,90]){

            color(Couleur2){


                //                     <- Cutting shapes from here ->  
                //(On/Off, Xpos,Ypos,Length,Width,Filet)3*Thick+2,Thick+5
                echo((Width - PCBWidth)/2-3*Thick+1);
                echo(Thick+1.2);
                translate([((Width - PCBWidth)/2), 0, 0] - [3*Thick+2, 0, 0]){
                    if (DE9On ==1) {
                        rotate([0,180,90]) translate([centerDB9X,centerDB9Y,centerDB9Z])
                        dsub (1.2,17.04,10);
                    }
                    USBbSquareHole(USBbOn, 54.61+1.2, FootHeight+PCBThick, 9, 5, 1, Ccenter=false);//USBb
                    USBcSquareHole(USBcOn, 51+1.45, FootHeight-0.5+PCBThick-1, 11, 6, 1, Ccenter=false);//USBc
                    I2CSquareHole(I2COn, 81.28-1.2, FootHeight+PCBThick, 14, 9, 1, Ccenter=false);//I2C
                    //DE9SquareHole(DE9On, 74-9.5, FootHeight+PCBThick, 30, 13.2, 1, Ccenter=false);//DE9

                    RJ12SquareHole(RJ12On, 98.425-1.3, FootHeight+PCBThick, 14, 13, 1, Ccenter=false);//SPI
                    DCSquareHole(DCOn, 119.38+0.8, FootHeight+PCBThick, 10, 12, 1, Ccenter=false);//DC barrel


                }}}

        //                            <- To here -> 

        // placing text for the front panel 
        //translate ([-.5,0,0])
        //rotate([90,0,90])        
        //translate([-((Width + PCBWidth)/2) ,0,0]+[-2.3,0,0])
        color(Couleur1){
            //translate ([-.5,0,0])
            translate ([-0.9, 4,-3])
            // module LText(OnOff,Tx,Ty,Font,Size,Content,_valign="baseline",_halign="left")
            rotate([-90,0,-90])    
            // module LText(OnOff,Tx,Ty,Font,Size,Content,_valign="baseline",_halign="left")
            {
                Fontsize = 3; 
                TextBaseLine = - FootHeight * .92 ; 
                //                      <- Adding text from here ->   

                LText(USBbOn,-55,TextBaseLine,"Arial Black",Fontsize,"USB",_halign = "center",_valign="top");
                LText(USBcOn,-53,TextBaseLine,"Arial Black",Fontsize,"USB",_halign = "center",_valign="top");
                LText(I2COn, -81.28,TextBaseLine,"Arial Black",Fontsize,"I2C",_halign = "center",_valign="top");
                LText(DE9On, -74.44,TextBaseLine,"Arial Black",Fontsize,"COM",_halign = "center",_valign="top");
                LText(RJ12On,-98.425,TextBaseLine,"Arial Black",Fontsize,"Remote",_halign = "center",_valign="top");
                LText(DCOn,-119.9 ,TextBaseLine,"Arial Black",Fontsize,"DC",_halign = "center",_valign="top");    

                //                            <- To here ->
            }
        }
    }
}


/////////////////////////// <- Main part -> /////////////////////////

module VESAmount75(stud_height_mm,h_offset_mm) {
    // A VESA mount for D 75mm
    // from Wikipedia https://en.wikipedia.org/wiki/Flat_Display_Mounting_Interface
    // D 75mm	MIS-D, 75, C	75	75
    
    // First, I will create the 4 studs in the correct
    // (square) pattern
    stud_distance_mm = 75;
    sd = stud_distance_mm;
    half_stud_distance_mm = sd/2;
    hsd = half_stud_distance_mm;
    
    sh = stud_height_mm;
    translate([0,0,sh-h_offset_mm])
        union() {
            rotate([180,0,0])
            translate([hsd,hsd,0])
                m4_stud(0,0,sh,0);
            rotate([180,0,0])
            translate([hsd,-hsd,0])
                m4_stud(0,0,sh,0);
            rotate([180,0,0])
            translate([-hsd,hsd,0])
                m4_stud(0,0,sh,0);
            rotate([180,0,0])
            translate([-hsd,-hsd,0])
                m4_stud(0,0,sh,0);
        }
}
module VESApunch75(stud_height_mm,h_offset_mm) {
//    sh = stud_height_mm +2; // addin length for cutting
    sh = stud_height_mm + -2; // addin length for cutting
    radius = 4.5; // just a guess!
    
    stud_distance_mm = 75;
    sd = stud_distance_mm;
    half_stud_distance_mm = sd/2;
    hsd = half_stud_distance_mm;
    translate([0,0,sh-h_offset_mm])
    union() {
        rotate([180,0,0])
        translate([hsd,hsd,0])
        cylinder(h=sh,r=radius,center = false);
        rotate([180,0,0])
        translate([hsd,-hsd,0])
        cylinder(h=sh,r=radius,center = false);
        rotate([180,0,0])
        translate([-hsd,hsd,0])
        cylinder(h=sh,r=radius,center = false);
        rotate([180,0,0])
        translate([-hsd,-hsd,0])
        cylinder(h=sh,r=radius,center = false);
        }
}

module TShellWithVESA() {
    stud_height_mm = 7.8;

//    h_offset_mm = 1;
    h_offset_mm = 7.8;  //Set flush with Top.
        // Coque haut - Top Shell
    difference() {
        translate([0,Width,Height+0.2]){
            color( Couleur1,1){
                rotate([0,180,180]){
                    Coque();
                }
            }
        }
//        translate([Length/2,Width/2,Height+0.2]) 
        translate([Length/2,Width/2,35]) 
        VESApunch75(stud_height_mm+15,h_offset_mm);
    }

    // Note: The Coque is not centered on the origin,
    // so we have to do some math for that....
    translate([Length/2,Width/2,Height+0.2]) 
    color("blue"){
        VESAmount75(stud_height_mm,h_offset_mm);
    }
    
// #VESApunch75(stud_height_mm);
}

if(GPAD_TShellWithVESA==1){
    TShellWithVESA();
}

if(GPAD_TShell==1){
    // Coque haut - Top Shell
    translate([0,Width,Height+0.2]){
        color( Couleur1,1){
            rotate([0,180,180]){
                Coque();
            }
        }
    }
}

module centeredHeatSetInsert() {
    translate([0,-60.05,0])
   import( "flanged insert M4 D7.1 H9.11.stl",convexity=1);
}


if(HEAT_SET_INSERTS==1){

    stud_distance_mm = 75;
    sd = stud_distance_mm;
    half_stud_distance_mm = sd/2;
    hsd = half_stud_distance_mm;
    insert_height = 6;
    #color("gray")
    translate([Length/2,Width/2,Height+0.2]) 
//    translate([0,0,insert_height/2+1])
    translate([0,0, 0+1])
    union() {
        rotate([180,0,0])
        translate([hsd,hsd,0])
        centeredHeatSetInsert();
        rotate([180,0,0])
        translate([hsd,-hsd,0])
        centeredHeatSetInsert();
        rotate([180,0,0])
        translate([-hsd,hsd,0])
        centeredHeatSetInsert();
        rotate([180,0,0])
        translate([-hsd,-hsd,0])
        centeredHeatSetInsert();
        }

}


module SPKBOSS62(stud_height_mm,h_offset_mm) {
    // A VESA mount for D 75mm
    // from Wikipedia https://en.wikipedia.org/wiki/Flat_Display_Mounting_Interface
    // D 75mm	MIS-D, 75, C	75	75
    
    // First, I will create the 4 studs in the correct
    // (square) pattern
    stud_distance_mm = 62;
    sd = stud_distance_mm;
    half_stud_distance_mm = sd/2;
    hsd = half_stud_distance_mm;
    
    sh = stud_height_mm;
    translate([0,0,sh-h_offset_mm])
        union() {
            rotate([180,0,0])
            translate([hsd,hsd,0])
                m4_stud(0,0,sh,0);
            rotate([180,0,0])
            translate([hsd,-hsd,0])
                m4_stud(0,0,sh,0);
            rotate([180,0,0])
            translate([-hsd,hsd,0])
                m4_stud(0,0,sh,0);
            rotate([180,0,0])
            translate([-hsd,-hsd,0])
                m4_stud(0,0,sh,0);
        }
}
module SPKBOSSpunch62(stud_height_mm,h_offset_mm) {
//    sh = stud_height_mm +2; // addin length for cutting
    sh = stud_height_mm + -2; // addin length for cutting
    radius = 4.5; // just a guess!
    
    stud_distance_mm = 60;
    sd = stud_distance_mm;
    half_stud_distance_mm = sd/2;
    hsd = half_stud_distance_mm;
    translate([0,0,sh-h_offset_mm])
    union() {
        rotate([180,0,0])
        translate([hsd,hsd,0])
        cylinder(h=sh,r=radius,center = false);
        rotate([180,0,0])
        translate([hsd,-hsd,0])
        cylinder(h=sh,r=radius,center = false);
        rotate([180,0,0])
        translate([-hsd,hsd,0])
        cylinder(h=sh,r=radius,center = false);
        rotate([180,0,0])
        translate([-hsd,-hsd,0])
        cylinder(h=sh,r=radius,center = false);
        }
}
 module GPAD_BShell () { 
   // Coque bas - Bottom shell
    i=0;
    difference(){
    difference (){
    union () {
    
        union(){
   // speaker_clamp(); 
    difference(){
        color(Couleur1,1){
            union(){ 
                Coque();
               translate( [3*Thick+2,Thick+5,5])SpeakerHolder(0,SpeakerHoleX,PCBWidth-FootPosX,11,Ccenter=true); //Speaker holder
                // Pied support PCB - PCB feet

                if (PCBFeet==1){// Feet
                    // Pieds PCB - PCB feet (x4) 
                    translate([PCBPosX,PCBPosY,0]){ 
                        Feet();
                    }   

                }
            }}


        color( Couleur1,1){
            translate( [3*Thick+2,Thick+5,0]){         //([-.5,0,0]){
                //(On/Off, Xpos, Ypos, Diameter)


                for ( i = [-12.25:-8.25] ){
                    CylinderHole(1,PCBLength-(LEDXposOffset+LEDspacing*i),LEDYposOffset,5); //LED1 , switch signs to move downwards ??Question
                }
              //CylinderHole(1,PCBLength-46.99,PCBWidth-FootPosX,5); //LED6 power //??Question extra?

                //(On/Off, Xpos,Ypos,Length,Width,Filet)
                SquareHole(1,DisplayXpos,DisplayYpos,DisplayLenght,DisplayWidth,DisplayFilet,Ccenter=true);   //Display
                CylinderHole(1,SpeakerHoleX,68.58,2); //reset hole
                //(On/Off, Xpos, Ypos, "Font", Size, Diameter, Arc(Deg), Starting Angle(Deg),"Text",_halign = "center",_valign="top") 

                // RotaryEncoder
                CylinderHole(1,RotaryEncoderXpos,RotaryEncoderYpos,RotaryEncoderDiameter); //RotaryEncoder
                CylinderSpacer(1,RotaryEncoderXpos,RotaryEncoderYpos,RotaryEncoderDiameter+Thick+m/2); //cutout for RotaryEncoder

            }}
        color( Couleur1,1){
            translate( [3*Thick+2,Thick+5,0]){         //([-.5,0,0]){
              //  (On/Off, Xpos, Ypos, Diameter)
             
              SpeakerHole(1, Krake_rev2_76mmSPK ? SpeakerPositionX : SpeakerHoleX, Krake_rev2_76mmSPK ?SpeakerPositionY:SpeakerHoleY ,SpeakerDiameter_mm/2.5,Ccenter=true); //Buzzer // Speaker 
            
            
            
            
     //LED hole generator  unsure why this works   ??Question repeated commented to avoid confusion    
                //for ( i = [0 : 0] ){                    //CylinderHole(1,PCBLength-(27.94+12.7*i),15.24,5); //LED1-5  
              //  }
                CylinderHole(1,PCBLength-46.99+translationVariable,PCBWidth-FootPosX,5); //LED6 power light ??Question delete extra

                //(On/Off, Xpos,Ypos,Length,Width,Filet)
                SquareHole(1,DisplayXpos,DisplayYpos,DisplayLenght,DisplayWidth,DisplayFilet,Ccenter=true);   //Display
                CylinderHole(1,SpeakerHoleX,68.58,2); //reset hole

                //(On/Off, Xpos, Ypos, "Font", Size, Diameter, Arc(Deg), Starting Angle(Deg),"Text",_halign = "center",_valign="top") 
                rotate([0,180,0])translate( [0,0,-(Thick+.99)])CText(1,-(muteButtonXpos),muteButtonYpos,"Arial Black",4,9,110,270,"MUTE")color(Couleur3);

                CylinderHole(1,muteButtonXpos,muteButtonYpos,muteButtonDiameter); //Mute Button
                CylinderSpacer(1,muteButtonXpos,muteButtonYpos,muteButtonDiameter+Thick+m/2); //cutout for mute button


                //   // SquareHole(1,PCBLength-63.87,33.12,1,1,0,Ccenter=true);       //testing 
            }
        }
SpeakerCutOut();
    }
    translate ([-5,0,0])
    SpeakerHexGrill(Krake_rev2_76mmSPK,SpeakerPositionX, SpeakerPositionY);
        }
    
SpeakerEnclosure();
}
    CableHole();
    }
 AllScrewPockets ();
}
    }
 

/// SPKBOSS62 and SPKBOSSpunch62 integration w Speaker 62mm hole to hole
if(GPAD_BShell==1){
difference (){
GPAD_BShell();
translate ([SpeakerPositionX+5,SpeakerPositionY+8,Thick+7.8+.1])
SPKBOSSpunch62 (7.8,7.8);

}
translate ([SpeakerPositionX+5,SpeakerPositionY+8,Thick+7.8+.1])
SPKBOSS62 (7.8,7.8);
}

if(RotaryEncoder ==1){
    //RotaryEncoder
    translate( [3*Thick+2,Thick+5,0])     
    ButtonSwitch(1,RotaryEncoderXpos,RotaryEncoderYpos,RotaryEncoderDiameter); //RotaryEncoder  
}
if(BButton ==1){
    //button
    translate( [3*Thick+2,Thick+5,0])     
    ButtonSwitch(1,muteButtonXpos,muteButtonYpos,muteButtonDiameter); //Mute Button
}
if(LED_Standoff == 1){
    //(OnOff,Cx,Cy,Cdia,Cpitch,Cheight,Ccenter=false){ 
    translate( [3*Thick+2,Thick+5,5]){//([-.5,0,0]){
        LedSpacer(1,PCBLength-27.94,15.24,5,2.54,Thick+FootHeight+PCBThick/2+.1-6,false); //LED1      
        LedSpacer(1,PCBLength-40.64,15.24,5,2.54,Thick+FootHeight+PCBThick/2+.1-6,false); //LED2
        LedSpacer(1,PCBLength-53.34,15.24,5,2.54,Thick+FootHeight+PCBThick/2+.1-6,false); //LED3
        LedSpacer(1,PCBLength-66.04,15.24,5,2.54,Thick+FootHeight+PCBThick/2+.1-6,false); //LED4
        LedSpacer(1,PCBLength-78.74,15.24,5,2.54,Thick+FootHeight+PCBThick/2+.1-6,false); //LED5
        LedSpacer(1,PCBLength-46.99,PCBWidth-FootPosX,5,2.54,Thick+FootHeight+PCBThick/2+.1-6,false); //LED6 power
    }
}
if(PCB_SIMPLE==1){
    //////////////////// - PCB only visible in the preview mode - /////////////////////    
    translate([3*Thick+2,Thick+5,Thick+FootHeight+PCBThick/2+.1]){

        rotate([0,0,90])translate([0,0,PCBThick-0.2]);//import("General Alarm Device Enclosure-GeneralPurposeAlarmDevicePCB 1.stl", convexity=3);
            %cube ([PCBLength,PCBWidth,PCBThick]);
        translate([PCBLength/2,PCBWidth/2,0]){ 
            color("Olive")
            rotate([0,0,90])
                %text("SIMPLE PCB", halign="center", valign="center", font="Arial black") ;

        }
    } // Fin PCB 
}
if(PWA_GPAD==1){
    //////////////////// - PCB only visible in the preview mode - /////////////////////    
    translate([3*Thick+2,Thick+5,Thick+FootHeight+PCBThick/2+.1]){

        rotate([0,0,90])translate([0,0,PCBThick-0.2])import("General Alarm Device Enclosure-GeneralPurposeAlarmDevicePCB 1.stl", convexity=3);
            %cube ([PCBLength,PCBWidth,PCBThick]);
        translate([PCBLength/2,PCBWidth/2,0]){ 
            color("Olive")
                %text("PCB", halign="center", valign="center", font="Arial black");
        }
    } // Fin PCB 
}
if(PWA_KRAKE==1){
    //////////////////// - PCB only visible in the preview mode - /////////////////////    
    translate([3*Thick+2  + translationVariable,Thick+5,Thick+FootHeight+PCBThick/2-.1]){
        rotate([0,0,90])translate([0,0,PCBThick-0.2]);
        rotate([0,0,90])translate([-55.88,17.78,0])
        color(Couleur3)
        import("KRAKE_PWArev1.stl", convexity=3);
        //%cube ([PCBLength,PCBWidth,PCBThick]);
        //translate([PCBLength/2,PCBWidth/2,0]){ 
        //color("Olive")
        //%text("PCB", halign="center", valign="center", font="Arial black");
    }}
//}
/*
  translate([3*Thick+2,Thick+5,Thick+FootHeight+PCBThick/2+.1]){

  rotate([0,0,90])translate([0,0,PCBThick-0.2])import("General Alarm Device Enclosure-GeneralPurposeAlarmDevicePCB 1.stl", convexity=3);
  %cube ([PCBLength,PCBWidth,PCBThick]);
  translate([PCBLength/2,PCBWidth/2,0]){ 
  color("Olive")
  %text("PCB", halign="center", valign="center", font="Arial black");
  }
  } // Fin PCB 
  }
*/

// Panneau avant - Front panel  <<<<<< Text and holes only on this one.
//rotate([0,-90,-90]) 
if(GPAD_FPanL==1){
    translate([Length-(Thick*2+m/2),Thick+m/2,Thick+m/2])
    FPanL();
}
//Panneau arrière - Back panel
if(GPAD_BPanL==1){
    color(Couleur2)
    translate([Thick+m/2,Thick+m/2,Thick+m/2])
    Panel(Length,Width,Thick,Filet);
}

// Module Section
// RoundBox(length = 100, width = 50, height = 30, radius = 10, resolution = 50);
module frontPanel(){
    difference() {
        Coque();
        SpeakerHole(OnOff = 1, Cx = 30, Cy = 20, Cdia = 20, Ccenter = true);
    }
}

//This is the purchased part.

// This is a 2mm screw that connects the BShell to the TShell.
if (T_BShellScrew==1){
    //translate([3*Thick+11,-1,Height/2+4]){
    // rotate([90,0,0])
    // import("MCMaster_Carr_Torx_Roundhead_Screw_99397A324.stl");
    //}


    union(){ //sides holes
        $fn=50;
        translate([3*Thick+11,0,Height/2+4]){
            rotate([90,0,0]){
                import("MCMaster_Carr_Torx_Roundhead_Screw_99397A324.stl");
            }
        }
        translate([Length-((3*Thick)+11),0,Height/2+4]){
            rotate([90,0,0]){
                import("MCMaster_Carr_Torx_Roundhead_Screw_99397A324.stl");
            }
        }
        translate([3*Thick+11,Width-5,Height/2-4]){
            rotate([90,0,0]){
                import("MCMaster_Carr_Torx_Roundhead_Screw_99397A324.stl");
            }
        }
        translate([Length-((3*Thick)+11),Width-5,Height/2-4]){
            rotate([90,0,0]){
                import("MCMaster_Carr_Torx_Roundhead_Screw_99397A324.stl");
            }
        }
    }
}//fin de sides holes


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////// speaker expansion Courtney ////////////
//consider port location and access holes      
    
module SpeakerEnclosure (){

module SpeakerBoxKnife(){
    if (Krake_rev2_76mmSPK == 1)
translate ([SpeakerPositionX+4+Thick*5, SpeakerPositionY,Height/4-Thick]){
    cube ([1.6*SpeakerDiameter_mm-10,SpeakerDiameter_mm*1.5-10, Height*2], center=true);
    }}
    
 module SpeakerBoxOuter (){
     if (Krake_rev2_76mmSPK == 1)
difference (){ 
    translate ([SpeakerPositionX+Thick*5+5, SpeakerPositionY,Height/4+Thick+10]){
    cube ([1.6*SpeakerDiameter_mm,SpeakerDiameter_mm*1.5+10, SpeakerHeight_mm+5], center=true);
        }
        SpeakerBoxKnife();
    }
  }
  
  module SpeakerShelf (){
      if (Krake_rev2_76mmSPK == 1)
  translate ([SpeakerPositionX +SpeakerHeight_mm + 10, SpeakerPositionY,SpeakerHeight_mm/2-Thick]){
  cube ([5,SpeakerDiameter_mm*1.5, 3*SpeakerHeight_mm/4], center=true);
  }
  }
  union (){
  SpeakerShelf();
  SpeakerBoxOuter();
  }
  
     }
 
module SpeakerCutOut () {
    if (Krake_rev2_76mmSPK == 1)
translate ([SpeakerPositionX+Thick*5+45, SpeakerPositionY-52.5,-10]){
cube ([15,SpeakerDiameter_mm*1.25+5,Height]);
  }
  }
  
 ///////////////screw parameters///////////////////
   ScrewDiameter=2.2;
   ScrewLength=5+Thick;
   Spacing=100;
 module ScrewPocket(){
     if (Krake_rev2_76mmSPK == 1)
cylinder (d=ScrewDiameter,h=ScrewLength+Thick,center=true);
}

module AllScrewPockets (){
    if (Krake_rev2_76mmSPK == 1)
translate([SpeakerPositionX + Thick*5 + 5,
               SpeakerPositionY,
               Height/4 + Thick + 10 - (Height-10)/2]) {

        part_x = 1.6*SpeakerDiameter_mm;
        part_y = SpeakerDiameter_mm*1.5 + 10;
        part_z = Height - 10;

        edge_offset = 4;   // distance from edges to hole center (mm)

        x = part_x/2 - edge_offset;
        y = part_y/2 - edge_offset;

        // 4 corner holes
        translate([ x,  y, Height-ScrewLength-Thick]) 
            ScrewPocket();
        translate([-x,  y, Height-ScrewLength-Thick]) 
            ScrewPocket();
        translate([ x, -y, Height-ScrewLength-Thick]) 
            ScrewPocket();
        translate([-x, -y, Height-ScrewLength-Thick]) 
            ScrewPocket();
    }
}
 
module SpeakerBoxLid(){
    if (Krake_rev2_76mmSPK == 1)
difference (){
    translate ([SpeakerPositionX+Thick*5+5, SpeakerPositionY,SpeakerHeight_mm-ScrewLength]){
        cube ([1.6*SpeakerDiameter_mm,       SpeakerDiameter_mm*1.5+10, Thick], center=true);
        }
    AllScrewPockets();
    }
}

if (SPKLid==1){
SpeakerBoxLid();
}

module CableHole() {
    if (Krake_rev2_76mmSPK == 1)
    translate ([1.8*SpeakerDiameter_mm,SpeakerDiameter_mm,3*SpeakerHeight_mm/4]){
        rotate ([0,90,0]){
            cylinder (d=10,h=Thick*5,center=true);
            }
        }
    }

