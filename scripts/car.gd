extends KinematicBody2D

var opos = null;

onready var left = get_node("wheels/left");
onready var right = get_node("wheels/right");

# Meters per second
var velocity = 0;
var max_velocity = 1;

# Degrees
var angle = 0;
var max_angle = 30;

func cap():
	if angle > max_angle:
		angle = max_angle;
	elif angle < -max_angle:
		angle = -max_angle;
	
	if velocity > max_velocity:
		velocity = max_velocity;
	elif angle < -max_angle:
		angle = -max_angle;

func vel():
	return velocity * Vector2(cos(rotation), sin(rotation));

func _process(delta):
	if Input.is_key_pressed(KEY_UP):
		velocity += 0.001;
	if Input.is_key_pressed(KEY_DOWN):
		velocity -= 0.001;
	
	if Input.is_key_pressed(KEY_LEFT):
		angle -= 0.03;
	elif Input.is_key_pressed(KEY_RIGHT):
		angle += 0.03;
	
	cap();
	
	left.rotation_degrees = angle;
	right.rotation_degrees = angle;
	
	move_and_collide(vel());

func reset():
	position = opos;
	
	velocity = 0;
	rotation = 0;
