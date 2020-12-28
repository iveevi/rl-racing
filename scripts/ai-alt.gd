extends KinematicBody2D

# Spawns
var spawn;

# Speed constants
var acc_const = 0.3;
var drag_const = 0.99;
var brake_const = 0.95;

var max_velocity = 20;
var min_velocity = 5;

# Global and Saved State Variables
var velocity = 0;
var accel = 0;
var steer = 0;
var dist = 0;
var reward = 0;
var p_accumul = 1;
var accumul = 1;
var old = 0;
var index = 0;
var _O = [];
var state = [];
var old_state = [];

# Sensors
var right = 0;
var left = 0;
var front = 0;
var back = 0;
var fright = 0;
var fleft = 0;
var bright = 0;
var bleft = 0;

# Parent
var parent = null;

# Calculations
func calculate(X):
	old_state = X;
	return choose(parent.calculate(X));

# Selecting the discrete option
#	0 is brake, {1, -1} for {brake, gas}
#	1 is steer, {1, -1} for {right, left}
func choose(O):
	# 0 is [1, 1]
	# 1 is [1, -1]
	# 2 is [-1, 1]
	# 3 is [-1, -1]
	
	_O = O;
	
	if parent.tau == 0:
		var x = 0;
		
		for i in range(1, 4):
			if O[x] < O[i]:
				x = i;
		
		old = O[x];
		index = x;
		
		if x == 0:
			return [1, 1];
		elif x == 1:
			return [1, -1];
		elif x == 2:
			return [-1, 1];
		elif x == 3:
			return [-1, -1];
		else:
			return [0, 0];
	
	var eO = [];
	var sum = 0;
	
	for i in O:
		sum += exp(i/parent.tau);
	
	for i in O:
		eO.append(exp(i/parent.tau)/sum);
	
	var dO = [0];
	
	sum = 0;
	for i in eO:
		sum += i;
		
		dO.append(sum);
	
	var rnd = parent.rng.randf_range(0, 1);
	
	var x = 0;
	for i in range(0, 4):
		if rnd > dO[i] and rnd < dO[i + 1]:
			x = i;
			
			break;
	
	old = O[x];
	index = x;
	
	if x == 0:
		return [1, 1];
	elif x == 1:
		return [1, -1];
	elif x == 2:
		return [-1, 1];
	elif x == 3:
		return [-1, -1];
	else:
		return [0, 0];

# Setup (Ready)
var ppos = null;

var id = 0;

func _ready():
	parent = get_parent();
	
	id = parent.rng.randi_range(0, get_node(spawn).get_child_count() - 1);
	
	global_position = get_node(spawn).get_child(id).global_position;
	
	rotation_degrees = 90 + get_node(spawn).get_child(id).rotation_degrees;
	
	ppos = position;

# Prevent Early Learning
var learn = false;

# Process (Run)
func _process(delta):
	state = [right, left, front, back,
		fright, fleft, bright, bleft];
	
	# Q-Learning
	if learn:
		parent.Q_learn(old, _O, accumul - p_accumul, state, old_state, index);
	
	# Turn on Learning
	learn = true;
	
	var vecs = calculate(state);
	
	# Braking and Acceleration
	if vecs[0] > 0:
		accel = acc_const;
		
		velocity *= brake_const;
	else:
		accel = 0;
		
		velocity += acc_const;
	
	velocity = min(velocity, max_velocity);
	velocity = max(velocity, min_velocity);
	
	# Applying discrete steer
	if vecs[1] > 0:
		rotation += velocity * 0.0025;
	else:
		rotation -= velocity * 0.0025;
	
	# Applying Action
	var col = move_and_collide(Vector2(cos(rotation),
		sin(rotation)) * velocity);
	
	# Calculating the Reward
	p_accumul = accumul;
	
	accumul = log(exp(accumul) + (position - ppos).length());
	
	ppos = position;
	
	# Collision resetting
	if col:
		reset();
		
		return;

# Reseting the Agent
func reset():
	# Moving the Agent
	id = parent.rng.randi_range(0, get_node(spawn).get_child_count() - 1);
	
	global_position = get_node(spawn).get_child(id).global_position;
	rotation_degrees = 90 + get_node(spawn).get_child(id).rotation_degrees;
	
	# Reset Variables
	velocity = 0;
	accumul = 0;
	
	learn = false;
	
	ppos = position;
	
	pass;
