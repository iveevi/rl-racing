extends ProgressBar

var car = null;

func _ready():
	car = get_parent().get_parent().get_node("player");

func _process(delta):
	set_value(100 * car.velocity/car.max_velocity);
