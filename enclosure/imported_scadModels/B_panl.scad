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
    
    }