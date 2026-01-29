module cover_flex(w,h,l,g)
{
union(){
    cube([w,1,h],false);
    if(l == 0)
        {
            cube([1,g,h]);
            }

else if(l == 1)
    {
            translate([w-1,0,0])cube([1,g,h]);
        
        }    
    
    }
        }

g= 1.6;
n = 21;
l = 30;
t = 0.3;
h =1.5;
flexiable_cover(n,l,h,g,t);
module flexiable_cover(n,l,h,g,t) {
  union(){

    translate(v = [0,-g,0]) for (a =[1:n]){translate([0,a*g,t]){
    if(a !=n )
        cover_flex(l,h,a%2,g);
    else
        cover_flex(l,h,-12,g);
    }
    }
    
    cube([l,(n-0.5)*g,t],false);
    translate([l/2,0,0]) rotate([-90,0,0])difference(){
    cylinder(h=1,r=4);
    translate([0,2.5,0])cube([50,5,5],true);
    }
}
}