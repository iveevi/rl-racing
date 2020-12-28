extends ProgressBar

var car = null;

func _ready():
	car = get_node("../../network/car");

func _process(delta):
	set_value(100 * car.velocity/car.max_velocity);
