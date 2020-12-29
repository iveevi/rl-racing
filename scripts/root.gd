extends Node2D

var cc = -1;

onready var env = get_node("environments");

onready var top = get_node("cells");

func _ready():
	get_node("cells").current = true;

var tab_pressed = false;
var cntrl_pressed = false;

func _process(_delta):
	if Input.is_key_pressed(KEY_TAB):
		if !tab_pressed:
			cc = (cc + 1) % env.get_child_count();
			
			tab_pressed = true;
	else:
		tab_pressed = false;
	
	if Input.is_key_pressed(KEY_ESCAPE):
		top.current = true;
		
		cc = -1;
		
	if cc != -1:
		env.get_child(cc).get_node("track-view").current = true;
	
	"""
	if Input.is_key_pressed(KEY_CONTROL):
		if Input.is_key_pressed(KEY_S) and cntrl_pressed == false:
			get_node(network).save();
		
			cntrl_pressed = true;
	else:
		cntrl_pressed = false;"""
	
