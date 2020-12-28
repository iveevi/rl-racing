extends Line2D

var parent = null;

var go = false;

func _process(delta):
	if !go:
		return;
	
	global_rotation = 0;
	
	if get_point_count() >= 2000:
		remove_point(0);
	
	# add_point(0.05 * (parent.global_position - global_position));
