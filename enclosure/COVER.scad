
$fs = 0.1;

use <latch.scad>



module drafted_pin(radius, hieght)
{
    

c1 = radius;
h = hieght;
c2 =(h < c1)? c1 - h : 0;

cylinder(h,c1,c2);    
    
    
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
     
module s_cover(){
    
difference(){    cube([length,width,thickness]);
translate([length,width/2,thickness])rotate([0,45,0])cube([sqrt(2)*thickness,width+2,sqrt(2)*thickness],true);
}    
    }


 union(){union(){       
translate([3,0,0])s_cover();
   rotate([90,0,0])translate([3,4,-width])cylinder(width,4,4);
  
        joint();
        
 }mirror([0,1,0])translate([0,-width,0]) joint();}
        
 
 }
        



difference(){union(){
union()
{
translate([-15,-20,0]) cube([70,70,2]);
translate([-2,-10,2])cube([10,10,8]);    
}
translate([0,30,0])mirror([0,1,0]){
    translate([-2,-10,2])cube([10,10,8]);
    }
}

translate([0,30,0])mirror([0,1,0])rotate([90,0,0])translate([3,4,0])drafted_pin(3+0.15,2+0.15); 
rotate([90,0,0])translate([3,4,0])drafted_pin(3+0.15,2+0.15);
color("green"){cover(30,50,2);}
translate([-1,0,0])cube([50,30,2]); 
}


difference(){
translate([0,0.3,0])color("blue"){cover(30-0.6,50-0.6,2);}
translate([46-2,15-2-0.3,-1])cube([9,5+0.6,8]);
}



width = 30-0.6;
length = 50-0.6;
thickness=2;


difference(){
color("red")translate([46.6,18,7])rotate([90,90,0])lock_m();
 
translate([3,0.3,0])translate([length,width/2,thickness])rotate([0,45,0])translate([1,0,0])cube([sqrt(2)*thickness,width+2,sqrt(2)*thickness],true);
}

tollerance = 0.1;
difference(){
translate([46+5,15-2.5-0.3,2])cube([3,7,3]);
    
        translate([46.6,18,6.2])    translate([4.5,-2.5,-11.7])
       scale([1+tollerance,2,1+tollerance])translate([-4.5,2.5,11.7])rotate([90,90,0])lock_m();

    
    }
    
