
use <Util_modules.scad>
include <General_paramters.scad>
include <imported_scadModels/StudModules.scad>

use <imported_scadModels/COVER.scad>
use <imported_scadModels/recessed.scad>
use <imported_scadModels/slot.scad>
use <imported_scadModels/flex_cover.scad>


GPAD_TshellDoorRecess = 1; // turn on/off door when krake Tshell is on 

recessed_module_on_off =1;
//// recess sub modules when recess module is on///

sd_door_on_off       = 0;
recessed_wall_on_off = 0;
recessed_bottom_on_off = 0;// turn on/off the recess moduel only

GPAD_TShellWithVESA  = 1; // Krake TShell 


AC_button_x = 38.77;
AC_button_y = 44.44;
/////////////////////////////////////////////////
///////// recess paramters //////////
R_height = Krake_76mmSPK_56h? Height - 40: 9;
button_pins_height = Krake_76mmSPK_56h? 12.8  : 6.85;
Door_recess_x_offset = Krake_rev2_76mmSPK? translationVariable - PCBLength+28 : -6.3;
Door_recess_y_offset = Krake_rev2_76mmSPK? PCBWidth  -93.5: 36.5;
///////////////////////////////////////

T_shell_part();

module T_shell_part(){
difference(){
if(GPAD_TShellWithVESA == 1){
    TShellWithVESA();
    }
else
{
    // Coque haut - Top Shell
    translate([0,Width,Height+0.2]){
        color( Couleur1,1){
            rotate([0,180,180]){
                Coque();
            }
        }
    }
    
}
if (GPAD_TshellDoorRecess == 1){
translate([Length/2 +Door_recess_x_offset,Width/2-Door_recess_y_offset,Height+0.25])rotate([180,0,90])translate([-15,-10,-3])cube([90,35+19,10]);
    
}}
if (GPAD_TshellDoorRecess == 1){

translate([Length/2 +Door_recess_x_offset,Width/2-Door_recess_y_offset,Height+0.25]){
    
rotate([180,0,90]){difference(){cover_unit(); 
translate([55,20,0])translate([0,0,-10])cylinder(h = 22, r = 7.9);


}
translate([55,20,0]){rotate([0,0,90])mirror([0,0,0])test_locking();

}
}

}

}}



module TShellWithVESA() {
stud_height_mm = 7.8;

//    h_offset_mm = 1;
h_offset_mm = 7.8;  //Set flush with Top.
    // Coque haut - Top Shell
difference() {
    translate([0,Width,Height+0.2]){
        color( Couleur1,1){
            rotate([0,180,180]){
                Coque();
            }
        }
    }
//        translate([Length/2,Width/2,Height+0.2]) 
   translate([Length/2-29,Width/2,Height+0.2]) 
    VESApunch75(stud_height_mm+15,h_offset_mm);
}

// Note: The Coque is not centered on the origin,
// so we have to do some math for that....
translate([Length/2 - 29 ,Width/2,Height+0.2]) 
color("blue"){
VESAmount75(stud_height_mm,h_offset_mm);
}

}


module VESApunch75(stud_height_mm,h_offset_mm) {
//    sh = stud_height_mm +2; // addin length for cutting
sh = stud_height_mm + -2; // addin length for cutting
radius = 4.5; // just a guess!

stud_distance_mm = 75;
sd = stud_distance_mm;
half_stud_distance_mm = sd/2;
hsd = half_stud_distance_mm;
translate([0,0,sh-h_offset_mm])
union() {
rotate([180,0,0])
translate([hsd,hsd,0])
cylinder(h=sh,r=radius,center = false);
rotate([180,0,0])
translate([hsd,-hsd,0])
cylinder(h=sh,r=radius,center = false);
rotate([180,0,0])
translate([-hsd,hsd,0])
cylinder(h=sh,r=radius,center = false);
rotate([180,0,0])
translate([-hsd,-hsd,0])
cylinder(h=sh,r=radius,center = false);
}
}



module VESAmount75(stud_height_mm,h_offset_mm) {
// A VESA mount for D 75mm
// from Wikipedia https://en.wikipedia.org/wiki/Flat_Display_Mounting_Interface
// D 75mm	MIS-D, 75, C	75	75

// First, I will create the 4 studs in the correct
// (square) pattern
stud_distance_mm = 75;
sd = stud_distance_mm;
half_stud_distance_mm = sd/2;
hsd = half_stud_distance_mm;

sh = stud_height_mm;
translate([0,0,sh-h_offset_mm])
union() {
rotate([180,0,0])
translate([hsd,hsd,0])
m4_stud(0,0,sh,0);
rotate([180,0,0])
translate([hsd,-hsd,0])
m4_stud(0,0,sh,0);
rotate([180,0,0])
translate([-hsd,hsd,0])
m4_stud(0,0,sh,0);
rotate([180,0,0])
translate([-hsd,-hsd,0])
m4_stud(0,0,sh,0);
}
}


module cover_slot() {
gap = -15.5;
slot_w = 1.3;
slot_h = 3;
slot_t = 0.5;
slot_l = 55;

difference(){union(){
translate([45.3-gap,90,25.5]){

mirror(v = [1,0,0]) translate([gap,0,0]) rotate([90,0,0]) slot(w = slot_w, h = slot_h, t = slot_t, l = slot_l);
translate([gap,0,0]) rotate([90,0,0]) slot(w = slot_w, h = slot_h, t = slot_t, l = slot_l);
}
}
translate([45.2,48+27.5+5,0])cube([-gap*2+0.2,4,50]);

} 
}

module recessed_module(){
translate([0,0,-R_height+9]){
    if (sd_door_on_off == 1){
        translate(v = [46.3+29,44+47.5,24.8]) rotate([0,0,180])flexiable_cover(n = 26, l = 29, h = 1, g = 1.6, t = 0.3);
        }
    if(recessed_bottom_on_off == 1)translate([0,5,0])cover_slot();

    }
if(recessed_wall_on_off == 1){
    difference(){
        reccessed_bottom_f(R_height = R_height);
        if(GPAD_TshellDoorRecess){
         cut_cube_height = 6;
        cut_cube_x = 24;
        cut_cube_y = 30;
        translate([cut_cube_x,cut_cube_y,Height-30-cut_cube_height])cube([10,10,cut_cube_height]);
        translate([cut_cube_x+50,cut_cube_y,Height-30-cut_cube_height])cube([10,10,cut_cube_height]);
        }}


}


if(recessed_bottom_on_off == 1){
translate([0,0,-R_height+9])AC_buttons_pins2(s_t = 0.5, s_w =0.5, B_height =button_pins_height);

translate([0,0,-R_height+9])difference(){
    reccesed_f();
    translate([AC_button_x,AC_button_y,26.8])cylinder(r=6,h=10);
    translate([AC_button_x,AC_button_y+29.67,26.8])cylinder(r=6,h=10);
    translate([AC_button_x+8,AC_button_y+7,26.5])
    {
        cube([23+5,40,3]);
    }  
        translate([45.2,48+10+27.5,10])cube([26.2+5,4,50]);
    }       
}
}
if(recessed_module_on_off == 1){
translate([Length/2 +Door_recess_x_offset,Width/2-Door_recess_y_offset,Height+0.25])translate([-34,-32,-40.2]) recessed_module();
}
