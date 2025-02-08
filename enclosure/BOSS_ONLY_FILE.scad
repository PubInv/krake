difference() {
    cylinder(h=10, d=2.5); // Main body
    cylinder(h=15, d=0.7, center = true); // Center hole
    translate([0, 0, 12]) cylinder(h=10, d=1.5, center = true); // Side boss
}
$fn = 100;
translate([0,0,1])
cube([25.4, 25.4,2], center = true);
