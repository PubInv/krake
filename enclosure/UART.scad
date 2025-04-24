
include <C:\Users\Laptop Pro\OneDrive\Desktop\PPF\PbF\4pi\Esp32BasedProjects\krake\enclosure\DB9-Library-SCAD\files\D-Sub\dsub.scad>

centerDB9X = 0;
centerDB9Y = 0;
centerDB9Z = 0;

difference(){
    translate([-8,-20,1]){
        cube([20,40,3]);
    }


    //Locate and punch DB9
    translate([centerDB9X,centerDB9Y,centerDB9Z]){
dsub(1.17,17.04,10);
    }
    //Locate and punch ?DB25?
    /*
    translate([0,50,0]){
        dsub(1.05,39.09,10);
    } */
}