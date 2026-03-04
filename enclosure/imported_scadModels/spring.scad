$fn =50;

module simi_c(w,t,h){
difference(){
    cylinder(h =h,r=w/2);
    translate([0,0,-0.5])cylinder(h =h+1,r=w/2-t);
    translate([-w/2,0+0.1,-0.5])cube([w,w/2,h+1]);
        }
    }
module spring(w,t,h,n)
{
    
    for(i = [0:n-1])
    {
translate([(w-t)*i,0,0])mirror([0,i%2,0])simi_c(w,t,h);        
        
        
        }
    }


spring(3,0.5,0.2,10);