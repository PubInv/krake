use <Util_modules.scad>
include <General_paramters.scad>
use <PartNumbering.scad>

PartName = "BPanl";
PartVersion = "V4.0";
Date = "26141";

B_panl_part();

module B_panl_part()
{
    Panel(Length,Width,Thick,Filet);
    
    if(NumberingSystem == 1){
    translate([1.8,100,20])rotate([90,180,90])
    numbering(0.9,4,PartName,PartVersion,Date);}
 
 color("red")translate([-0.4,115,35])mirror([0,1,0])rotate([90,0,90])linear_extrude(height = 1, center = true, convexity = 10)
   scale(0.012)import (file = "KRAKE_3.dxf", layer = "0");
    
}
 
 
   
