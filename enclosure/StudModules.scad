/*
	By Ron Webb
	bigron@gmail.com
	February 11, 2023
	Version 1.0
	
	I am still VERY new to using OpenSCAD but one feature that I KNOW I will 
	be taking advantage of is modules. There are items that I know I will be
	using in many different designs. The first that I could think of was 
	creating standoffs for circuit boards that facilitate placing a heat-
	set insert into. The most common heat-set inserts used are M2.5, which
	is the standard for security Raspberry Pi single-board computers, and 
	M3, pretty much the most common size I've seen used for circuit boards.
	There are times when you need to go with smaller or larger and shorter
	or longer. 
	
	The method for building each standoff studs is essentially the same, 
	just the values of each dimension varies. I was finding different 
	values on these inserts, depending on where I was looking. Some 
	sources gave more info than others, so I analyzed what I knew and
	extrapolated values that I could not find published. Maker Tech Store
	(https://www.makertechstore.com) the most thurough information about 
	the inserts that they sell but unfortunately, they do not sell all sizes. 
	McMaster-Carr (https://www.mcmaster.com/heat-set-inserts/) has a much
	larger array of sizes but their information is much more limited, as well
	as mixing metric and imperial sizes in the description of each item. My
	inserts are mostly Alibaba specials from unknown sources.
	
	I tried to explain everything in each module, so hopefully it will make
	it easier for you to modify things if they are not right for you. If you
	wish to use any module, you can copy whatever portion you wish. In order
	to call each module, you would do so like the following:
	
				m3_long_stud(x-axis,y-axis,desired height,offset from surface);
				
				or 
				
				m3_long_stud(0,0,7.8,0);
	
	The first example shows a description while the second would put a 7.8mm 
	standoff stud at 0,0,0. You can make each standoff stud as tall as you 
	would like but due to the length of each insert, there is a minimum height.
	If you were to use a height that is less than the minimum, such as 1, it 
	will place the shortest possible standoff stud at the location. The minimal
	height stud will have the inner hole extend all the way down to the base
	or offset level. Anything taller will have the length of the insert plus
	a 0.76mm gap below, per recommendations.
	
*/
//$fn= $preview ? 32 : 64;
//
//
//rotate([0,0,90])	{
//	translate([-10,0,0]) linear_extrude(1.5) text( "M3 Long" ,
//	halign = "right" ,valign = "center" ,size = 5 );
//	}
//
//m3_long_stud(0,0,7.8,0);



//M2.0 "Short" Heat-set insert
module m2_short_stud(x,y,sh,off=0) 
	{	
	maxid = 3.58;       				//Maximum Insert Diameter
	ted = 3.12;         				//Tapered End Diameter
	oil = 2.92;         				//Overal Insert Length
	ophs = 3.00;        				//Optimum Pilot Hole Size
	oshd = 3.12;        				//Optimum Surface Hole Diameter
	rmwt = maxid*0.53;  				//Recommended Min Wall Thickness
	ahd = 0.76;         				//Added Hole Depth for Blind Holes
	th = 0.43;          				//Height of 8° Taper
	cham = 0.5;							//Chamfer at top of stud
	stud_top = maxid+(rmwt*2); 			//Top of Stud Diameter
	stud_bot = stud_top+(cham*2); 		//Bottom of Stud Diameter
		/*
		The following line assures that the overall stud height is AT LEAST
		the length of the heat-set insert PLUS a clearance for blind holes.
		You can request a height TALLER than the minimum, just not less.	
		*/
	st_h = sh > (oil+ahd) ? sh : oil+ahd;
	st_base = st_h-cham;		//Height of Stud to chamfer
		/*
		The following is a convoluted variable that keeps the hole from going
		unnecessarily deep if using a stud height is taller than the minimum 
		height. "off" is the offset and "st_h" is the total length of
		the stud. Adding those two variables sets that total height above zero.
		Now, we want to know where to set the base of the hole; the hole depth
		should be insert length ("oil") plus the clearance for a blind hole
		insertion ("ahd").
		*/
	hh = (off+st_h)-(oil+ahd);


	difference() {
		union() {
			translate([x,y,off]) cylinder(d=stud_bot,st_base);
			translate([x,y,st_base+off]) cylinder(d1=stud_bot,d2=stud_top,cham);
		}
		
		#union() {
			translate([x,y,hh]) cylinder(d=ophs,oil+ahd);
			translate([x,y,st_h-th+off]) cylinder(d1=ophs,d2=oshd,th);
		}			
	}	
}

//M2.0 "Long" Heat-set insert
module m2_long_stud(x,y,sh,off=0) 
	{	
	maxid = 3.58;       				//Maximum Insert Diameter
	ted = 2.84;         				//Tapered End Diameter
	oil = 4.78;         				//Overal Insert Length
	ophs = 2.70;        				//Optimum Pilot Hole Size
	oshd = 2.84;        				//Optimum Surface Hole Diameter
	rmwt = maxid*0.53;  				//Recommended Min Wall Thickness
	ahd = 0.76;         				//Added Hole Depth for Blind Holes
	th = 0.50;          				//Height of 8° Taper
	cham = 0.5;							//Chamfer at top of stud
	stud_top = maxid+(rmwt*2); 			//Top of Stud Diameter
	stud_bot = stud_top+(cham*2); 		//Bottom of Stud Diameter
		/*
		The following line assures that the overall stud height is AT LEAST
		the length of the heat-set insert PLUS a clearance for blind holes.
		You can request a height TALLER than the minimum, just not less.	
		*/
	st_h = sh > (oil+ahd) ? sh : oil+ahd;
	st_base = st_h-cham;		//Height of Stud to chamfer
		/*
		The following is a convoluted variable that keeps the hole from going
		unnecessarily deep if using a stud height is taller than the minimum 
		height. "off" is the offset and "st_h" is the total length of
		the stud. Adding those two variables sets that total height above zero.
		Now, we want to know where to set the base of the hole; the hole depth
		should be insert length ("oil") plus the clearance for a blind hole
		insertion ("ahd").
		*/
	hh = (off+st_h)-(oil+ahd);


	difference() {
		union() {
			translate([x,y,off]) cylinder(d=stud_bot,st_base);
			translate([x,y,st_base+off]) cylinder(d1=stud_bot,d2=stud_top,cham);
		}
		
		#union() {
			translate([x,y,hh]) cylinder(d=ophs,oil+ahd);
			translate([x,y,st_h-th+off]) cylinder(d1=ophs,d2=oshd,th);
		}			
	}	
}

//M2.5 "Short" Heat-set insert
module m25_short_stud(x,y,sh,off=0) 
	{	
	maxid = 4.37;       				//Maximum Insert Diameter
	ted = 3.99;         				//Tapered End Diameter
	oil = 3.43;         				//Overal Insert Length
	ophs = 3.89;        				//Optimum Pilot Hole Size
	oshd = 4.04;        				//Optimum Surface Hole Diameter
	rmwt = maxid*0.53;  				//Recommended Min Wall Thickness
	ahd = 0.76;         				//Added Hole Depth for Blind Holes
	th = 0.53;          				//Height of 8° Taper
	cham = 0.5;							//Chamfer at top of stud
	stud_top = maxid+(rmwt*2); 			//Top of Stud Diameter
	stud_bot = stud_top+(cham*2); 		//Bottom of Stud Diameter
		/*
		The following line assures that the overall stud height is AT LEAST
		the length of the heat-set insert PLUS a clearance for blind holes.
		You can request a height TALLER than the minimum, just not less.	
		*/
	st_h = sh > (oil+ahd) ? sh : oil+ahd;
	st_base = st_h-cham;		//Height of Stud to chamfer
		/*
		The following is a convoluted variable that keeps the hole from going
		unnecessarily deep if using a stud height is taller than the minimum 
		height. "off" is the offset and "st_h" is the total length of
		the stud. Adding those two variables sets that total height above zero.
		Now, we want to know where to set the base of the hole; the hole depth
		should be insert length ("oil") plus the clearance for a blind hole
		insertion ("ahd").
		*/
	hh = (off+st_h)-(oil+ahd);


	difference() {
		union() {
			translate([x,y,off]) cylinder(d=stud_bot,st_base);
			translate([x,y,st_base+off]) cylinder(d1=stud_bot,d2=stud_top,cham);
		}
		
		#union() {
			translate([x,y,hh]) cylinder(d=ophs,oil+ahd);
			translate([x,y,st_h-th+off]) cylinder(d1=ophs,d2=oshd,th);
		}			
	}	
}

//M2.5 "Long" Heat-set insert
module m25_long_stud(x,y,sh,off=0) 
	{	
	maxid = 4.37;       				//Maximum Insert Diameter
	ted = 3.66;         				//Tapered End Diameter
	oil = 5.56;         				//Overal Insert Length
	ophs = 3.58;        				//Optimum Pilot Hole Size
	oshd = 4.04;        				//Optimum Surface Hole Diameter
	rmwt = maxid*0.53;  				//Recommended Min Wall Thickness
	ahd = 0.76;         				//Added Hole Depth for Blind Holes
	th = 1.60;          				//Height of 8° Taper
	cham = 0.5;							//Chamfer at top of stud
	stud_top = maxid+(rmwt*2); 			//Top of Stud Diameter
	stud_bot = stud_top+(cham*2); 		//Bottom of Stud Diameter
		/*
		The following line assures that the overall stud height is AT LEAST
		the length of the heat-set insert PLUS a clearance for blind holes.
		You can request a height TALLER than the minimum, just not less.	
		*/
	st_h = sh > (oil+ahd) ? sh : oil+ahd;
	st_base = st_h-cham;		//Height of Stud to chamfer
		/*
		The following is a convoluted variable that keeps the hole from going
		unnecessarily deep if using a stud height is taller than the minimum 
		height. "off" is the offset and "st_h" is the total length of
		the stud. Adding those two variables sets that total height above zero.
		Now, we want to know where to set the base of the hole; the hole depth
		should be insert length ("oil") plus the clearance for a blind hole
		insertion ("ahd").
		*/
	hh = (off+st_h)-(oil+ahd);


	difference() {
		union() {
			translate([x,y,off]) cylinder(d=stud_bot,st_base);
			translate([x,y,st_base+off]) cylinder(d1=stud_bot,d2=stud_top,cham);
		}
		
		#union() {
			translate([x,y,hh]) cylinder(d=ophs,oil+ahd);
			translate([x,y,st_h-th+off]) cylinder(d1=ophs,d2=oshd,th);
		}			
	}	
}



//M3 "Short" Heat-set insert
module m3_short_stud(x,y,sh,off=0) 
	{	

	maxid = 5.59;       				//Maximum Insert Diameter
	ted = 5.16;         				//Tapered End Diameter
	oil = 3.81;         				//Overal Insert Length
	ophs = 5.05;        				//Optimum Pilot Hole Size
	oshd = 5.23;        				//Optimum Surface Hole Diameter
	rmwt = maxid*0.53;  				//Recommended Min Wall Thickness
	ahd = 0.76;         				//Added Hole Depth for Blind Holes
	th = 0.64;          				//Height of 8° Taper
	cham = 0.5;							//Chamfer at top of stud
	stud_top = maxid+(rmwt*2); 			//Top of Stud Diameter
	stud_bot = stud_top+(cham*2); 		//Bottom of Stud Diameter
		/*
		The following line assures that the overall stud height is AT LEAST
		the length of the heat-set insert PLUS a clearance for blind holes.
		You can request a height TALLER than the minimum, just not less.	
		*/
	st_h = sh > (oil+ahd) ? sh : oil+ahd;
	st_base = st_h-cham;		//Height of Stud to chamfer
		/*
		The following is a convoluted variable that keeps the hole from going
		unnecessarily deep if using a stud height is taller than the minimum 
		height. "off" is the offset and "st_h" is the total length of
		the stud. Adding those two variables sets that total height above zero.
		Now, we want to know where to set the base of the hole; the hole depth
		should be insert length ("oil") plus the clearance for a blind hole
		insertion ("ahd").
		*/
	hh = (off+st_h)-(oil+ahd);


	difference() {
		union() {
			translate([x,y,off]) cylinder(d=stud_bot,st_base);
			translate([x,y,st_base+off]) cylinder(d1=stud_bot,d2=stud_top,cham);
		}
		
		#union() {
			translate([x,y,hh]) cylinder(d=ophs,oil+ahd);
			translate([x,y,st_h-th+off]) cylinder(d1=ophs,d2=oshd,th);
		}			
	}	
}


//M3 "Long" Heat-set insert
module m3_long_stud(x,y,sh,off=0) {	

	maxid = 5.59;       				//Maximum Insert Diameter
	ted = 4.83;         				//Tapered End Diameter
	oil = 6.35;         				//Overal Insert Length
	ophs = 4.69;        				//Optimum Pilot Hole Size
	oshd = 5.23;        				//Optimum Surface Hole Diameter
	rmwt = maxid*0.53;  				//Recommended Min Wall Thickness
	ahd = 0.76;         				//Added Hole Depth for Blind Holes
	th = 1.92;          				//Height of 8° Taper
	cham = 0.5;							//Chamfer at top of stud
	stud_top = maxid+(rmwt*2); 			//Top of Stud Diameter
	stud_bot = stud_top+(cham*2); 		//Bottom of Stud Diameter
		/*
		The following line assures that the overall stud height is AT LEAST
		the length of the heat-set insert PLUS a clearance for blind holes.
		You can request a height TALLER than the minimum, just not less.	
		*/
	st_h = sh > (oil+ahd) ? sh : oil+ahd;
	st_base = st_h-cham;		//Height of Stud to chamfer
		/*
		The following is a convoluted variable that keeps the hole from going
		unnecessarily deep if using a stud height is taller than the minimum 
		height. "off" is the offset and "st_h" is the total length of
		the stud. Adding those two variables sets that total height above zero.
		Now, we want to know where to set the base of the hole; the hole depth
		should be insert length ("oil") plus the clearance for a blind hole
		insertion ("ahd").
		*/
	hh = (off+st_h)-(oil+ahd);


	difference() {
		union() {
			translate([x,y,off]) cylinder(d=stud_bot,st_base);
			translate([x,y,st_base+off]) cylinder(d1=stud_bot,d2=stud_top,cham);
		}
		
		#union() {
			translate([x,y,hh]) cylinder(d=ophs,oil+ahd);
			translate([x,y,st_h-th+off]) cylinder(d1=ophs,d2=oshd,th);
		}			
	}	
}

//M4 Heat-set insert
module m4_stud(x,y,sh,off=0) {	

	maxid = 6.35;       				//Maximum Insert Diameter
	ted = 5.38;         				//Tapered End Diameter
	oil = 7.94;         				//Overal Insert Length
	ophs = 5.28;        				//Optimum Pilot Hole Size
	oshd = 5.94;        				//Optimum Surface Hole Diameter
	rmwt = maxid*0.53;  				//Recommended Min Wall Thickness
	ahd = 0.76;         				//Added Hole Depth for Blind Holes
	th = 2.35;          				//Height of 8° Taper
	cham = 0.5;							//Chamfer at top of stud
	stud_top = maxid+(rmwt*2); 			//Top of Stud Diameter
	stud_bot = stud_top+(cham*2); 		//Bottom of Stud Diameter
		/*
		The following line assures that the overall stud height is AT LEAST
		the length of the heat-set insert PLUS a clearance for blind holes.
		You can request a height TALLER than the minimum, just not less.	
		*/
	st_h = sh > (oil+ahd) ? sh : oil+ahd;
	st_base = st_h-cham;		//Height of Stud to chamfer
		/*
		The following is a convoluted variable that keeps the hole from going
		unnecessarily deep if using a stud height is taller than the minimum 
		height. "off" is the offset and "st_h" is the total length of
		the stud. Adding those two variables sets that total height above zero.
		Now, we want to know where to set the base of the hole; the hole depth
		should be insert length ("oil") plus the clearance for a blind hole
		insertion ("ahd").
		*/
	hh = (off+st_h)-(oil+ahd);


	difference() {
		union() {
			translate([x,y,off]) cylinder(d=stud_bot,st_base);
			translate([x,y,st_base+off]) cylinder(d1=stud_bot,d2=stud_top,cham);
		}
		
		#union() {
			translate([x,y,hh]) cylinder(d=ophs,oil+ahd);
			translate([x,y,st_h-th+off]) cylinder(d1=ophs,d2=oshd,th);
		}			
	}	
}

//M5 Heat-set insert
module m5_stud(x,y,sh,off=0) {	

	maxid = 8.33;       				//Maximum Insert Diameter
	ted = 7.82;         				//Tapered End Diameter
	oil = 6.75;         				//Overal Insert Length
	ophs = 7.70;        				//Optimum Pilot Hole Size
	oshd = 8.00;        				//Optimum Surface Hole Diameter
	rmwt = maxid*0.53;  				//Recommended Min Wall Thickness
	ahd = 0.76;         				//Added Hole Depth for Blind Holes
	th = 1.07;          				//Height of 8° Taper
	cham = 0.5;							//Chamfer at top of stud
	stud_top = maxid+(rmwt*2); 			//Top of Stud Diameter
	stud_bot = stud_top+(cham*2); 		//Bottom of Stud Diameter
		/*
		The following line assures that the overall stud height is AT LEAST
		the length of the heat-set insert PLUS a clearance for blind holes.
		You can request a height TALLER than the minimum, just not less.	
		*/
	st_h = sh > (oil+ahd) ? sh : oil+ahd;
	st_base = st_h-cham;		//Height of Stud to chamfer
		/*
		The following is a convoluted variable that keeps the hole from going
		unnecessarily deep if using a stud height is taller than the minimum 
		height. "off" is the offset and "st_h" is the total length of
		the stud. Adding those two variables sets that total height above zero.
		Now, we want to know where to set the base of the hole; the hole depth
		should be insert length ("oil") plus the clearance for a blind hole
		insertion ("ahd").
		*/
	hh = (off+st_h)-(oil+ahd);


	difference() {
		union() {
			translate([x,y,off]) cylinder(d=stud_bot,st_base);
			translate([x,y,st_base+off]) cylinder(d1=stud_bot,d2=stud_top,cham);
		}
		
		#union() {
			translate([x,y,hh]) cylinder(d=ophs,oil+ahd);
			translate([x,y,st_h-th+off]) cylinder(d1=ophs,d2=oshd,th);
		}			
	}	
}