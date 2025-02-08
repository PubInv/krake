
Wallthickness = 2;
Screwdiameter = 3; // Mc-Master Carr Screw 99461A941
Holediameter = 0.8*Screwdiameter;
Bossdiameter = 2*Holediameter;
FootHeight   = 15; //From GPAD enclosure
ScrewLenght = 8;
counterBoreDepth = 0.3*Screwdiameter;

//To-Do 
//Hole isn't all the way through

translate ([0,0,FootHeight+ScrewLenght+1])
import("MCMaster_Carr_Pan_Head_Screw_99461a941.stl");

difference() {
    cylinder(h=FootHeight, d=Bossdiameter); // Main body
    translate ([0, 0,Wallthickness*0.7])
    cylinder(h=FootHeight+1, d=Holediameter, center = false);
    translate([0, 0,FootHeight-counterBoreDepth]) 
    cylinder(h=counterBoreDepth+1, d=Screwdiameter, center = false); // Side boss
}
$fn = 100;
translate([0,0,1])
cube([25.4, 25.4,2], center = true);
