
//$fs = 1;

//$fa = 0.1;
module button_cap(){
$fn = 200;
union(){
translate([0,0,-20-5])cylinder(20,4.5,4.5);
translate([0,0,-5])difference(){

cylinder(5+2,1.45,1.45);

translate([0,0,-1])cylinder(12,0.7,0.7);

}

mirror([0,0,0])translate([-1.5,1.8,-5])cube([3,2,2.5+5]);
mirror([0,1,0])translate([-1.5,1.8,-5])cube([3,2,2.5+5]);
mirror([1,1,0])translate([-1.5,1.8,-5])cube([3,2,2.5+5]);
mirror([1,1,0])mirror([0,1,0])translate([-1.5,1.8,-5])cube([3,2,2.5+5]);

}

}

button_cap();