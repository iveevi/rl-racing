extends KinematicBody2D

var spawn;

var angle;

# Speed constants
var acc_const = 0.3;
var drag_const = 0.99;
var brake_const = 0.95;

# Velocities, Throttles, Steers and Brakes
var max_velocity = 20;
#var min_velocity = -5;
var min_velocity = 5;
var velocity = 0;
var accel = 0;
var steer = 0;
var brakes = 0;
var score = 0;

# Sensors
var right = 0;
var left = 0;
var front = 0;
var back = 0;
var fright = 0;
var fleft = 0;
var bright = 0;
var bleft = 0;

# Time Control
var dlt = 0;
var time = 0;

# Buffers, Distances
var buff = [];
var dists = [];
var states = [];
var outs = [];
var dist = 0;
var Rt = 0;
var sig = 0;

var index = -1;

var buff_size = 180;

var parent = null;

func calculate(X):
	return choose(parent.calculate(X));

# Temperature constant

# Exploit
#var tau = 0;

# Exploit-Explore
# var tau = 2;

# Explore
#var tau = 1000;

# Selecting the discrete option
#	0 is brake, {1, -1} for {brake, gas}
#	1 is steer, {1, -1} for {right, left}
func choose(O):
	# 0 is [1, 1]
	# 1 is [1, -1]
	# 2 is [-1, 1]
	# 3 is [-1, -1]
	
#	print("------------------------------");
	outs.append(O);
	
	if parent.tau == 0:
		var x = 0;
		
		for i in range(1, 4):
			if O[x] < O[i]:
				x = i;
		
		if x == 0:
			return [1, 1];
		elif x == 1:
			return [1, -1];
		elif x == 2:
			return [-1, 1];
		elif x == 3:
			return [-1, -1];
		# Safety
		else:
			return [0, 0];
	
	var eO = [];
	var sum = 0;
	
	for i in O:
		sum += exp(i/parent.tau);
	
	for i in O:
		eO.append(exp(i/parent.tau)/sum);
	
#	print("O: " + str(O));
#	print("eO: " + str(eO));
	
	var dO = [0];
	
	sum = 0;
	for i in eO:
		sum += i;
		
		dO.append(sum);
	
#	print("dO: " + str(dO));
	
	var rnd = parent.rng.randf_range(0, 1);
	
#	print("\trnd: " + str(rnd));
	
	var x = 0;
	for i in range(0, 4):
		if rnd > dO[i] and rnd < dO[i + 1]:
			x = i;
			
			break;
	
#	print("\tind: " + str(x));
	
	if x == 0:
		return [1, 1];
	elif x == 1:
		return [1, -1];
	elif x == 2:
		return [-1, 1];
	elif x == 3:
		return [-1, -1];
	# Safety
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

# Process (Run)
func _process(delta):
	time += delta;
	dlt = delta;
	
	# Buffer/History
	var vecs = calculate([right, left, front, back,
		fright, fleft, bright, bleft]);
	
#	if buff.size() > buff_size:
#		buff.remove(0);
	
	buff.append(vecs);
	
#	if states.size() > buff_size:
#		states.remove(0);
	
	states.append([right, left, front, back, fright, fleft, bright, bleft]);
	
	# Braking and Acceleration
	brakes = vecs[0];
	
	if vecs[0] > 0:
		accel = acc_const;
		
		velocity *= brake_const;
	else:
		accel = 0;
		
		velocity += acc_const;
	
	velocity = min(velocity, max_velocity);
	velocity = max(velocity, min_velocity);
	
	# Applying discrete steer
	steer = vecs[1];
	
	if steer > 0:
		rotation += velocity * 0.0025;
	else:
		rotation -= velocity * 0.0025;
		
	var col = move_and_collide(Vector2(cos(rotation),
		sin(rotation)) * velocity);
	
	# Managing distance measures
	if index == -1:
		dists.append((position - ppos).length());
		
		dist = dists[0];
	else:
		dists.append((position - ppos).length());
		
		dist = (position - ppos).length();
	
	index += 1;
	
	ppos = position;
	
	# Collision resetting
	if col or (index > buff_size && dist < 1):
		reset();
		
		return;

# Reseting the Agent
func reset():
	id = parent.rng.randi_range(0, get_node(spawn).get_child_count() - 1);
#	id = 0;
	
	global_position = get_node(spawn).get_child(id).global_position;
	rotation_degrees = 90 + get_node(spawn).get_child(id).rotation_degrees;
	
	# Reset Variables
	velocity = 0;
	index = -1;
	time = 0;
	score = 0;
	
	ppos = position;
	
	parent.Q_learn([states, buff, dists, outs]);
	
	dists.clear();
	buff.clear();
	states.clear();
	outs.clear();
	
	# Statistics Update
#	parent.rounds += 1;
	
	pass;
