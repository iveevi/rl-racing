extends Node2D

var cc = -1;
var mode = 0;
var nmodes = 2;

onready var env = get_node("environments");
onready var net = get_node("network");

onready var top = get_node("track").get_node("track-view");
onready var fps = get_node("car").get_node("FP_Camera");

func _ready():
	top.current = true;

var tab_pressed = false;
var cntrl_pressed = false;

func _process(_delta):
	if mode == 0:
		top.current = true;
	else:
		fps.current = true;
	
	if Input.is_key_pressed(KEY_TAB):
		if !tab_pressed:
			tab_pressed = true;
			
			mode = (mode + 1) % nmodes;
	else:
		tab_pressed = false;
	
	if Input.is_key_pressed(KEY_ESCAPE):
		mode = 0;
