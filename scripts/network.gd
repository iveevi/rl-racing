extends Node2D

# AI
onready var ai_prefab = load("res://scenes/agent.tscn");

var rounds = 0;

func _ready():
	var env = get_parent().get_node("environments");
	
	for i in range(0, env.get_child_count()):
		var car = ai_prefab.instance();
		
		car.spawn = env.get_child(i).get_child(0).get_path();
		car.angle = -60;
		
		add_child(car);
