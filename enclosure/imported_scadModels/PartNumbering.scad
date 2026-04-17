module test_part()
{cube([40,15,2]);
    
    }
    
    
    
    
year= 26;
week = 14;
day = 1;
PartName = "Tshell";
partVersion = "v1.0";


partNunmber = str(PartName, "-",partVersion);

batch =str("B", formate_number(year),formate_number(week),day);





// debossed
    /*
difference(){
test_part();
translate([2,8.5,2-0.6])numbering(0.6+0.01,4,PartNumber =partNunmber );
    translate([2,2,2-0.6])numbering(0.6+0.01,4,PartNumber =batch);
}*/
// embossed

test_part();
translate([2,8.5,2-0.1])numbering(0.8+0.1,4,"TSHELL","v1.0","26141");
    



module numbering(t, s, PartName, Partversion, date)
    {
        linear_extrude(t)
text(str("K_",PartName,"-",Partversion),size = s,font = "Arial Rounded MT Bold:style=Bold");
        
        translate([0,-6.5,0])linear_extrude(t)
text(str("B",date),size = s,font = "Arial Rounded MT Bold:style=Bold");
        }

    
function formate_number(num) = 
    (
        
        (num < 10)?str("0",num): str(num)
        
         );