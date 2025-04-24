D-Sub Library for OpenScad
--------------------------

Introduction
-------------

This library is designed as an easy way to cut out D-Sub connectors by simply adding width of the connector. I didn't like how some people use rectangular cutouts for Db-9 connectors and wanted to have an actual proper shape for aesthetic and practical reasons. I took the opportunity to learn a bit about OpenScad while making something useful.


Install
-------

Simply drag and drop the file into "libraries" folder in OpenScad or drag and drop the code into the code window.

Using This Library
--------------------
This libary ads a module called "dsub" that contains 3 parameters in this order: "Scale", "Connector Size", "Hight"

The scale will likely need to be made a little bigger to allow for different housings especially if your connector is female. 1.1 works well for db9 and 1.05 for db25. These are the only connectors I have on hand and those are what I tested. The connector size must corrolate to the largest width of the connector. See the chart below for some sample sizes. VGA and RS232 are DB-9/ HD DB-15 while Parallel ports are DB-25:

DB-9/HD DB-15 -- 17.04
DB-15 -- 25.37
DB-25 -- 39.09
DB-50 -- 52.93

Below is a fast poorly written script to test it.

include <dsub.scad>

difference(){
translate([-8,-20,1]){
    cube([20,100,3]);
}
dsub(1.1,17.04,10);
translate([0,50,0]){
    dsub(1.05,39.09,10);
}
}

Other Info
-----------

For more information on D-Sub conectors you can find a pretty good link below. I didn't create this site but found it useful in writing this script.
 
http://www.interfacebus.com/Connector_D-Sub_Mechanical_Dimensions.html

Warranty & License 
---------

This script is provided without any warranty and the user takes full responsability for it's use. This script is public domain. 
