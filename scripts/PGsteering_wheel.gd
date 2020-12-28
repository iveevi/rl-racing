extends Sprite

func _process(delta):
	rotation_degrees = get_parent().get_parent().get_child(1).rotation * 140;
