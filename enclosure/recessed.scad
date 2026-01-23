 ACB_x = 38.77-5;
ACB_y = 42.44;
ACB_z = 40;
AC_button_x = 38.77;
AC_button_y = 44.44;

use <COVER.scad>

use <button_cap.scad>
use <flex_cover.scad>
use <slot.scad>



//translate([AC_button_x,AC_button_y,25])button_cap_S(5.175,5,60);

//translate([33,32,40])rotate([180,0,90])cover_unit();
cover_slot();
module cover_slot() {
 
translate(v = [46.3,44,24.8]) flexiable_cover(n = 23, l = 18, h = 1, g = 1.6, t = 0.3);
gap = -10;
slot_w = 1.3;
slot_h = 3;
slot_t = 0.5;
slot_l = 35;
difference(){union(){
translate([55.3,75,25.5]){

mirror(v = [1,0,0]) translate([gap,0,0]) rotate([90,0,0]) slot(w = slot_w, h = slot_h, t = slot_t, l = slot_l);
translate([gap,0,0]) rotate([90,0,0]) slot(w = slot_w, h = slot_h, t = slot_t, l = slot_l);

}
}
translate([45.2,48,0])cube([20.2,4,50]);
  
} 
}
w_p = 2;
h_p = 50;
l_p = 0.5;


r_w = 40;
r_l = 70;
r_h = 9;
r_t = 2;

module reccessed(w,l,h,t)
{

difference(){    
    translate([w/2,l/2,h/2])
    difference(){
    cube([w,l,h], true);    
    translate([0,0,0])cube([w-t,l-t,h+t], true);  
        
        
        
    }
    //translate([-1,15,-11+0.4])cube([w+2,40,16],false);
}

}






under_r_x = 30.5-2;
under_r_y = 28;
under_r_z = 38-r_h-r_t-0.1;






snap_l = 6.5;
snap_h = 2;
snap_w = snap_h/2;

module hh(){
  difference(){
    reccesed_f();
      translate([AC_button_x,AC_button_y,25])cylinder(r=6,h=10);
    translate([AC_button_x,AC_button_y+29.67,25])cylinder(r=6,h=10);  
      
          translate([AC_button_x+8,AC_button_y,26.5])
      {
          cube([17,35,3]);
          
          } 
          translate([45.2,48,0])cube([20.2,4,50]);
      }
      AC_buttons_pins2(s_t = 0.5, s_w =0.5);
      
//translate([AC_button_x+24.98,AC_button_y+34.8,28.9])rotate([0,0,180])rotate([0,180,270])cover2(17,44,2);
//reccessed_bottom_f();

}
difference(){
    hh();
//cube([50,50,50]);
}
//reccesed_f();

difference()
{
union(){
//reccessed_bottom_f();
//reccesed_f();
}    
 //cube([500,60,80]);   
    }

module reccesed_f(){
    
translate([under_r_x+3,under_r_y,under_r_z])cube([r_w-2,r_l,r_t], false);
    translate([1,0,-0.8])add_supp_snap();
translate([100+1,0,-0.8])mirror([1,0,0])translate([1,0,0])add_supp_snap();
}
 
 module reccessed_bottom_f(){
    union(){
    translate([30.5,28,38-r_h])reccessed(r_w,r_l,r_h,r_t);
        
rotate([0,180,0])translate([-70.5,43,-38]){translate([-0.1,0.1,0])snap(snap_h,snap_w,40-0.2,snap_l+7,1);
    translate([0.1,0.1,0])mirror([1,0,0])translate([-r_w,0,0]) snap(snap_h,snap_w,40-0.2,snap_l+7,1);
    }
    //translate([0,0,15-r_h])add_supp_snap();
    //translate([100+1,0,15-r_h])mirror([1,0,0])translate([0,0,0])add_supp_snap();
    }
    
}
    
    module add_supp_snap(){
    difference(){
    translate([30.5,38,27])supp_snap();
translate([under_r_x-0.001,under_r_y+15,under_r_z-r_h+3])translate([2,25,5.5])scale([1.15,1.05,1.15])translate([-2,-25,-5.5])snap(snap_h,snap_w,40,snap_l,2);
          translate([28.8,38,24])cube([2,50,2]);
        }
    }
        
      
module  supp_snap(){
difference(){
hull(){
hull(){
rotate([0,45,0])cube([w_p,h_p,w_p]);
translate([0,0,-l_p])rotate([0,45,0])cube([w_p,h_p,w_p]);}
translate([-2,0,0])hull(){
rotate([0,45,0])cube([w_p,h_p,w_p]);
translate([0,0,-l_p])rotate([0,45,0])cube([w_p,h_p,w_p]);}
}
translate([-3,-1,-20])cube([w_p+1,h_p+2,50],false);

}
}

 module snap(w = 100,h, t,l,thick)
       {
           
           translate([0,0,l])rotate([90,270,180])mirror([1,0,0])union(){
           translate([0,thick,0])linear_extrude(t)polygon(points=[[0,0],[w,0],[w/2,h]]);       
    cube([l,thick,t],false);
   }
   }
   
   



//test();
//translate([10,10,10])cube([30,50,50]);


     /*
difference(){translate([ACB_x,ACB_y,ACB_z-10])rotate([0,0,90])linear_extrude(10)round_hull(20,34);


    translate([ACB_x,ACB_y,ACB_z-5])rotate([0,0,90])translate([0,0 ,0.5])linear_extrude(5)round_hull(16,34);
    translate([30,15,ACB_z])cube([50,50,50],true);
}*/

//AC_buttons();
//AC_buttons_pins();



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

module AC_buttons_pins2(s_t = 2,s_w = 0.5, scale_s = 1)
{
    translate([AC_button_x,AC_button_y,-0.5+ACB_z-15.17])translate([0,0,5])translate([0,0,-5])button_cap_S(4.57,5,60,t=s_t,w =s_w);
    
    
        translate([AC_button_x,AC_button_y+29.67,-0.5+ACB_z-15.17])translate([0,0,5])translate([0,0,-5])button_cap_S(4.57,5,60,t = s_t, w =s_w);
    }

module AC_buttons2()
{
    
    

translate([AC_button_x+5,AC_button_y-4,25])rotate([0,0,90])ACB();
translate([AC_button_x+5,AC_button_y-4+29.67,25])rotate([0,0,90])ACB();
    
    
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
    
