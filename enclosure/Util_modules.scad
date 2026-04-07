
include <General_paramters.scad>

/////////// - Boitier générique bord arrondis - Generic rounded box - //////////
module RoundBox($a=Length, $b=Width, $c=Height){// Cube bords arrondis
$fn=Resolution;            
translate([0,Filet,Filet]){  
    minkowski (){                                              
        cube ([$a-(Length/2),$b-(2*Filet),$c-(2*Filet)], center = false);
        rotate([0,90,0]){    
            cylinder(r=Filet,h=Length/2, center = false);
}  } }
}// End of RoundBox Module
//strange cube sizing to mitigate the fact after minkowski the box would get larger as addition of cylinder size plus cube size thus desired length/2, desired width - 2 times the extra added because of the cylinder
//confused on the translate and rotate requirements , when you rotate it it drops below the line so you need to translate it up to keep it on the right plane, the rotate means the cylinder is on the longest edge off the cube
////////////////////////////////////////////////////////////////////  



//Module Coque/Shell          
module Coque(){//Coque - Shell  

Thick = Thick*2;  
difference(){    
difference(){//sides decoration
union(){    
difference() {//soustraction de la forme centrale - Substraction Fileted box
//??unsure why subtracting a fileted box after subtracting a cube
difference(){//soustraction cube median - Median cube slicer
union() {//union               
    difference(){//Coque    
        RoundBox();
        translate([Thick/2,Thick/2,Thick/2]){     
            RoundBox($a=Length-Thick, $b=Width-Thick, $c=Height-Thick);
        }
    }//Fin diff Coque                            
    difference(){//largeur Rails        
        translate([Thick+m,Thick/2,Thick/2]){// Rails
            RoundBox($a=Length-((2*Thick)+(2*m)), $b=Width-Thick, $c=Height                             -(Thick*2));
        }//fin Rails
        translate([((Thick+m/2)*1.55),Thick/2,Thick/2+0.1]){ // +0.1 added to avoid the artefact
            RoundBox($a=Length-((Thick*3)+2*m), $b=Width-Thick, $c=Height-Thick);
        }           
    }//Fin largeur Rails
}//Fin union                                   
translate([-Thick,-Thick,Height/2]){// Cube à soustraire
    cube ([Length+100, Width+100, Height], center=false);
}                                            
}//fin soustraction cube median - End Median cube slicer
translate([-Thick/2,Thick,Thick]){// Forme de soustraction centrale 
RoundBox($a=Length+Thick, $b=Width-Thick*2, $c=Height-Thick);       
}                          }                                          
difference(){// wall fixation box legs
union(){
translate([3*Thick +5,Thick,Height/2]){
    rotate([90,0,0]){
        $fn=6;
        cylinder(d=16,Thick/2);
    }  }
translate([Length-((3*Thick)+5),Thick,Height/2]){
    rotate([90,0,0]){
        $fn=6;
        cylinder(d=16,Thick/2);
   }  }}
translate([4,Thick+Filet,Height/2-57]){   
rotate([45,0,0]){
    cube([Length,40,40]);    
}}
translate([0,-(Thick*1.46),Height/2]){
cube([Length,Thick*2,10]);
}} //Fin fixation box legs 
}
union(){// outbox sides decorations
for(i=[0:Thick:Length/4]){
// Ventilation holes part code submitted by Ettie - Thanks ;) 
translate([10+i,-Dec_Thick+Dec_size,1]){
cube([Vent_width,Dec_Thick,Height/4]);
}
translate([(Length-10) - i,-Dec_Thick+Dec_size,1]){
cube([Vent_width,Dec_Thick,Height/4]);
}
translate([(Length-10) - i,Width-Dec_size,1]){
cube([Vent_width,Dec_Thick,Height/4]);
}
translate([10+i,Width-Dec_size,1]){
cube([Vent_width,Dec_Thick,Height/4]);
}
}// fin de for
// }
}//fin union decoration
}//fin difference decoration
union(){ //sides holes
$fn=50;
translate([3*Thick+5,20,Height/2+4]){
rotate([90,0,0]){
cylinder(d=2,20);
}
}
translate([Length-((3*Thick)+5),20,Height/2+4]){
rotate([90,0,0]){
cylinder(d=2,20);
}
}
translate([3*Thick+5,Width+5,Height/2-4]){
rotate([90,0,0]){
cylinder(d=2,20);
}
}
translate([Length-((3*Thick)+5),Width+5,Height/2-4]){
rotate([90,0,0]){
cylinder(d=2,20);
}
}
}//fin de sides holes

}//fin de difference holes
}// fin coque 
