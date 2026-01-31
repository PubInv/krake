
$fs = 0.1;

$fn =30;

use <latch.scad>


module test_locking(){
    difference(){
translate([6,-83,-25])import("Testpiece.stl",convexity=10);

difference(){
translate([0,0,-15])cylinder(h = 30, r = 50);
translate([0,0,-16])cylinder(h = 35, r = 8);
}
//translate([-25,-59,-50])cube([50,50,100]);
    }
//cube([29,17.6,50],true); // this cube for cutout

}



cover_length = 80;
cover_width = 35;
cover_thickness = 2.1;
Resolution = 50;



module lock(w,h,h2,l,tip){

points = [[0,0],[0,h],[-h2,h+h2],[-h2,h+h2+tip],[w,h+h2+tip],[w,0]];
linear_extrude(l)polygon(points);
}

//cover_unit();

difference(){cover_unit();
    translate([50,17.5,0])translate([0,0,-16])cylinder(h = 22, r = 7.9);
}
translate([50,17.5,0]){rotate([0,0,270])mirror([0,0,1])scale([1,1,1])test_locking();
    }

module round_hull(r,w)
{
    
$fn=Resolution;
hull()
{
    circle(r);
    translate([w,0,0,])circle(r);
    }    
    
    }
module drafted_pin(radius, hieght)
{
    

c1 = radius;
h = hieght;
c2 =(h < c1)? c1 - h : 0;

cylinder(h,c1,c2);    
    
    
    }

 module s_cover(width,length,thickness){
    
     
     
     
     union(){ difference()
    {
    linear_extrude(thickness)round_hull(width/2,length-width);
   translate([width/4+(length-width),0,0])cube([width/2,width,10],true);
   translate([-width/4,0,0])cube([width/2,width,10],true);
   
     
        } 
        thickness2 = 0.5;
        width2 = 29.7;
       
            translate([(length-width),0,0]){
                cylinder(h=thickness,r1 = width/2, r2 =width/2-(sqrt(2)*thickness));
            
            }
        
        }
    }  
   
    
module cover(width,length,thickness)
    {
//width = 30;
//length = 60;
//thickness = 2;
        
module joint()
        {
            
        
 rotate([90,0,0])translate([3,4,0])drafted_pin(3,2);
        
            }
     
          
Resolution = 50;
            

 union(){union(){       
translate([0,width/2,0])  
    s_cover(width,length,thickness);
     

   rotate([90,0,0])translate([3,4,-width])cylinder(width,4,4);
  
        joint();
        
 }mirror([0,1,0])translate([0,-width,0]) joint();}
        
 
 }
        
module cover_unit2(){

difference(){union(){
union()
{
translate([-15,-10,0]) cube([cover_length+3,cover_width+20,2.1]);
translate([-2,-10,2])cube([10,10,6.5]);    
 translate([45,17.5,3.6])rotate([0,180,-90])locking(w = 16, h = 1.5);

}
translate([0,cover_width,0])mirror([0,1,0]){translate([-2,-10,2])cube([10,10,6.5]);}
}
door_back_gap = 0.4;
translate([-door_back_gap,0,0])cube([door_back_gap,cover_width,3]);

#translate([0.5,35,0])mirror([0,1,0])rotate([90,0,0])translate([3,4,0])drafted_pin(3+0.15,2+0.15); 
#rotate([90,0,0])translate([3.5,4,0])drafted_pin(3+0.15,2+0.15);



//scale([1,1,1.04])translate([0,0,-0.05])
color("green"){cover(cover_width,cover_length,cover_thickness);}




    translate([cover_length-cover_width/2-2.8,cover_width/2,2]) cube([2,6.5,3],true);


translate([cover_length-cover_width/2-9.6+0.75,cover_width/2-4,2.2])cube([11-3.5,5+2.5,5]);
}


difference(){
    
union(){translate([0.5,0.15,0])color("blue"){cover(cover_width-0.3,cover_length-1,cover_thickness);}
        b_h = 0.6;
         b_w = 5;
         c_w = 2;
         h2 = 1;
         w2 = h2;
         w = 14.3- b_w;
                translate([cover_length-cover_width-0.12,cover_width/2,cover_thickness]){
 
         
        points = [[0-c_w,0],[0,b_h],[b_w-1,b_h],[b_w-1,b_h+h2],[b_w+w2,b_h+h2],[b_w,b_h],[b_w,0]];
    rotate([0,0,-90])rotate_extrude(angle = 180)translate([w,0,0])mirror([0,0,0])polygon(points);

            
            }

}
translate([cover_length-cover_width/2-4,cover_width/2-2.6,-1])cube([4,5+0.6,8]);
}

}


module locking(w = 1, h = 1)
{
    points = [[-5,0],[-5,h],[h,h],[0,0]];
    rotate_extrude(angle = 180)translate([w,0,0])mirror([1,0,0])polygon(points);

}



module cover_unit(){

difference(){union(){
union()
{
    
translate([-15,-10,0]) cube([cover_length+7,cover_width+20,2.1]);
translate([-2,-10,2])cube([10,10,6.5]);    

}
translate([0,cover_width,0])mirror([0,1,0]){translate([-2,-10,2])cube([10,10,6.5]);}
}
door_back_gap = 0.4;
translate([-door_back_gap,0,0])cube([door_back_gap,cover_width,3]);

translate([0.5,35,0])mirror([0,1,0])rotate([90,0,0])translate([3,4,0])drafted_pin(3+0.15,2+0.15); 
rotate([90,0,0])translate([3.5,4,0])drafted_pin(3+0.15,2+0.15);



//scale([1,1,1.04])translate([0,0,-0.05])
color("green"){cover(cover_width,cover_length,cover_thickness);}
}

translate([cover_length-16.8,cover_width/2+5,0])rotate([90,0,0])lock(w = 10,h =4.8,h2 = 8, l = 15,tip=0.5);



    

translate([0.5,0.25,0])color("blue"){cover(cover_width-0.5,cover_length-1.2,cover_thickness);}




}

