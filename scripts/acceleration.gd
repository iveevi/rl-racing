extends ProgressBar

var car = null;

func _ready():
	car = get_parent().get_parent().get_node("car");

func _process(delta):
	set_value(100 * car.accel/car.acc_const);
