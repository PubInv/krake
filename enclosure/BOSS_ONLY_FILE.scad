//Name of File: BOSS_ONLY_FILE.scad
//For Testing the fit of screws on printed 3D parts.
//Author: Oyindamola Adeniran
//Date: 20250208
//Liscence: Public Domain

Wallthickness = 2;
Screwdiameter = 3; // Mc-Master Carr Screw 99461A941
Holediameter = 0.8*Screwdiameter;
Bossdiameter = 2*Holediameter;
FootHeight   = 15; //From GPAD enclosure
ScrewLenght = 8;
counterBoreDepth = 0.3*Screwdiameter;
wallLenght = 25.4;
wallWidth = 25.4;
inner_allowancePLA = 0.5;//Error on Lee's 3D printer(undersize error).
outer_allowancePLA = 0.3; 
Screw = 1;
Boss = 1;
TheWall =1;

module BOSS_ONLY (){
// Import 3mm Plastite Screw
if(Screw ==1){
translate ([0,0,FootHeight+ScrewLenght+1])
import("MCMaster_Carr_Pan_Head_Screw_99461a941.stl");
}

if(Boss ==1){
// The Boss
difference() {
    cylinder(h=FootHeight, d=Bossdiameter+outer_allowancePLA); // Main body
    translate ([0, 0,(Wallthickness*0.7)])
    cylinder(h=FootHeight+1, d=Holediameter+inner_allowancePLA, center = false);
    translate([0, 0,FootHeight-counterBoreDepth]) 
    cylinder(h=counterBoreDepth+1, d=Screwdiameter+inner_allowancePLA, center = false); // Side boss
}
if(TheWall ==1){
// The Wall
translate([0,0,Wallthickness/2])
cube([wallLenght,wallWidth,Wallthickness], center = true);
}
}
$fn = 100;
}

BOSS_ONLY();
