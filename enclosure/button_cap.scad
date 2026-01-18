
//$fs = 1;

//$fa = 0.1;

use <spring.scad>

module button_cap2(h){
$fn = 200;
union(){
translate([0,0,-h-5])cylinder(h,4.5,4.5);
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

module button_cap(h){
$fn = 200;
union(){
    
difference(h){

cylinder(h,1.45,1.45);

translate([0,0,-2])cylinder(h,0.7,0.7);

}
}

}
module button_cap_S(h,n,d,t=0.5,w = 0.5){
button_cap(h);
for(i = [0:n]){
rotate([0,0,d*i])translate([2.5,0,h-1]){translate([0,0,1-t])spring(2,w,t,3);

translate([-2.5,-0.5,1-t])cube([2,1,t]);
}
}}


button_cap_S(40,5,60);