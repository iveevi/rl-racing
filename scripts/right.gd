extends RayCast2D

var collision_point = position;

func _ready():
	pass

func _process(delta):
	if is_colliding():
		collision_point = get_collision_point();
		get_parent().right = global_position.distance_to(collision_point);
	else:
		get_parent().right = 1e+10;
