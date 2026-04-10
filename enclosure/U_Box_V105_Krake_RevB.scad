//File: U_Box_V105_Krake_RevB.scad
//Modified by Public Invention to make enclosures for:
// GPAD
// KRAKE (TM)

////////////////////////////////////////////////////////////////////
//  Heartman/Hearty OpenSCAD Parametric Box 
// MODIFIED BY NAGHAM KHEIR, COURTNEY LUDICK //
//  Krake & GPAD Configurable Enclosure Generator                //
//  License: CC BY-NC 3.0                                        //
////////////////////////////////////////////////////////////////////

// Thanks to RonRN18 for this Creative Commons Attribution set of Heat-set insert:
// https://www.thingiverse.com/thing:5849866
include <imported_scadModels/StudModules.scad>


use <Util_modules.scad>
use <B_shell.scad>
use <T_shell.scad>
use <F_panl.scad>
use <B_panl.scad>

include <General_paramters.scad>


////////////////////////////////////////////////////////////////////
// Export Options
////////////////////////////////////////////////////////////////////
GPAD_TShell          = 0;
GPAD_BShell          = 0; //2 w/LCD 
GPAD_FPanL           = 0;//3 bottom
GPAD_BPanL           = 0; //4 top


////////////////////////////////////////////////////////////////////

// Import 3mm Plastite Screw
if(BOSSScrew ==1){
translate ([PCBLength-70.74,12.24,25]) //is this just to reposition the screw to the correct place?, where are these numbers from?
//translate ([0,0,FootHeight+ScrewLenght+1])
import("parts_toPrint/MCMaster_Carr_Pan_Head_Screw_99461a941.stl");
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


/////////////////////////// <- Main part -> /////////////////////////



if(GPAD_TShell==1){
T_shell_part();
}


module centeredHeatSetInsert() {
translate([0,-60.05,0])
import( "imported_scadModels/flanged insert M4 D7.1 H9.11.stl",convexity=1);
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



if((GPAD_BShell==1)){
GPAD_BShell_part();
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
}}
if(PCB_SIMPLE==1){
////////////- PCB only visible in the preview mode - /////////////////////    
translate([3*Thick+2,Thick+5,Thick+FootHeight+PCBThick/2+.1]){
rotate([0,0,90])translate([0,0,PCBThick-0.2]);//import("General Alarm Device Enclosure-GeneralPurposeAlarmDevicePCB 1.stl", convexity=3);
%cube ([PCBLength,PCBWidth,PCBThick]);
translate([PCBLength/2,PCBWidth/2,0]){ 
color("Olive")
rotate([0,0,90])
%text("SIMPLE PCB", halign="center", valign="center", font="Arial black") ;

}} // Fin PCB 
}

if(PWA_GPAD==1){
/////////////- PCB only visible in the preview mode - /////////////////////    
translate([3*Thick+2,Thick+5,Thick+FootHeight+PCBThick/2+.1]){
rotate([0,0,90])translate([0,0,PCBThick-0.2])import("General Alarm Device Enclosure-GeneralPurposeAlarmDevicePCB 1.stl", convexity=3);
%cube ([PCBLength,PCBWidth,PCBThick]);
translate([PCBLength/2,PCBWidth/2,0]){ 
color("Olive")
%text("PCB", halign="center", valign="center", font="Arial black");
}} // Fin PCB 
}
if(PWA_KRAKE==1){
/////////- PCB only visible in the preview mode -/////////////////    
translate([3*Thick+2  + translationVariable,Thick+5,Thick+FootHeight+PCBThick/2-.1]){
rotate([0,0,90])translate([0,0,PCBThick-0.2]);
rotate([0,0,90])translate([-55.88,17.78,0])
color(Couleur3)
import("imported_scadModels/KRAKE_PWArev2.stl", convexity=3);
}}

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
B_panl_part();
}

// Module Section
//RoundBox(length = 100, width = 50, height = 30, radius = 10, resolution =50);




//This is the purchased part.
// This is a 2mm screw that connects the BShell to the TShell.
if (T_BShellScrew==1){

union(){ //sides holes
$fn=50;
translate([3*Thick+11,0,Height/2+4]){
rotate([90,0,0]){
import("parts_toPrint/MCMaster_Carr_Torx_Roundhead_Screw_99397A324.stl");
}
}
translate([Length-((3*Thick)+11),0,Height/2+4]){
rotate([90,0,0]){
import("parts_toPrint/MCMaster_Carr_Torx_Roundhead_Screw_99397A324.stl");
}
}
translate([3*Thick+11,Width-5,Height/2-4]){
rotate([90,0,0]){
import("parts_toPrint/MCMaster_Carr_Torx_Roundhead_Screw_99397A324.stl");
}
}
translate([Length-((3*Thick)+11),Width-5,Height/2-4]){
rotate([90,0,0]){
import("parts_toPrint/MCMaster_Carr_Torx_Roundhead_Screw_99397A324.stl");
}}}}//fin de sides holes


///////////////screw parameters///////////////////
ScrewDiameter=2.2;
ScrewLength=5+Thick;
Spacing=100;
module ScrewPocket(){
if (Krake_rev2_76mmSPK == 1)
cylinder (d=ScrewDiameter,h=ScrewLength+Thick,center=true);
}

///////////////////
module SpeakerBoxLid(){
if (Krake_rev2_76mmSPK == 1)
difference (){
translate ([SpeakerPositionX+Thick*5+5, SpeakerPositionY,SpeakerHeight_mm-ScrewLength]){
cube ([1.6*SpeakerDiameter_mm,       SpeakerDiameter_mm*1.5+10, Thick], center=true);
}
AllScrewPockets();
}}


if (SPKLid==1){
SpeakerBoxLid();
}
///////////////////////




module mp3_player(){
translate([44,42.5,19+4.8])cube([20.7,20.7,8.6]);

}


if(SPK==1){
color(c=[0,0,2.8])
translate([PCBLength*.91,PCBWidth*.82,3])

import("imported_scadModels/Speaker2W-SpeakerOutline.stl");
}




/////////////////////////////////////
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
cylinder(h = clamp_width-.001, r = (speaker_diameter/2) + flexure_gap + flexure_thickness);

// Inner clearance for speaker
translate([PCBLength*.91,PCBWidth*.82,clamp_Z])
translate([0,0,overlap])
cylinder(h = clamp_width +0.2 - overlap, r = (speaker_diameter/2) + flexure_gap);

// Create flexure cuts
translate([PCBLength*.91,PCBWidth*.82,clamp_Z])
for (angle = [0,90,180,270]) {
rotate([0,0,angle])
translate([speaker_diameter/2, -flexure_thickness/2, 0])
cube([flexure_gap*2, flexure_thickness +7, clamp_width +4]);
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
