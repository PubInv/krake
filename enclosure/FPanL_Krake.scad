/* [Box dimensions] */
// - Longueur - Length  
  Length        = 83.82+13;       
// - Largeur - Width
  //Width         = 138+13;   
  Width         = 138+13;                     
// - Hauteur - Height  
  Height        = 40;  
// - Epaisseur - Wall thickness  
  Thick         = 2;//[2:5]  
  
/* [Box options] */
// - Diamètre Coin arrondi - Filet diameter  
  Filet         = 2;//[0.1:12] 
// - lissage de l'arrondi - Filet smoothness  
  Resolution    = 50;//[1:100] 
// - Tolérance - Tolerance (Panel/rails gap)
  m             = 0.9;
// Pieds PCB - PCB feet (x4) 
  PCBFeet       = 1;// [0:No, 1:Yes]
// - Decorations to ventilation holes
  Vent          = 1;// [0:No, 1:Yes]
// - Decoration-Holes width (in mm)
  Vent_width    = 1.5;   


  
/* [PCB_Feet] */
//All dimensions are from the center foot axis

// - Coin bas gauche - Low left corner X position
PCBPosX         = 0;
// - Coin bas gauche - Low left corner Y position
PCBPosY         = 0;
// - Longueur PCB - PCB Length
PCBLength       = 83.82;
// - Largeur PCB - PCB Width
PCBWidth        = 137.16;
// - Heuteur pied - Feet height
PCBThick        = 1.6;
// - Heuteur pied - Feet height
FootHeight      = 15;
// - Diamètre pied - Foot diameter
FootDia         = 7;
// - Diamètre trou - Hole diameter
FootHole        = 3.0;  
// - Coin bas gauche - Low left corner X position
FootPosX         = 5.08;
// - Coin bas gauche - Low left corner Y position
FootPosY         = 5.08;

/* [STL element to export] */
//Coque haut - Top shell
GPAD_TShell          = 0;// [0:No, 1:Yes]
//Coque bas- Bottom shell
GPAD_BShell          = 0;// [0:No, 1:Yes]
//Panneau avant - Front panel
GPAD_FPanL           = 1;// [0:No, 1:Yes]
//Panneau arrière - Back panel  
GPAD_BPanL           = 0;// [0:No, 1:Yes]
//Buttons
BButton         = 0;
//T and BShellScrew
T_BShellScrew   = 1;
BOSSScrew = 0;
//show pcb
PCB_View        = 0;
LED_Standoff    = 0;
LED_Standoff_Single    = 1; 
/* [Hidden] */
// - Couleur coque - Shell color  
Couleur1        = "Orange";       
// - Couleur panneaux - Panels color    
Couleur2        = "OrangeRed";    
// Thick X 2 - making decorations thicker if it is a vent to make sure they go through shell
Dec_Thick       = Vent ? Thick*2 : Thick; 
// - Depth decoration
Dec_size        = Vent ? Thick*2 : 0.8;

// Krake Modifications for SpeakerHole
SpeakerHoleY = 15.24;
SpeakerHoleX = PCBLength-15.24;

//Krake Modifications for Display
 DisplayXpos = PCBLength-50.8;
 DisplayYpos = 71.12;
 DisplayLenght = 26;
 DisplayWidth = 76;
 DisplayFilet = 0;
 
 //Krake Modificationd for muteButton
 muteButtonXpos = PCBLength-10;
 muteButtonYpos = 31.75;
 muteButtonDiameter = 15;
 //For instructions on mute Button inscription go to line 594.
 
//                           <- Panel ->  
module Panel(Length,Width,Thick,Filet){
    scale([0.5,1,1])
    %minkowski(){
            cube([Thick,Width-(Thick*2+Filet*2+m),Height-(Thick*2+Filet*2+m)]);
            translate([0,Filet,Filet])
            rotate([0,90,0])
            cylinder(r=Filet,h=Thick, $fn=100);
      }
}

//                      <- Linear text panel -> 
module LText(OnOff,Tx,Ty,Font,Size,Content,_valign="baseline",_halign="left"){

    if(OnOff==1)
    translate([Tx,Ty,Thick+.5])
    linear_extrude(height = 0.5){
    text(Content, size=Size, font=Font, halign = _halign,valign=_valign);
    }
}

//                          <- Square hole ->  
// Sx=Square X position | Sy=Square Y position | Sl= Square Length | Sw=Square Width | Filet = Round corner
module SquareHole(OnOff,Sx,Sy,Sl,Sw,Filet,Ccenter=false){
    if(OnOff==1)
     minkowski(){
        translate([Sx+Filet/2,Sy+Filet/2,-1])
            cube([Sl-Filet,Sw-Filet,10],center=Ccenter);
            cylinder(d=Filet,h=10, $fn=100,center=Ccenter);
       }
}
 
 // Panneau avant - Front panel  <<<<<< Text and holes only on this one.
//rotate([0,-90,-90]) 
if(GPAD_FPanL==1){
        translate([Length-(Thick*2+m/2),Thick+m/2,Thick+m/2])
        FPanL();
}

module FPanL(){
    difference(){
        color(Couleur2)
        Panel(Length,Width,Thick,Filet);
    
 
    rotate([90,0,90]){
        color(Couleur2){
//                     <- Cutting shapes from here ->  
            //(On/Off, Xpos,Ypos,Length,Width,Filet)3*Thick+2,Thick+5
        echo((Width - PCBWidth)/2-3*Thick+1);
            echo(Thick+1.2);
            translate([((Width - PCBWidth)/2),0,0]-[3*Thick+2,0,0]){
        SquareHole  (1,54.61+1.2,FootHeight+PCBThick,9,5,1,Ccenter=false); //USB
        SquareHole  (1,81.28-1.2,FootHeight+PCBThick,14,9,1,Ccenter=false); //I2C
        SquareHole  (1,98.425-1.3,FootHeight+PCBThick,14,13,1,Ccenter=false); //SPI
        SquareHole  (1,119.38+0.8,FootHeight+PCBThick,10,12,1,Ccenter=false); //DC barrel
            }
       

//                            <- To here -> 
           }
       }
}
  color(Couleur1){
        translate ([-.5,0,0])
        rotate([90,0,90])        
        translate([((Width - PCBWidth)/2) ,0,0]+[-2.3,0,0]){
//                      <- Adding text from here ->   
      //(On/Off, Xpos, Ypos, "Font", Size, "Text",_halign = "center",_valign="top")  
        LText(1,54.61,FootHeight*.9,"Arial Black",3,"USB",_halign = "center",_valign="top");
        LText(1, 81.28   ,FootHeight*.9,"Arial Black",3,"I2C",_halign = "center",_valign="top");
        LText(1,98.425,FootHeight*.9,"Arial Black",3,"Remote",_halign = "center",_valign="top");
        LText(1,119.38,FootHeight*.9,"Arial Black",3,"DC",_halign = "center",_valign="top");    
       // SquareHole(1,0,FootHeight*.9,1,1,0,Ccenter=true); //origin
//                            <- To here ->
            }
      }
}

module frontPanel(){
    difference() {
        Coque();
    
        SpeakerHole(OnOff = 1, Cx = 30, Cy = 20, Cdia = 20, Ccenter = true);
    }
}
  
