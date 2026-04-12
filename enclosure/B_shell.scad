
use <Util_modules.scad>
include <imported_scadModels/StudModules.scad>

include <General_paramters.scad>


speaker_ring = 1;


////////////////////////////////////////////////////////////////////
//Parameters for LEDHole
LEDspacing = 12.7 ;
LEDYposOffset = 15.24  ; // offset from the Encoder edge of PCB
LEDXposOffset = 27.94  ; // offset from the connector edge of PCB
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//Modifications for Display
DisplayXpos = PCBLength-50.8  + translationVariable;
DisplayYpos = 71.12;
DisplayLenght = 26;
DisplayWidth = 76;
DisplayFilet = 0;
////////////////////////////////////////////////////////////////////

//Modifications for SpeakerHole
SpeakerHoleY = Krake ? PCBWidth*.769 : 15.24;
SpeakerHoleX = translationVariable + ( Krake ? PCBLength*.815 :PCBLength - 15.24 );

// Krake Modifications for RotaryEncodeHole
i=-1; // Encoder is one LEd spacing below LEDs
RotaryEncoderXpos      = Krake ? PCBLength-(LEDXposOffset+LEDspacing*i) : translationVariable + PCBLength-(LEDXposOffset+LEDspacing*i);
RotaryEncoderYpos      = 15.24 ;
RotaryEncoderDiameter  = 16 ;


GPAD_BShell_part();

//interface module with the main Assambly file 
/// SPKBOSS62 and SPKBOSSpunch62 integration w Speaker 62mm hole to hole
module GPAD_BShell_part(){
if((Krake_rev2_76mmSPK==1)){
difference (){ 
GPAD_BShell();
translate ([SpeakerPositionX+2.5,SpeakerPositionY+4,Thick+7.8+.1])
SPKBOSSpunch62 (7.8,7.8);
}
translate ([SpeakerPositionX+2.5,SpeakerPositionY+4,Thick+7.8+.1])
SPKBOSS62 (7.8,7.8);
if(speaker_ring == 1){
//import("parts_toPrint/SpeakerGasket3Inch-Extrude_7.7mm.stl");

SpeakerFlushRing(3.5,7.8-1.75,35, posx = SpeakerPositionX+2.5,posy =SpeakerPositionY+4,posz = Thick);
}

}else
{
    GPAD_BShell();
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
for ( i = [-6.5:-2.5] ){
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
SpeakerHole(1, Krake_rev2_76mmSPK ? SpeakerPositionX+2.5-(3*Thick+2) : SpeakerHoleX, Krake_rev2_76mmSPK ?SpeakerPositionY+9-(Thick+5):SpeakerHoleY ,SpeakerDiameter_mm/2.5-1.5,Ccenter=true); //Buzzer // Speaker 
//LED hole generator  unsure why this works   ??Question repeated commented to avoid confusion    
CylinderHole(1,PCBLength-46.99+translationVariable,PCBWidth-FootPosX,5); //LED6 power light ??Question delete extra
//(On/Off, Xpos,Ypos,Length,Width,Filet)
SquareHole(1,DisplayXpos,DisplayYpos,DisplayLenght,DisplayWidth,DisplayFilet,Ccenter=true);   //Display
CylinderHole(1,SpeakerHoleX,68.58,2); //reset hole
rotate([0,180,0])translate( [0,0,-(Thick+.99)])CText(1,-(muteButtonXpos),muteButtonYpos,"Arial Black",4,9,110,270,"MUTE")color(Couleur3);
CylinderHole(1,muteButtonXpos,muteButtonYpos,muteButtonDiameter); //Mute Button
CylinderSpacer(1,muteButtonXpos,muteButtonYpos,muteButtonDiameter+Thick+m/2); //cutout for mute button
}}
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


}}




module SquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
if(OnOff==1)
minkowski(){
translate([Sx+Filet/2,Sy+Filet/2,-1])
cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
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


module Feet(){     
////////////////////////////// - 4 Feet - //////////////////////////////////////////   
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
}
} // Fin du module Feet



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


//This is for all the circular holes.
//                          <- Circle hole -> 
// Cx=Cylinder X position | Cy=Cylinder Y position | Cdia= Cylinder dia | Cheight=Cyl height
module CylinderHole(OnOff,Cx,Cy,Cdia,Ccenter=false){
if(OnOff==1)
translate([Cx,Cy,-1])
cylinder(d=Cdia,10, $fn=50,center=Ccenter);
}
//

//                          <- Circle Spacer -> 
// Cx=Cylinder X position | Cy=Cylinder Y position | Cdia= Cylinder dia | Cheight=Cyl height
module CylinderSpacer(OnOff,Cx,Cy,Cdia,Ccenter=false){
if(OnOff==1)
translate([Cx,Cy,Thick])
cylinder(d=Cdia,10, $fn=50,center=Ccenter);
}



//////////////////////////////////////////////////////////////////////////
/////////// speaker expansion Courtney ////////////
//consider port location and access holes      
module SpeakerEnclosure (){

module SpeakerBoxKnife(){
if (Krake_rev2_76mmSPK == 1)
translate ([SpeakerPositionX+Thick*5-7, SpeakerPositionY,Height/4+Thick+10]){
cube ([1.05*SpeakerDiameter_mm,SpeakerDiameter_mm*1.5+15, Height+10], center=true);
}}

module SpeakerBoxOuter (){
if (Krake_rev2_76mmSPK == 1)

translate ([SpeakerPositionX+Thick*5-7, SpeakerPositionY,Height/4+Thick+9]){
cube ([1.095*SpeakerDiameter_mm,SpeakerDiameter_mm*1.5+20, SpeakerHeight_mm+15.5], center=true);
}
}
difference (){ 
SpeakerBoxOuter(); 
SpeakerBoxKnife();
}}


module SpeakerHole(Krake_rev2_76mmSPK,Cx,Cy,Cdia,Ccenter=false){
//difference(){
if(Krake_rev2_76mmSPK==1)
  translate([Cx ,Cy - 5,-1]){
for(j = [0  : 2]){

rotate(a = 360*j/3,v = [0,0,1])
for(i = [1.8 : Cdia/60 : Cdia/1.5]){
    rotate(a = 1*(i/Cdia)*360,v = [0,0,1])
    translate([i,i,0])

    cylinder(d=i*.65,h = 10, $fn=100,center=Ccenter);
    //echo(i);
}}}}


module SpeakerHexGrill(Krake_rev2_76mmSPK, Cx, Cy, Cdia=SpeakerDiameter_mm*1.25, h=Thick, cell=6, wall=1){
if (Krake_rev2_76mmSPK == 1)
translate([Cx+9, Cy+5, 0])
intersection(){
cylinder(d=Cdia*0.95, h=h, $fn=100);
for (x=[-Cdia/2:cell:Cdia/2])
for (y=[-Cdia/2:cell:Cdia/2])
translate([x + (y%2)*cell/2, y*0.866, 0])
    difference(){
        cylinder(r=cell/2, h=h, $fn=6);
        cylinder(r=(cell/2)-wall, h=h+0.2, $fn=6);
    }}}
    
    
module CableHole() {
if (Krake_rev2_76mmSPK == 1)
translate ([1.2*SpeakerDiameter_mm,SpeakerDiameter_mm,3*SpeakerHeight_mm/4]){
rotate ([0,90,0]){
cylinder (d=10,h=Thick*10,center=true);
}}}

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
}}


module SPKBOSS62(Krake_rev2_76mmSPK, stud_height_mm,h_offset_mm) {
if(Krake_rev2_76mmSPK==1);
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

module SPKBOSSpunch62(Krake_rev2_76mmSPK,stud_height_mm,h_offset_mm) {
if(Krake_rev2_76mmSPK==1);
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



//translate ([SpeakerPositionX+2.5,SpeakerPositionY+4,Thick+7.8+.1])
module SpeakerFlushRing(width ,height,innerRadius,posx =0, posy = 0, posz =0)
{
    $fn = 50;
    translate([posx,posy,posz]){
        difference()
    {
        cylinder(r = innerRadius+width, h =height);
        translate([0,0,-1])
        cylinder(r = innerRadius, h =height+2);
        }}
}
