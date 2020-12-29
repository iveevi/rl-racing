extends RayCast2D

var collision_point = position;

var parent = null;

func _ready():
	parent = get_parent();
	
	pass

func _process(delta):
	var value = 10;
	
	if is_colliding():
		collision_point = get_collision_point();
		
		value = log(global_position.distance_to(collision_point));
	
#	if name == "fright":
#		parent.fright = value;
#	if name == "fleft":
#		parent.fleft = value;
#	if name == "bright":
#		parent.bright = value;
#	if name == "bleft":
#		parent.bleft = value;
#	if name == "front":
#		parent.front = value;
#	if name == "back":
#		parent.back = value;
#	if name == "right":
#		parent.right = value;
#	if name == "left":
#		parent.left = value;
