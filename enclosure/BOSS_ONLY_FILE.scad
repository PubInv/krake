
Wallthickness = 2;
Screwdiameter = 3; // Mc-Master Carr Screw 99461A941
Holediameter = 0.8*Screwdiameter;
Bossdiameter = 2*Holediameter;






translate ([0,0,20])
import("ImageToStl.com_99461a941_phillips+rounded+head+thread-forming+screws+(1).stl");

difference() {
    cylinder(h=10, d=Bossdiameter); // Main body
   %cylinder(h=12, d=Holediameter, center = false); // Center hole
    translate([0, 0, 12]); 
    //cylinder(h=10, d=1.5, center = true); // Side boss
}
$fn = 100;
translate([0,0,1])
cube([25.4, 25.4,2], center = true);
