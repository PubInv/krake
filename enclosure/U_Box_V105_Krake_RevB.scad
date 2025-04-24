////////////////////////////////////////////////////////////////////
//  Heartman/Hearty OpenSCAD Parametric Box - MODIFIED BY NAGHAM //
//  Krake & GPAD Configurable Enclosure Generator                //
//  License: CC BY-NC 3.0                                        //
////////////////////////////////////////////////////////////////////

/* Project Selector */
Krake = 1;      // [0:Off, 1:On]
GPAD  = 0;      // [0:Off, 1:On]

////////////////////////////////////////////////////////////////////
// Common Parameters - Base settings shared by all configurations
////////////////////////////////////////////////////////////////////

Length         = 83.82 + 13;       
Width          = 138 + 13;                     
Height         = 40;  
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

PCBPosX        = 0;
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
Dec_Thick      = Vent ? Thick * 2 : Thick;
Dec_size       = Vent ? Thick * 2 : 0.8;

////////////////////////////////////////////////////////////////////
// Export Options
////////////////////////////////////////////////////////////////////

GPAD_TShell          = 0;
GPAD_BShell          = 0;
GPAD_FPanL           = 1;
GPAD_BPanL           = 0;
BButton              = 0 ;
RotaryEncoder        = 0;  // change to a real rotary encoder 
T_BShellScrew        = 0;
BOSSScrew            = 0;
PCB_SIMPLE           = 0;
PWA_GPAD             = 0;
PWA_KRAKE            = 1;
LED_Standoff         = 0;
LED_Standoff_Single  = 0;
PWA                  = 0;

//Modifications for Display
DisplayXpos = PCBLength-50.8;
DisplayYpos = 71.12;
DisplayLenght = 26;
DisplayWidth = 76;
DisplayFilet = 0;

//Modificationd for muteButton
muteButtonXpos = PCBLength-10;
muteButtonYpos = 31.75;
muteButtonDiameter = 15;

//Modifications for SpeakerHole
SpeakerHoleY = Krake ? 103.24 : 15.24;
SpeakerHoleX = PCBLength - 15.24;

// Krake Modifications for RotaryEncodeHole
RotaryEncoderXpos      = Krake ? PCBLength - 15 : 0;
RotaryEncoderYpos      = Krake ? 15.24 : 0;
RotaryEncoderDiameter  = Krake ? 15.8 : 0;

// Connectors Modifications specific port logic
USBbOn    = Krake ? 0 : 1;
USBcOn    = Krake ? 1 : 0;
DE9On     = Krake ? 1 : 0;
I2COn     = Krake ? 0 : 1;
RJ12On    = Krake ? 1 : 1;
DCOn      = Krake ? 1 : 1;
//PWA_GPAD  = Krake ? 0 : 1;
//PWA_KRAKE = Krake ? 1 : 0;
DE9SquareHole      = Krake ? 1 : 0;


if (Krake + GPAD > 1)
echo("WARNING: More than one project mode active!!!");

//For instructions on mute Button inscription go to line 594.

// Import 3mm Plastite Screw
if(BOSSScrew ==1){
translate ([PCBLength-70.74,12.24,25])
//translate ([0,0,FootHeight+ScrewLenght+1])
import("MCMaster_Carr_Pan_Head_Screw_99461a941.stl");
}

if(PWA ==1){ 
translate([-10,-50,10])
rotate ([0,0,90])
import("KRAKE_PWArev1.stl");
}

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


////////////////////////////////// - Module Coque/Shell - //////////////////////////////////         

module Coque(){//Coque - Shell  
Thick = Thick*2;  
difference(){    
difference(){//sides decoration
union(){    
difference() {//soustraction de la forme centrale - Substraction Fileted box

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
                  RoundBox($a=Length-((2*Thick)+(2*m)), $b=Width-Thick, $c=Height-(Thick*2));
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
module SpeakerHole(OnOff,Cx,Cy,Cdia,Ccenter=false){
//difference(){
if(OnOff==1)
translate([Cx,Cy,-1]){
for(j = [1  : 3]){

rotate(a = 360*j/3,v = [0,0,1])
for(i = [0.8 : Cdia/60 : Cdia/2]){
rotate(a = 1*(i/Cdia)*360,v = [0,0,1])
translate([i,i,0])

cylinder(d=i*.65,h = 10, $fn=100,center=Ccenter);
//echo(i);


}
}
}
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
module SpeakerHolder(OnOff,Cx,Cy,Cdia,Ccenter=false){ 
if(OnOff==1){
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
linear_extrude(height = 0.5){
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
linear_extrude(height = 0.5){
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
 
rotate([0,180,90]) translate([centerDB9X,centerDB9Y,centerDB9Z])
    dsub (1.2,17.04,10);
    
USBbSquareHole(USBbOn, 54.61+1.2, FootHeight+PCBThick, 9, 5, 1, Ccenter=false);//USBb
USBcSquareHole(USBcOn, 51+1.45, FootHeight+PCBThick-1, 11, 6, 1, Ccenter=false);//USBc
I2CSquareHole(I2COn, 81.28-1.2, FootHeight+PCBThick, 14, 9, 1, Ccenter=false);//I2C
//DE9SquareHole(DE9On, 74-9.5, FootHeight+PCBThick, 30, 13.2, 1, Ccenter=false);//DE9

RJ12SquareHole(RJ12On, 98.425-1.3, FootHeight+PCBThick, 14, 13, 1, Ccenter=false);//SPI
DCSquareHole(DCOn, 119.38+0.8, FootHeight+PCBThick, 10, 12, 1, Ccenter=false);//DC barrel
  

}}}}
//                            <- To here -> 


color(Couleur1){
translate ([-.5,0,0])
rotate([90,0,90])        
translate([((Width - PCBWidth)/2) ,0,0]+[-2.3,0,0]){
//                      <- Adding text from here ->   
//(On/Off, Xpos, Ypos, "Font", Size, "Text",_halign = "center",_valign="top")  
LText(USBbOn,54.61,FootHeight*.9,"Arial Black",3,"USB",_halign = "center",_valign="top");
LText(USBcOn,54.61,FootHeight*.9,"Arial Black",3,"USB",_halign = "center",_valign="top");
LText(I2COn, 81.28   ,FootHeight*.9,"Arial Black",3,"I2C",_halign = "center",_valign="top");
LText(DE9On, 76.28   ,FootHeight*.9,"Arial Black",3,"COM",_halign = "center",_valign="top");
LText(RJ12On,98.425,FootHeight*.9,"Arial Black",3,"Remote",_halign = "center",_valign="top");
LText(DCOn,119.38,FootHeight*.9,"Arial Black",3,"DC",_halign = "center",_valign="top");    
//SquareHole(1,0,FootHeight*.9,1,1,0,Ccenter=true); //origin
//                            <- To here ->
}
}
}


/////////////////////////// <- Main part -> /////////////////////////

if(GPAD_TShell==1){
// Coque haut - Top Shell
color( Couleur1,1){
translate([0,Width,Height+0.2]){
rotate([0,180,180]){
Coque();
}
}
}


}

if(GPAD_BShell==1){
// Coque bas - Bottom shell
i=0;
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

for ( i = [0 : 4] ){

CylinderHole(1,PCBLength-(27.94+12.7*i),15.24,5); //LED1  
}
CylinderHole(1,PCBLength-46.99,PCBWidth-FootPosX,5); //LED6 power

//(On/Off, Xpos,Ypos,Length,Width,Filet)
SquareHole(1,DisplayXpos,DisplayYpos,DisplayLenght,DisplayWidth,DisplayFilet,Ccenter=true);   //Display
CylinderHole(1,SpeakerHoleX,68.58,2); //reset hole
//(On/Off, Xpos, Ypos, "Font", Size, Diameter, Arc(Deg), Starting Angle(Deg),"Text",_halign = "center",_valign="top") 


rotate([0,180,0])translate( [0,0,-(Thick+.99)])CText(1,-( RotaryEncoderXpos),RotaryEncoderYpos,"Arial Black",4,9,110,270,"RE");
CylinderHole(1,RotaryEncoderXpos,RotaryEncoderYpos,RotaryEncoderDiameter); //RotaryEncoder
CylinderSpacer(1,RotaryEncoderXpos,RotaryEncoderYpos,RotaryEncoderDiameter+Thick+m/2); //cutout for mute button
}}
color( Couleur1,1){
translate( [3*Thick+2,Thick+5,0]){         //([-.5,0,0]){
//(On/Off, Xpos, Ypos, Diameter)
SpeakerHole(1,SpeakerHoleX,SpeakerHoleY,11,Ccenter=true); //Buzzer
for ( i = [0 : 4] ){

CylinderHole(1,PCBLength-(27.94+12.7*i),15.24,5); //LED1  
}
CylinderHole(1,PCBLength-46.99,PCBWidth-FootPosX,5); //LED6 power

//(On/Off, Xpos,Ypos,Length,Width,Filet)
SquareHole(1,DisplayXpos,DisplayYpos,DisplayLenght,DisplayWidth,DisplayFilet,Ccenter=true);   //Display
CylinderHole(1,SpeakerHoleX,68.58,2); //reset hole
//(On/Off, Xpos, Ypos, "Font", Size, Diameter, Arc(Deg), Starting Angle(Deg),"Text",_halign = "center",_valign="top") 
rotate([0,180,0])translate( [0,0,-(Thick+.99)])CText(1,-(muteButtonXpos),muteButtonYpos,"Arial Black",4,9,110,270,"MUTE");
CylinderHole(1,muteButtonXpos,muteButtonYpos,muteButtonDiameter); //Mute Button
CylinderSpacer(1,muteButtonXpos,muteButtonYpos,muteButtonDiameter+Thick+m/2); //cutout for mute button


//   // SquareHole(1,PCBLength-63.87,33.12,1,1,0,Ccenter=true);       //testing 
}
}

}
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
    translate([3*Thick+2,Thick+5,Thick+FootHeight+PCBThick/2+.1]){

rotate([0,0,90])translate([0,0,PCBThick-0.2]);
    rotate([0,0,90])translate([-55.88,17.78,0])
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

