// <EnclosureREVc.scad>
//  Copyright (C) 204  Oyindamola Adeniran
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

x=54.9;//x +5PCB Width
y=26.9;//y +4PCB Height
z=0;//z
x1= 57.88;
y1= 30.685;
height_top = 3;
corner_radius=4.5;
corner_radiusb=4.5;
wall_thick =5;
hole_diameter = 3.5; // for screws
post_diameter = 10;
LED_diameter = 6.6; // for LED
lid_thickness = 2;
lid_lip = 2;
lid_tolerance =0.5;

extend = 30; //Show inside Enclosure [20 is closing enclosure number.]

power_surge_diameter =17;
power_surge_location =extend+5;

heightPCB = 113.0620;
widthPCB = 53.370;

heightPCBHole = 103.163;
widthPCBHole = 43.4340;

halfheightPCBHole = heightPCBHole/2;
halfwidthPCBHole = widthPCBHole/2;


//THE TOP 

module topPlate() {
    color("Yellow")
        hull(){
            translate ([x,y,z]){
                cylinder (r=corner_radius, h=height_top);
            }
            translate ([-x,y,z]){
                cylinder (r=corner_radius, h=height_top);
            }
            translate ([-x,-y,z]){
                cylinder (r=corner_radius, h=height_top);
            }
            translate ([x,-y,z]){
                cylinder (r=corner_radius, h=height_top);
            }   
        }
}

module topPlateHoles() {
    
//ESP32 HOLE
translate([-15.195,-0.416,0])
cube([32,54,7],center=true);

//Top LED 1
translate([-36.83,21.685,0])
cylinder(h=15,d1=LED_diameter,d2 = LED_diameter, center=true);
//Top LED 2
translate([-36.83,14.3352,0])
cylinder(h=15,d1=LED_diameter,d2 = LED_diameter, center=true);
//Top LED 3
translate([-36.83,6.9854,0])
cylinder(h=15,d1=LED_diameter,d2 = LED_diameter, center=true);
//Top LED 4
translate([-36.83,-0.3644,0])
cylinder(h=15,d1=LED_diameter,d2 = LED_diameter, center=true);
//Top LED 5
translate([-36.83,-7.7142,0])
cylinder(h=15,d1=LED_diameter,d2 = LED_diameter, center=true);
//Top LED 6
translate([26.6618,20.0228,0])
cylinder(h=15,d1=LED_diameter,d2 = LED_diameter, center=true);
//Top LED 7
translate([26.6618,12.6568,0])
cylinder(h=15,d1=LED_diameter,d2 = LED_diameter, center=true);
//Top LED 8
translate([51.8498,11.654,0])
cylinder(h=15,d1=LED_diameter,d2 = LED_diameter, center=true);

//PUSH_BUTN 1
translate([9.0333,21.685,0])
cube([9.1967,7.20,15], center=true);
//PUSH_BUTN 2
translate([9.0333,13.435,0])
cube([9.1967,7.20,15], center=true);
//PUSH_BUTN 3
translate([9.0333,5.185,0])
cube([9.1967,7.20,15], center=true);
//PUSH_BUTN 4
translate([9.0333,-3.065,0])
cube([9.1967,7.20,15],center=true);
//TRANSISTOR HOLE
translate([10.76,-13.41,00])
cube([10.5,4.641,15],center=true);
}

module top() {
    difference(){
        topPlate();
        topPlateHoles();
    }
}


module caseShape() {
    color("red")
    hull(){
        translate ([x1,y1,extend]){
            cylinder (r=corner_radius, h=25);
        }
        translate ([-x1,y1,extend]){
            cylinder (r=corner_radius, h=25);
        }
        translate ([-x1,-y1,extend]){
            cylinder (r=corner_radius, h=25);
        }
        translate ([x1,-y1,extend]){
            cylinder (r=corner_radius, h=25);
        }
    }
}

module A() {
     difference(){
        caseShape();
//INSIDE HOLLOW
        color("blue")
        hull(){
        translate ([55.88,27.658,(extend+11.5)]){
            cylinder (r=corner_radiusb, h=26.5, center=true);
        }
            translate ([-55.88,27.658,(extend+11.5)]){
            cylinder (r=corner_radiusb, h=26.5, center=true);
        }
            translate ([-55.88,-27.658,(extend+11.5)]){
            cylinder (r=corner_radiusb, h=26.5, center=true);
        }
            translate ([55.88,-27.658,(extend+11.5)]){
            cylinder (r=corner_radiusb, h=26.5, center=true);
            }
        }
        
        //Small Hole for Enclosure
        translate([-halfheightPCBHole,halfwidthPCBHole,60])
        cylinder(h=102,d1=hole_diameter,d2 = hole_diameter, center=true);  
        
        //Small Hole 2 for Enclosure
        translate([halfheightPCBHole,halfwidthPCBHole,60])
        cylinder(h=102,d1=hole_diameter,d2 = hole_diameter, center=true);
        
        //Small Hole 3 for Enclosure
        translate([halfheightPCBHole,-halfwidthPCBHole,60])
        cylinder(h=102,d1=hole_diameter,d2 = hole_diameter, center=true);
        //Hole 4 for Enclosure
        translate([-halfheightPCBHole,-halfwidthPCBHole,60])
        cylinder(h=102,d1=hole_diameter,d2 = hole_diameter, center=true);
        
        //POWER SURGE
        translate([x1,-6.049,(extend+15)])
        rotate([0,90,0])
        cylinder (h=12,d1=power_surge_diameter,d2=power_surge_diameter,center=true);
        //The barrel jack is sticking out at 2.8mm
        
        //ESP32 USB A-HOLE
        translate([-18,-35,extend+10])
        rotate([0,0,90])
        cube([10,20,15],center=true);
    }
}

module B() {
    $fn=50;
    
    //THE 4 CYCLINDERS 
    difference(){
    //Screw Cylinder 1
        color("DarkGrey")
    translate([-halfheightPCBHole,halfwidthPCBHole,13])
    cylinder(h=19.5,d1=13,d2 =13, center=true);
     //Small Hole 1
    translate([-halfheightPCBHole,halfwidthPCBHole,12])
    cylinder(h=25,d1=hole_diameter,d2 = hole_diameter, center=true);   
    }   
    difference (){    
    //Screw Cylinder 2
        color("DarkGrey")
    translate([halfheightPCBHole,halfwidthPCBHole,13])
    cylinder(h=19.5,d1=13,d2 =13,center=true);
    //Small Hole 2
    translate([halfheightPCBHole,halfwidthPCBHole,12])
    cylinder(h=25,d1=hole_diameter,d2 = hole_diameter, center=true);
    }
    difference(){
    //Screw Cylinder 3
        color("DarkGrey")
    translate([halfheightPCBHole,-halfwidthPCBHole,13])
    cylinder(h=19.5,d1=13,d2 =13, center=true);
        //Small Hole 3
    translate([halfheightPCBHole,-halfwidthPCBHole,12])
    cylinder(h=25,d1=hole_diameter,d2 = hole_diameter, center=true);
    }
     difference(){   
    //Screw Cylinder 4
         color("DarkGrey")
    translate([-halfheightPCBHole,-halfwidthPCBHole,13])
    cylinder(h=19.5,d1=13,d2 =13,center=true);
    //Hole 4
    translate([-halfheightPCBHole,-halfwidthPCBHole,12])
    cylinder(h=25,d1=hole_diameter,d2 = hole_diameter, center=true);
     }
    // ISSUES
     //The distance between the edge of board and LED is 0.4mm but might not be an issue.
     //Elevate the PCB board 2mm
}

module case() {  
    A();
    B();
}

top();
case();
