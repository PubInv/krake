/* [Box dimensions] */
// - Longueur - Length  
  Length        = 83.82+13;       
// - Largeur - Width
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
TShell          = 0;// [0:No, 1:Yes]
//Coque bas- Bottom shell
BShell          = 1;// [0:No, 1:Yes]
//Panneau avant - Front panel
FPanL           = 0;// [0:No, 1:Yes]
//Panneau arrière - Back panel  
BPanL           = 0;// [0:No, 1:Yes]
//Buttons
BButton         = 0;
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

module RoundBox($a=Length, $b=Width, $c=Height){// Cube bords arrondis
                    $fn=Resolution;            
                    translate([0,Filet,Filet]){  
                    minkowski (){                                              
                        cube ([$a-(Length/2),$b-(2*Filet),$c-(2*Filet)], center = false);
                        rotate([0,90,0]){    
                        cylinder(r=Filet,h=Length/2, center = false);
                            } 
                        }
                    }
                }// End of RoundBox Module

module Coque(){//Coque - Shell  
    Thick = Thick*2;  
    difference(){    
        difference(){//sides decoration
            union(){    
                     difference() {//soustraction de la forme centrale - Substraction Fileted box
                      
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
                                          RoundBox($a=Length-((2*Thick)+(2*m)), $b=Width-Thick, $c=Height-(Thick*2));
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
                                    }                          
                                }                                          


                difference(){// wall fixation box legs
                    union(){
                        translate([3*Thick +5,Thick,Height/2]){
                            rotate([90,0,0]){
                                    $fn=6;
                                    cylinder(d=16,Thick/2);
                                    }   
                            }
                            
                       translate([Length-((3*Thick)+5),Thick,Height/2]){
                            rotate([90,0,0]){
                                    $fn=6;
                                    cylinder(d=16,Thick/2);
                                    }   
                            }

                        }
                            translate([4,Thick+Filet,Height/2-57]){   
                             rotate([45,0,0]){
                                   cube([Length,40,40]);    
                                  }
                           }
                           translate([0,-(Thick*1.46),Height/2]){
                                cube([Length,Thick*2,10]);
                           }
                    } //Fin fixation box legs
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

module SpeakerHole(OnOff,Cx,Cy,Cdia,Ccenter=false){
    //difference(){
     if(OnOff==1)
    translate([Cx,Cy,-1]){
        for(j = [1  : 3]){
            
            rotate(a = 360*j/3,v = [0,0,1])
        for(i = [0.8 : Cdia/60 : Cdia/2]){
            rotate(a = 1*(i/Cdia)*360,v = [0,0,1])
            translate([i,i,0])
            
           cylinder(d=i*.65,h = 10, $fn=100,center=Ccenter);
           //echo(i);
            
            
        }
    }
    }
}
//Speaker Holder
module SpeakerHolder(OnOff,Cx,Cy,Cdia,Ccenter=false){ 
 if(OnOff==1){
 translate([Cx,Cy,-Thick])
        difference(){
        cylinder(d=Cdia+Thick,10, $fn=50,center=Ccenter);
        cylinder(d=Cdia,10, $fn=50,center=Ccenter);
    }
 }
}

//Button
module ButtonSwitch(OnOff,Cx,Cy,Cdia,Ccenter=false){
    //difference(){
     if(OnOff==1)
    translate([Cx,Cy,-1]){
     //rotate([180,0,0])
        
ThreadThick = 0.20;
//ThreadWidth = 0.40;

Protrusion = 0.9;

HoleWindage = 0.25;

//------
// Dimensions

Post = [3.86,3.86,3.0];

OD = 0;
HEIGHT =  FootHeight;
DOMEHEIGHT = 1;

Button = [12,0+Post[2],6*ThreadThick];

NumSides = 8*4;

//----------------------
//- Build it

    difference() {
	    union() {
        //retension ring
        translate([0,0,4*Thick])
            cylinder(d=Cdia+Thick,h=Thick,$fn=NumSides);
        //dome 
		translate([0,0,0])
			resize([0,0,2*Button[DOMEHEIGHT]])
				sphere(d=(Cdia-m/2)/cos(180/NumSides),$fn=NumSides);
        //Post
		cylinder(d=Cdia-m/2,h=FootHeight,$fn=NumSides);
        
	    }
    //retension hole
	translate([0,0,(FootHeight-Post[2]/2) ]){
		cube(Post + [HoleWindage,HoleWindage,Protrusion],center=true);
    }
}
    }
}        
        
module Panel(Length,Width,Thick,Filet){
    scale([0.5,1,1])
    minkowski(){
            cube([Thick,Width-(Thick*2+Filet*2+m),Height-(Thick*2+Filet*2+m)]);
            translate([0,Filet,Filet])
            rotate([0,90,0])
            cylinder(r=Filet,h=Thick, $fn=100);
      }
}        

//                          <- Circle Spacer -> 
// Cx=Cylinder X position | Cy=Cylinder Y position | Cdia= Cylinder dia | Cheight=Cyl height
module CylinderSpacer(OnOff,Cx,Cy,Cdia,Ccenter=false){
    if(OnOff==1)
    translate([Cx,Cy,Thick])
        cylinder(d=Cdia,10, $fn=50,center=Ccenter);
}

//                          <- Circle hole -> 
// Cx=Cylinder X position | Cy=Cylinder Y position | Cdia= Cylinder dia | Cheight=Cyl height
module CylinderHole(OnOff,Cx,Cy,Cdia,Ccenter=false){
    if(OnOff==1)
    translate([Cx,Cy,-1])
        cylinder(d=Cdia,10, $fn=50,center=Ccenter);
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

//                      <- Linear text panel -> 
module LText(OnOff,Tx,Ty,Font,Size,Content,_valign="baseline",_halign="left"){

    if(OnOff==1)
    translate([Tx,Ty,Thick+.5])
    linear_extrude(height = 0.5){
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
                linear_extrude(height = 0.5){
                text(Content[i], font = Font, size = Size,   halign = _halign,valign=_valign);
                    }
                }   
             }
      }
}

if(BShell==1){
// Coque bas - Bottom shell
    difference(){
        color(Couleur1,1){
            union(){ 
        Coque();
        translate( [3*Thick+2,Thick+5,5])SpeakerHolder(0,PCBLength-15.24,PCBWidth-FootPosX,11,Ccenter=true); 
                //Speaker holder
        // Pied support PCB - PCB feet
        if (PCBFeet==1){// Feet
            // Pieds PCB - PCB feet (x4) 
            translate([PCBPosX,PCBPosY,0]){ 
           // Feet();
            }   
            
        }
    }}
          color( Couleur1,1){
             translate( [3*Thick+2,Thick+5,0]){//([-.5,0,0]){
             //(On/Off, Xpos, Ypos, Diameter)
                SpeakerHole(1,PCBLength-15.24,15.24,11,Ccenter=true); //Buzzer
                SpeakerHole(0,PCBLength-15.24,PCBWidth-FootPosX,11,Ccenter=true); //Speaker
                CylinderHole(1,PCBLength-27.94,15.24,5); //LED1      
                CylinderHole(1,PCBLength-40.64,15.24,5); //LED2
                CylinderHole(1,PCBLength-53.34,15.24,5); //LED3
                CylinderHole(1,PCBLength-66.04,15.24,5); //LED4
                CylinderHole(1,PCBLength-78.74,15.24,5); //LED5
                CylinderHole(1,PCBLength-46.99,PCBWidth-FootPosX,5); //LED6 power
                                
             //(On/Off, Xpos,Ypos,Length,Width,Filet)
                SquareHole(1,PCBLength-50.8,71.12,26,76,0,Ccenter=true);   //Display
                CylinderHole(1,PCBLength-15.24,68.58,2); //reset hole
                 //(On/Off, Xpos, Ypos, "Font", Size, Diameter, Arc(Deg), Starting Angle(Deg),"Text",_halign = "center",_valign="top") 
                rotate([0,180,0])translate( [0,0,-(Thick+.99)])CText(1,-(PCBLength-10),31.75,"Arial Black",4,9,110,270,"MUTE");
                CylinderHole(1,PCBLength-10,31.75,15); //Mute Button
                CylinderSpacer(1,PCBLength-10,31.75,15+Thick+m/2); //cutout for mute button
                // SquareHole(1,PCBLength-63.87,33.12,1,1,0,Ccenter=true);   //testing
            }
        }
    }
}
