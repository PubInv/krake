ACB_x = 38.77-5;
ACB_y = 42.44;
ACB_z = 40;
AC_button_x = 38.77;
AC_button_y = 44.44;

use <COVER.scad>


translate([33,32,40])rotate([180,0,90])cover_unit();
/*
difference(){translate([ACB_x,ACB_y,ACB_z-10])rotate([0,0,90])linear_extrude(10)round_hull(20,34);


    translate([ACB_x,ACB_y,ACB_z-5])rotate([0,0,90])translate([0,0 ,0.5])linear_extrude(5)round_hull(16,34);
    translate([30,15,ACB_z])cube([50,50,50],true);
}*/

AC_buttons();
AC_buttons_pins();



module round_hull(r,w)
{
    
$fn=50;
hull()
{
    circle(r);
    translate([w,0,0,])circle(r);
    }    
    
    }

module AC_buttons_pins()
{
    translate([AC_button_x,AC_button_y,-0.5+ACB_z-10])rotate([0,180,0])AC_button_Pin();
        translate([AC_button_x,AC_button_y+29.67,-0.5+ACB_z-10])rotate([0,180,0])AC_button_Pin();
    }


module AC_button_Pin()
{
    
    union(){difference(){difference(){
     cylinder(1,2,2,true); 
    
    rotate_extrude(convexity = 10)translate([2, 0, 0])
        circle(r = 0.5);
        
      
}
translate([0,0,2])cube(4,true);}
cylinder(5.175,1.5,1.5,false);
}

    
}


module AC_buttons()
{
    
    

translate([AC_button_x+5,AC_button_y-4,25])rotate([0,0,90])ACB();
translate([AC_button_x+5,AC_button_y-4+29.67,25])rotate([0,0,90])ACB();
    
    
    }


module ACB(){
linear_extrude(20)union(){
mirror([1,0,0])translate([-8,0,0])ACB_s(4);
ACB_s(4);
}
module ACB_s(width = 5)
{
    
    length = 24;
    
    union(){union()

{
square([width,1],false);

square([1,length]);
translate([0,length,0])rotate([0,0,-90])difference(){difference(){
    
    circle(r = 1);
    translate([0.5,0.5,0])square([1,2],true);

    }translate([0,-1,0])square(2,true);
}

 }
translate([1,length-1,0])square([0,1]);
}
    
}

}





module ACB_tips(){
translate([AC_button_x,AC_button_y,35.5])ACB_tip();
translate([AC_button_x,AC_button_y+29.67,35.5]) ACB_tip();
    }
module ACB_tip(width = 5)
{
    difference(){
    sphere(d= width);
    translate([0,0,-width/2])cube(width,true);
    }}
    
