////////////////////// <- New module Panel -> //////////////////////

use <Util_modules.scad>
include <General_paramters.scad>

include <imported_scadModels/dsub.scad>

FPanL();

module FPanL(){

centerDB9X = -25;
centerDB9Y = -80;
centerDB9Z = -5;

difference(){
color(Couleur2)
Panel(Length,Width,Thick,Filet);

rotate([90,0,90]){

color(Couleur2){

//                     <- Cutting shapes from here ->  
//(On/Off, Xpos,Ypos,Length,Width,Filet)3*Thick+2,Thick+5
echo((Width - PCBWidth)/2-3*Thick+1);
echo(Thick+1.2);
translate([((Width - PCBWidth)/2), 0, 0] - [3*Thick+2, 0, 0]){
    if (DE9On ==1) {
        rotate([0,180,90]) translate([centerDB9X,centerDB9Y,centerDB9Z])
        dsub (1.2,17.04,10);
    }
    USBbSquareHole(USBbOn, 54.61+1.2, FootHeight+PCBThick, 9, 5, 1, Ccenter=false);//USBb
    USBcSquareHole(USBcOn, 51+1.45, FootHeight-0.5+PCBThick-1, 11, 6, 1, Ccenter=false);//USBc
    I2CSquareHole(I2COn, 81.28-1.2, FootHeight+PCBThick, 14, 9, 1, Ccenter=false);//I2C
    //DE9SquareHole(DE9On, 74-9.5, FootHeight+PCBThick, 30, 13.2, 1, Ccenter=false);//DE9

    RJ12SquareHole(RJ12On, 98.425-1.3, FootHeight+PCBThick, 14, 13, 1, Ccenter=false);//SPI
    DCSquareHole(DCOn, 119.38+0.8, FootHeight+PCBThick, 10, 12, 1, Ccenter=false);//DC barrel
}}}

//                            <- To here -> 

color(Couleur1){
translate ([-0.9, 4,-3])
// module LText(OnOff,Tx,Ty,Font,Size,Content,_valign="baseline",_halign="left")
rotate([-90,0,-90])    
// module LText(OnOff,Tx,Ty,Font,Size,Content,_valign="baseline",_halign="left")
{
Fontsize = 3; 
TextBaseLine = - FootHeight * .92 ; 
//                      <- Adding text from here ->   

LText(USBbOn,-55,TextBaseLine,"Arial Black",Fontsize,"USB",_halign = "center",_valign="top");
LText(USBcOn,-53,TextBaseLine,"Arial Black",Fontsize,"USB",_halign = "center",_valign="top");
LText(I2COn, -81.28,TextBaseLine,"Arial Black",Fontsize,"I2C",_halign = "center",_valign="top");
LText(DE9On, -74.44,TextBaseLine,"Arial Black",Fontsize,"COM",_halign = "center",_valign="top");
LText(RJ12On,-98.425,TextBaseLine,"Arial Black",Fontsize,"Remote",_halign = "center",_valign="top");
LText(DCOn,-119.9 ,TextBaseLine,"Arial Black",Fontsize,"DC",_halign = "center",_valign="top");    

//                            <- To here ->
}
}
}
}



////////////////////////////////////////////////////////////////////////
////////////////////// <- Holes Panel Manager -> ///////////////////////

            //              <- Square hole ->  
// Sx=Square X position | Sy=Square Y position | Sl= Square Length | Sw=Square Width | Filet = Round corner
module USBbSquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
if(OnOff==1)
minkowski(){
translate([Sx+Filet/2,Sy+Filet/2,-1])
cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
}
}
module USBcSquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
if(OnOff==1)
minkowski(){
translate([Sx+Filet/2,Sy+Filet/2,-1])
cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
}
}
module I2CSquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
if(OnOff==1)
minkowski(){
translate([Sx+Filet/2,Sy+Filet/2,-1])
cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
}
}

module DE9SquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
if(OnOff==1)
minkowski(){
translate([Sx+Filet/2,Sy+Filet/2,-1])
cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
}
}

module RJ12SquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
if(OnOff==1)
minkowski(){
translate([Sx+Filet/2,Sy+Filet/2,-1])
cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
}
}
module DCSquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
if(OnOff==1)
minkowski(){
translate([Sx+Filet/2,Sy+Filet/2,-1])
cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
}
}

