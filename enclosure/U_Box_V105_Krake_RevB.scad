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

use <imported_scadModels/COVER.scad>
use <imported_scadModels/recessed.scad>
use <imported_scadModels/slot.scad>
use <imported_scadModels/flex_cover.scad>
use <Util_modules.scad>
use <B_shell.scad>
use <F_panl.scad>

include <General_paramters.scad>


////////////////////////////////////////////////////////////////////
// Export Options
////////////////////////////////////////////////////////////////////
GPAD_TShell          = 0;
GPAD_BShell          = 0; //2 w/LCD 
GPAD_FPanL           = 1;//3 bottom
GPAD_BPanL           = 0; //4 top
sd_door_on_off       = 0;
recessed_bottom_on_off = 0;// turn on/off the recess moduel only
recessed_module_on_off =0;
//// recess sub modules when recess module is on///


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
   translate([Length/2-29,Width/2,Height+0.2]) 
    VESApunch75(stud_height_mm+15,h_offset_mm);
}

// Note: The Coque is not centered on the origin,
// so we have to do some math for that....
translate([Length/2 - 29 ,Width/2,Height+0.2]) 
color("blue"){
VESAmount75(stud_height_mm,h_offset_mm);
}

}

if(GPAD_TShell==1){

difference(){
if(GPAD_TShellWithVESA == 1){
    TShellWithVESA();
    }
else
{
    // Coque haut - Top Shell
    translate([0,Width,Height+0.2]){
        color( Couleur1,1){
            rotate([0,180,180]){
                Coque();
            }
        }
    }
    
}
if (GPAD_TshellDoorRecess == 1){
translate([Length/2 +Door_recess_x_offset,Width/2-Door_recess_y_offset,Height+0.25])rotate([180,0,90])translate([-15,-10,-3])cube([90,35+19,10]);
    
}}
if (GPAD_TshellDoorRecess == 1){

translate([Length/2 +Door_recess_x_offset,Width/2-Door_recess_y_offset,Height+0.25]){
    
rotate([180,0,90]){difference(){cover_unit(); 
translate([55,20,0])translate([0,0,-10])cylinder(h = 22, r = 7.9);


}
translate([55,20,0]){rotate([0,0,90])mirror([0,0,0])test_locking();

}
}

}

}



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
Panel(Length,Width,Thick,Filet);
}
// Module Section
//RoundBox(length = 100, width = 50, height = 30, radius = 10, resolution =50);

module frontPanel(){
difference() {
Coque();
SpeakerHole(OnOff = 1, Cx = 30, Cy = 20, Cdia = 20, Ccenter = true);
}
}
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



module RoundBox2(Length, Width, Height,f=1){// Cube bords arrondis
$fn=Resolution;            

translate([f,f,0]) minkowski()
{
cube([Length-(2*f),Width-(2*f),Height-1]);
cylinder(r=f,h=1,false);
}

}// End of RoundBox Module
module recessed_module(){
translate([0,0,-R_height+9]){
    if (sd_door_on_off == 1){
        translate(v = [46.3+29,44+47.5,24.8]) rotate([0,0,180])flexiable_cover(n = 26, l = 29, h = 1, g = 1.6, t = 0.3);
        }
    if(recessed_bottom_on_off == 1)translate([0,5,0])cover_slot();

    }
if(recessed_wall_on_off == 1){
    difference(){
        reccessed_bottom_f(R_height = R_height);
        if(GPAD_TshellDoorRecess){
         cut_cube_height = 6;
        cut_cube_x = 24;
        cut_cube_y = 30;
        translate([cut_cube_x,cut_cube_y,Height-30-cut_cube_height])cube([10,10,cut_cube_height]);
        translate([cut_cube_x+50,cut_cube_y,Height-30-cut_cube_height])cube([10,10,cut_cube_height]);
        }}


}
if(recessed_bottom_on_off == 1){
translate([0,0,-R_height+9])AC_buttons_pins2(s_t = 0.5, s_w =0.5, B_height =button_pins_height);

translate([0,0,-R_height+9])difference(){
    reccesed_f();
    translate([AC_button_x,AC_button_y,26.8])cylinder(r=6,h=10);
    translate([AC_button_x,AC_button_y+29.67,26.8])cylinder(r=6,h=10);
    translate([AC_button_x+8,AC_button_y+7,26.5])
    {
        cube([23+5,40,3]);
    }  
        translate([45.2,48+10+27.5,10])cube([26.2+5,4,50]);
    }       
}
}
if(recessed_module_on_off == 1){
translate([Length/2 +Door_recess_x_offset,Width/2-Door_recess_y_offset,Height+0.25])translate([-34,-32,-40.2]) recessed_module();
}
module mp3_player(){
translate([44,42.5,19+4.8])cube([20.7,20.7,8.6]);
}

module cover_slot() {
gap = -15.5;
slot_w = 1.3;
slot_h = 3;
slot_t = 0.5;
slot_l = 55;

difference(){union(){
translate([45.3-gap,90,25.5]){

mirror(v = [1,0,0]) translate([gap,0,0]) rotate([90,0,0]) slot(w = slot_w, h = slot_h, t = slot_t, l = slot_l);
translate([gap,0,0]) rotate([90,0,0]) slot(w = slot_w, h = slot_h, t = slot_t, l = slot_l);
}
}
translate([45.2,48+27.5+5,0])cube([-gap*2+0.2,4,50]);

} 
}
if(SPK==1){
color(c=[0,0,2.8])
translate([PCBLength*.91,PCBWidth*.82,3])

import("imported_scadModels/Speaker2W-SpeakerOutline.stl");
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
