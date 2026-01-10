
$fs = 0.1;



use <latch.scad>


cover_length = 80;
cover_width = 35;
cover_thickness = 2.1;
Resolution = 50;
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
        
            translate([(length-width),0,0])cylinder(h=thickness,r1 = width/2, r2 =width/2-(sqrt(2)*thickness));
        
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
        





 
 
module cover_unit(){
difference(){union(){
union()
{
translate([-15,-10,0]) cube([cover_length+3,cover_width+20,2.1]);
translate([-2,-10,2])cube([10,10,8]);    
}
translate([0,cover_width,0])mirror([0,1,0]){
    translate([-2,-10,2])cube([10,10,8]);
    }
}

#translate([-1.7,0,0])cube([3,cover_width,3]);
/*
translate([0,30,0])mirror([0,1,0])rotate([90,0,0])translate([3,4,0])drafted_pin(3+0.15,2+0.15); 
rotate([90,0,0])translate([3,4,0])drafted_pin(3+0.15,2+0.15);

*/

//scale([1,1,1.04])translate([0,0,-0.05])
color("green"){cover(cover_width,cover_length,cover_thickness);}

//color("red")translate([46.6,18,7])rotate([90,90,0])scale([1.05,1.05,1.05])lock_m();

    translate([cover_length-cover_width/2-2.8,cover_width/2,2]) cube([2,6.5,3],true);

//translate([3,15,-1])s_cover(cover_width,cover_length,cover_thickness+2);
}


difference(){
translate([0,0.3,0])color("blue"){cover(cover_width-0.6,cover_length-1,cover_thickness);}
#translate([cover_length-cover_width/2-9.6,cover_width/2-2.6,-1])cube([11,5+0.6,8]);
}



//width = 30-0.6;
//length = 50-0.6;
//thickness=2;


difference(){
color("red")translate([cover_length-cover_width/2-6.6,cover_width/2+2.7,7])rotate([90,90,0])lock_m();
 /*difference()
    {
        
 color("green"){cover(cover_width,cover_length,cover_thickness);}
        
 color("green")scale(0.99){cover(cover_width,cover_length,cover_thickness);}
        }*/
translate([-0.5,0,-0.2])translate([cover_length-cover_width/2,cover_width/2,cover_thickness])rotate([0,45,0])translate([1,0,0])cube([sqrt(2)*cover_thickness,cover_width+2,sqrt(2)*cover_thickness],true);
       
}

tollerance = 0.1;
difference(){
translate([cover_length-cover_width/2-1.8,cover_width/2-3.5,2])cube([3,7,3]);
    
        translate([cover_length-cover_width/2-6.6,cover_width/2+3,6.2])    translate([4.5,-2.5,-11.7])
       scale([1+tollerance,2,1+tollerance])translate([-4.5,2.5,11.7])rotate([90,90,0])lock_m();

    
    }

}

cover_unit();