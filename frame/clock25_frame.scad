/*
    clock25 frame
    
    by atctwo
    version 1
*/

use <MCAD/boxes.scad>

hub75_w = 192;
hub75_h = 192;
mh_corner_dx = 12.5;
mh_corner_dy = 10;
mh_middle_dx = 10;
mh_middle_dy = 10;
mh_d = 3.5;
cutout_radius = 20;
frame_width = 15.5;
frame_thickness = 10;

board_w = 114.3;
board_h = 71.755;
board_mh_dxy = 5.08;        // distance from middle of hole to edge of board
board_mh_d = 4.2;
board_mh_h = 5.4;
board_shell_padding = 1;
frame_board_spacing = 15;
thingy_r = 5;
inside_thingy_r = 15;
long_hole_width = 5;
long_hole_height = 3;
show_board = false;

$fn = 100;

// bit to hold board

arm_height = (hub75_h / 2) - (frame_width / 2);
block_w = board_w + (board_shell_padding * 2);
block_d = board_h + (board_shell_padding * 2) + frame_board_spacing;
block_middle_x =  (block_w / 2) + (frame_width/2);
block_middle_y = -(block_d / 2) + (frame_width/2) - frame_board_spacing;

translate([(hub75_w/2) - (board_w/2) - (frame_width/2), 0, 0])
{
    difference() {
        
        // main cube body
        union() {
                   
            // feet
            translate([0, -block_d, 0])
            cube([frame_width, block_d, frame_width]);
            
            translate([block_w, -block_d, 0])
            cube([frame_width, block_d, frame_width]);
            
            translate([0, -block_d, 0])
            cube([block_w, frame_width, frame_width]);
            
            // arms
            translate([0, -block_d, arm_height])
            cube([frame_width, block_d, frame_width]);
            
            translate([block_w, -block_d, arm_height])
            cube([frame_width, block_d, frame_width]);
            
            translate([0, -block_d, arm_height])
            cube([block_w, frame_width, frame_width]);
            
            // legs
            translate([0, -block_d, 0])
            cube([frame_width, frame_width, arm_height]);
            
            translate([block_w, -block_d, 0])
            cube([frame_width, frame_width, arm_height]);
            
            // big rounded bit in the middle
            difference() {
                translate([frame_width, -block_d+frame_width, arm_height])
                cube([
                    block_w - frame_width,
                    block_d - frame_width,
                    frame_width
                ]);
                    
                translate([frame_width, -block_d+frame_width, arm_height-0.1])
                roundedCube([
                        block_w - frame_width,
                        block_d - frame_width - frame_board_spacing,
                        frame_width+1
                    ], inside_thingy_r, true);
            }
        }
        
        // mounting holes
        
        // bottom left
        translate([
            block_middle_x - (board_w/2) + board_mh_dxy, 
            block_middle_y - (board_h/2) + board_mh_dxy + 8, 
            arm_height+frame_width-board_mh_h+0.001
        ]) cylinder(d=board_mh_d, h=board_mh_h+1.5);
        
        // top left
        translate([
            block_middle_x - (board_w/2) + board_mh_dxy - 0.4, 
            block_middle_y + (board_h/2) + board_mh_dxy - 1.7, 
            arm_height+frame_width-board_mh_h+0.001
        ]) cylinder(d=board_mh_d, h=board_mh_h+1.5);
        
        // bottom right
        translate([
            block_middle_x + (board_w/2) - board_mh_dxy, 
            block_middle_y - (board_h/2) + board_mh_dxy + 8, 
            arm_height+frame_width-board_mh_h+0.001
        ]) cylinder(d=board_mh_d, h=board_mh_h+1.5);
        
        // top right
        translate([
            block_middle_x + (board_w/2) - board_mh_dxy, 
            block_middle_y + (board_h/2) + board_mh_dxy - 1.7, 
            arm_height+frame_width-board_mh_h+0.001
        ]) cylinder(d=board_mh_d, h=board_mh_h+1.5);
        
        // long holes
        
        // west
        translate([
            block_middle_x - (board_w/2) + 1, 
            block_middle_y - (board_h/2) + (board_mh_dxy * 3.1), 
            arm_height+frame_width-long_hole_height
        ]) cube([board_w, board_h - (board_mh_dxy*3.1), long_hole_height+1]);
        
        // south
        translate([
            block_middle_x - (board_w/2) + long_hole_width*1.8, 
            block_middle_y - (board_h/2) + (board_mh_dxy * 3.1) - (long_hole_width*1.5), 
            arm_height+frame_width-long_hole_height
        ]) cube([board_w - (board_mh_dxy*3.5), board_h, long_hole_height+1]);
    }
    
    if (show_board) {
        color("darkcyan", 0.3)
        translate([
            (89.9 + board_w) + (frame_width/2),
            (-64.77-board_h) - (frame_width/2), 
            arm_height+frame_width
        ]) rotate([0, 0, 180])
        import("clock25_board.stl");
    }
}

/*
cube_w = board_w + (board_shell_padding * 2);
cube_d = board_h + (board_shell_padding * 2) + frame_board_spacing;
cube_h = (hub75_h / 2) + (frame_width / 2);
    
translate([(hub75_w/2) - (board_w/2), -cube_d, 0])
difference() {
    
    union() {
        
        // main cube
        roundedCube([cube_w, cube_d, cube_h], thingy_r);
        
        // flat bit at top
        translate([0, 0, cube_h - frame_width])
        roundedCube([cube_w, cube_d + frame_thickness, frame_width], thingy_r, true);
    }
    
    // holes
    translate([-2.5, frame_width, frame_width])
    roundedCube([cube_w+5, cube_d - (frame_width * 2), cube_h - (frame_width * 2)], thingy_r);
    
    translate([frame_width, -2.5, frame_width])
    roundedCube([cube_w - (frame_width * 2), cube_d + 5, cube_h - (frame_width * 2)], thingy_r);
    
    translate([frame_width, frame_width, -2.5])
    roundedCube([cube_w - (frame_width * 2), cube_d - (frame_width * 2), cube_h + 5], thingy_r);
    
}
*/

// frame to hold hub75
difference() {
    
    // main frame body
    union() {
        translate([0, frame_thickness, 0])
        rotate([90, 0, 0])
        roundedCube([hub75_w, hub75_h, frame_thickness], 10, true);
        
        // bottom left
        cube([hub75_w/2, frame_thickness, hub75_h/2]);
        
        // bottom right
        translate([hub75_w - (hub75_w/2), 0, 0])
        cube([hub75_w/2, frame_thickness, hub75_h/2]);
        
        // top left
        translate([0, 0, hub75_h - (hub75_h/2)])
        cube([hub75_w/2, frame_thickness, hub75_h/2]);
    }
    
    // mounting hole holes
    
    // bottom left
    translate([mh_corner_dx, -0.1, mh_corner_dy])
    hole(mh_d, frame_thickness+1);
    
    // bottom right
    translate([hub75_w - mh_corner_dx, -0.1, mh_corner_dy])
    hole(mh_d, frame_thickness+1);
    
    // top left
    translate([mh_corner_dx, -0.1, hub75_h - mh_corner_dy])
    hole(mh_d, frame_thickness+1);
    
    // top right
    translate([hub75_w - mh_corner_dx - 1.5, -0.1, hub75_h - mh_corner_dy])
    hole(mh_d, frame_thickness+1);
    
    // middle left
    translate([mh_middle_dx, -0.1, (hub75_h/2)])
    hole(mh_d, frame_thickness+1);
    
    // middle right
    translate([hub75_w - mh_middle_dx, -0.1, (hub75_h/2)])
    hole(mh_d, frame_thickness+1);
    
    // big cutouts
    cutout_w = hub75_w - (frame_width * 2);
    cutout_h = (hub75_h / 2) - (frame_width * 1.5);
    
    translate([frame_width, -0.5, frame_width])
    roundedCube([
        cutout_w,
        frame_thickness + 1,
        cutout_h
    ], cutout_radius);
    
    translate([frame_width, -0.5, (frame_width*2) + cutout_h])
    roundedCube([
        cutout_w,
        frame_thickness + 1,
        cutout_h
    ], cutout_radius);
}

// hole
module hole(d, h) {
    rotate([270, 0 ,0])
    cylinder(d=d, h=h);
}