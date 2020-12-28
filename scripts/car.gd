extends KinematicBody2D

# Declare member variables here. Examples:
var acc_pressed = false;
var rev_pressed = false;
var acc_const = 0.3;
var drag_const = 0.99;
var brake_const = 0.95;

var rest_velocity = 0.001;

var max_velocity = 20;
var min_velocity = -5;

var velocity = 0;

var game = 0;

# Called when the node enters the scene tree for the first time.
func _ready():
	get_child(2).current = true;
	pass

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	
	var vel = Vector2();
	var col;
	
	# Acceleration
	if Input.is_key_pressed(KEY_UP):
		velocity = min(velocity, max_velocity);
		
		vel.y = velocity * sin(rotation);
		vel.x = velocity * cos(rotation);
		col = move_and_collide(vel);
		
		velocity += acc_const;
	elif Input.is_key_pressed(KEY_SPACE):
		vel.y = velocity * sin(rotation);
		vel.x = velocity * cos(rotation);
		col = move_and_collide(vel);
		
		velocity *= brake_const;
	elif Input.is_key_pressed(KEY_DOWN):
		velocity = max(velocity, min_velocity);
		
		vel.y = velocity * sin(rotation);
		vel.x = velocity * cos(rotation);
		col = move_and_collide(vel);
		
		velocity -= acc_const;
	else:
		vel.y = velocity * sin(rotation);
		vel.x = velocity * cos(rotation);
		col = move_and_collide(vel);
		
		velocity *= drag_const;
	
	# Steering
	if Input.is_key_pressed(KEY_LEFT):
		rotation -= velocity * 0.0025;
	elif Input.is_key_pressed(KEY_RIGHT):
		rotation += velocity * 0.0025;
		
	if col:
		reset();

func reset():
	velocity = 0;
	
	position = get_parent().get_node("spawn").position;
	
	rotation = 0;
	
	pass;
