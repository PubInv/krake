
$fn = 50;
rotate_d = 15;
tip_lenght = 6;
thikness = 5;
width = 7;
height = 7;


// translate([-0.5,0,1.4])#sup_triangel();

module sup_triangel()
{
    h = 1;
points = [[(h),0],[-(h),0],[0,h]];
rotate([90,0,0])linear_extrude(10)polygon(points);
}
module locker()
{
    
    r1 = 7;
    
    cylinder(8,5,5);
    cylinder(3,1,r1);
    translate([0,0,5])mirror([0,0,1])cylinder(2,1,r1);
    translate([0,0,2])cylinder(h= 1, r = r1);
    translate([0,0,8.5])cube([2,9,2],true)
    
    ;}
    
    
    
    
    
    

    

    module the_tip(){
      {  difference(){
       translate([height-1,width/2+0.5,0.5])
       
       rotate([90,0,90]) linear_extrude(1)minkowski(){
       square([tip_lenght-1,thikness-1],false);
         
           
           
 //translate([height-1,(width/2)+tip_lenght,thikness/2])rotate([90,0,90])
            circle(r = 0.5);
        }
        
        
        translate([height-1-0.1,width/2-0.2,-0.1])rotate([90,0,90])linear_extrude(2)square([0.5+0.2,thikness+0.2]);
        }
       }}
       
    
       
   module latch(h = 10, width = width, thikness = thikness){
    union(){  
        
       translate([0,-width/2,0])cube([h,width,thikness]);
            cylinder(r =width/2,h = thikness);
       }
       }
       
       module lock_m(){
    difference(){
    union(){
       difference()
       {latch(height, width);
       translate([0,0,-0.1])latch(height*2,width-2.5,width+5);
           rotate([0,0,rotate_d])translate([0,-1,-1])cube([20,10,10]);
          translate([0,-1,-1]) cube([20,10,10]);
       }
       
       
       rotate([0,0,rotate_d])translate([3,1,0]){snap();}
       
       
       translate([0,-4,0])the_tip();
   
              rotate([0,0,rotate_d])translate([0,(width-1)/2-0.75,0])cube([height+0.5,1.25,thikness]);
       }
       translate([-1.9,0,8])rotate([90,0,-90])sup_triangel();
       }
       }
       
      // lock_m();
       
    translate([4.5,-2.5,-11.7])
       scale(1)translate([-4.5,2.5,11.7])rotate([90,90,0])lock_m();
 


 module snap(w = 5,h= height, t= thikness)
       {
           
           
           difference(){
       translate([height-5.5,(w/2)+2,t/2])rotate([0,0,0])cube([7.3,4,t],true);
    
               translate([height-3,w-(0.5),t/2])rotate([0,0,45])cube([5,8,t*2],true);
       
               translate([height-8,w-(0.5),t/2])rotate([0,0,45])cube([8,5,t*2],true);
             
           
     }
       
           
           }
       
