
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


module RoundBox2(Length, Width, Height,f=1){// Cube bords arrondis
$fn=Resolution;            

translate([f,f,0]) minkowski()
{
cube([Length-(2*f),Width-(2*f),Height-1]);
cylinder(r=f,h=1,false);
}

}// End of RoundBox Module


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


//                      <- Linear text panel -> 
module LText(OnOff,Tx,Ty,Font,Size,Content,_valign="baseline",_halign="left"){

if(OnOff==1)
translate([Tx,Ty,Thick+.5])
linear_extrude(height =0.7){
text(Content, size=Size, font=Font, halign = _halign,valign=_valign);
}
}


//                     <- Circular text panel->  
module CText(OnOff,Tx,Ty,Font,Size,TxtRadius,Angl,Turn,Content,_valign="baseline",_halign="center"){ 
if(OnOff==1) {
Angle = -Angl / len(Content);
translate([Tx,Ty,Thick+.5])
for (i= [0:len(Content)-1] ){   
rotate([0,0,i*Angle+90+Turn])
translate([0,TxtRadius,0]) {
linear_extrude(height = 0.7){
    text(Content[i], font = Font, size = Size,   halign = _halign,valign=_valign);
}
}   
}
}
}


////////////////////////////////////////////////////////////////////////
//                           <- Panel ->  
module Panel(Length,Width,Thick,Filet){
scale([0.5,1,1])
minkowski(){
cube([Thick,Width-(Thick*2+Filet*2+m),Height-(Thick*2+Filet*2+m)]);
translate([0,Filet,Filet])
rotate([0,90,0])
cylinder(r=Filet,h=Thick, $fn=100);
}
}


module beam2d(p1, p2, w) {
    hull() {
        translate(p1) circle(d = w, $fn = 20);
        translate(p2) circle(d = w, $fn = 20);
    }
}

function arch_y(x, width, arch_height) =
    arch_height == 0 ? 0 :
    let (r  = (pow(width/2, 2) + pow(arch_height, 2)) / (2*arch_height))
    let (cx = width/2)
    (arch_height + (r - arch_height)) - sqrt(max(r*r - pow(x - cx, 2), 0));


module xlattice_2d(width, height, cols, arch_height, bar_w, node_d) {
    col_w = width / cols;
    bpts = [ for (i = [0:cols]) [i*col_w, 0] ];
    tpts = [ for (i = [0:cols]) [i*col_w, height + arch_y(i*col_w, width, arch_height)] ];
    mpts = [ for (i = [0:cols]) [i*col_w, (bpts[i].y + tpts[i].y)/2] ];

    union() {
        for (i = [0:cols-1]) beam2d(bpts[i], bpts[i+1], bar_w);   // bottom rail
        for (i = [0:cols-1]) beam2d(tpts[i], tpts[i+1], bar_w);   // top rail
        for (i = [0:cols]) {                                      // posts
            beam2d(bpts[i], mpts[i], bar_w);
            beam2d(mpts[i], tpts[i], bar_w);
        }
        for (i = [0:cols-1]) {                                    // X bracing
            beam2d(bpts[i],   tpts[i+1], bar_w);
            beam2d(bpts[i+1], tpts[i],   bar_w);
        }
        for (i = [0:cols]) {                                      // joint nodes
            translate(bpts[i]) circle(d = node_d, $fn = 28);
            translate(mpts[i]) circle(d = node_d, $fn = 28);
            translate(tpts[i]) circle(d = node_d, $fn = 28);
        }
    }
}

module rib_slab(panel_w, panel_h, cols, arch_height, bar_w, node_d, rib_depth) {
    linear_extrude(height = rib_depth)
        xlattice_2d(panel_w, panel_h, cols, arch_height, bar_w, node_d);
}


module gusset(h = 15, run = 10, t = 1.5, fillet_r = 0.6) {
    // right angle sits at the wall/floor corner (origin);
    // profile drawn in the X (floor) - Z (wall) plane, then
    // extruded sideways along Y by thickness t.
    rotate([90, 0, 90])
        linear_extrude(height = t)
            hull() {
                translate([0, 0])      circle(r = fillet_r, $fn = 16); // root corner
                translate([run, 0])    circle(r = fillet_r, $fn = 16); // floor tip
                translate([0, h])      circle(r = fillet_r, $fn = 16); // wall tip
            }
}

module gusset_row(wall_len   = 100,
                   height     = 15,
                   run        = 10,
                   thick      = 1.5,
                   fillet_r   = 0.6,
                   spacing    = 25,     // used only if positions not given
                   margin     = 8,
                   positions  = undef) {

    xs = (positions != undef) ? positions :
         let(usable = wall_len - 2*margin,
             n      = max(floor(usable / spacing), 1))
         [ for (i = [0:n]) margin + i * (usable / n) ];

    for (x = xs)
        translate([x - thick/2, 0, 0])
            gusset(h = height, run = run, t = thick, fillet_r = fillet_r);
}
