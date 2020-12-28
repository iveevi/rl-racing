extends RayCast2D

var collision_point = position;

func _ready():
	pass

func _process(delta):
	update();
	
	if is_colliding():
		collision_point = get_collision_point();
		get_parent().left = global_position.distance_to(collision_point);
	else:
		get_parent().left = 1e+10;
