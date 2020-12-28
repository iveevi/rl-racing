extends Sprite

func _process(delta):
	rotation_degrees = get_parent().get_parent().get_node("network/car").steer * 60;
