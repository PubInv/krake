use <Util_modules.scad>
include <General_paramters.scad>
use <PartNumbering.scad>

PartName = "BPanl";
PartVersion = "V4.1";
Date = "26213";

B_panl_part();

module B_panl_part()
{
    difference(){
    Panel(Length,Width,Thick,Filet);
    
    if(NumberingSystem == 1){
    translate([1.25,50,20])rotate([90,0,90])
    numbering(0.9,4,PartName,PartVersion,Date);}
}
    // logo
 color("red")translate([-0.4,115+25,35-45])mirror([0,1,0])rotate([90,0,90])linear_extrude(height = 1, center = true, convexity = 10)
   scale(0.032)import (file = "KRAKE_3.dxf", layer = "0");
    color("red")translate([-0.8,130,39])mirror([0,1,0])rotate([90,0,90])linear_extrude(height = 1, center = false, convexity = 10)scale(0.1)import("PI-LogoTransparent.svg");
}
 
 
   
