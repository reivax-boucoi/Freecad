//Dimensions du tambour
tambour_length = 30;
tambour_width = 20;
tambour_thickness = 0.2;

//Ecartement des notes
note_tempo = 1;
note_spacing = 1;
note_startVOffset = 1;
note_startTOffset = 1;

//Dimensions cube note
note_width = 0.5;
note_length = 0.5;
note_height = 0.5;

//Tambour
color([1,0,0])
translate([0,0,-tambour_thickness]) 
cube([tambour_length,tambour_width,tambour_thickness]);

//Note
module drawNote(time,note) {
    translate([note_startTOffset+time*note_tempo,note_startVOffset+note*note_spacing,0]) 
    cube([note_length,note_width,note_height]);
}


//Placement des notes (temps, hauteur)
drawNote(0,0);
drawNote(0,1);
drawNote(1,2);
drawNote(1,3);
drawNote(2,5);
drawNote(3,2);