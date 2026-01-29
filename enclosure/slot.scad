

w=3;
h =8;
t=1;
module slot(w,h,t,l) {
 
points = [[0,0],[0,h/2],[w,h/2],[w,(h/2)-t],[t,((h/2)-t)-(w-t)], [t,0]];
linear_extrude(height = l) union() {
    mirror(v = [0,1,0]) polygon(points = points);
    polygon(points = points);
}
 
}
slot(w,h,t,10);