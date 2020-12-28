extends Node

export (int) var rows;
export (int) var cols;

onready var track = load("res://scenes/track.tscn");

func _ready():
	var xoff = 0;
	for i in range(0, rows):
		
		var yoff = 0;
		for j in range(0, cols):
			var tr = track.instance();
			
			tr.position.x = xoff;
			tr.position.y = yoff;
			
			tr.get_node("stats").text = "\n\n(" + str(i) + ", " + str(j) + ")";
			
			add_child(tr);
			
			yoff += 7000;
		
		xoff += 11000;
	
	var cx = xoff + 10240;
	var cy = 7000 * cols + 6000;
	
	var z = max(10 * cx/10240, 10 * cy/6000);
	
	get_parent().get_node("cells").zoom = Vector2(z, z);
	get_parent().get_node("cells").position = Vector2(xoff/2 - 5000, 3500 * cols - 5000);
