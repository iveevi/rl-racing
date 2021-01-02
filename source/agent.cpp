#include <agent.hpp>

double Agent::min_vel = -1;
double Agent::max_vel = 15;
double Agent::idle_vel = 0.05;

// Perhaps use delta instead
int Agent::cycle_thresh = 1000;

double Agent::k_a = 10;
double Agent::k_b = 0.997;
double Agent::k_d = 0.999;

double Agent::lambda = 0.997;

Agent::Agent()
{
	srand(clock());

	cycles = 0;
	spawn = 0;
	velocity = 0;
	rt = 0;

	eps = 1.0;
}

Agent::~Agent() {}

void Agent::step(double delta)
{
	std::pair <size_t, Vector <double>> action = get_action();

	bool done = move(action.first, delta);

	previous_state = current_state;

	current_state = get_state();

	double reward = get_reward();

	rt += reward;
	
	experience exp {
		previous_state,
		action.second,
		action.first,
		current_state,
		reward,
		done
	};

	push(exp);
}

double Agent::get_reward()
{
	if (crashed) {
		crashed = false;

		return -200;
	}

	if (rewarded) {
		rewarded = false;

		return 100;
	}

	return 0;
}

std::pair <size_t, Vector <double>> Agent::get_action()
{
	Vector <double> Q_values = model(current_state);

	size_t mx = 0;

	double rnd = distribution(generator);

	if (rnd > eps)
		mx = Q_values.imax();
	else
		mx = rand() % 6;

	return {mx, Q_values};
}

bool Agent::move(size_t mx, double delta)
{
	accelerate(mx / 3, delta);
	steer(mx % 3);

	Ref <KinematicCollision2D> ref = nullptr;

	velocity = cap(velocity, min_vel, max_vel);

	ref = move_and_collide(
			Vector2(
				velocity * cos(get_rotation()),
				velocity * sin(get_rotation())
	));

	using namespace std;
	if (ref.ptr() || cycles >= cycle_thresh) {
		rand_reset();

		crashed = true;

		return true;
	}

	rewarded = passed_gate();

	return false;
}

bool Agent::passed_gate() const
{
	for (size_t i = 0; i < gate_size; i++) {
		if (gate_list[i]->overlaps_body(this)) {
			if (!gate_passed[i]) {
				gate_passed[i] = true;
				return true;
			}
		} else {
			gate_passed[i] = false;
		}
	}

	return false;
}

//=====================================
void Agent::_init() {rt = 0;}

void Agent::rand_reset()
{
	int index = rand() % spawns;
	
	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(index));
	
	set_rotation_degrees(90 + nd->get_rotation_degrees());
	set_global_position(nd->get_global_position());

	ppos = get_global_position();

	// Method/function
	rewards[id].push(rt);
	epsilons[id].push(eps);
	episodes[id]++;

	csv << episode++ << "," << rt << std::endl;

	eps = cap(eps - 1e-3, 0.001, 1.0);

	cycles = 0;
	velocity = 0;
	rt = 0;
}

Vector <double> Agent::get_state()
{
	return Vector <double> (11,
		[&](size_t i) {
			switch (i) {
			case 0:
				return velocity;
			case 1:
				return velocity * cos(get_rotation());
			case 2:
				return velocity * sin(get_rotation());
			default:
				Vector2 other = rays[i - 3]->get_collision_point();

				return (double) get_global_position().distance_to(other);
			}
		}
	);
}

// Note, no reversing
void Agent::accelerate(size_t i, double delta)
{
	brake = false;
	idle = false;

	switch (i) {
	case 0:
		// Accelerate
		velocity += k_a * delta;
		break;
	case 1:
		// Nothing
		idle = true;
		velocity *= pow(k_d, delta);
		break;
	/* case 2: Ignore braking for now
		// Brake
		brake = true;
		velocity *= pow(k_b, delta);
		break; */
	}
}

void Agent::steer(size_t i)
{
	switch (i) {
	case 0:
		// Left
		set_rotation(get_rotation() - velocity * 0.0025);
		break;
	case 1:
		// Right
		set_rotation(get_rotation() + velocity * 0.0025);
		break;
	}
}

void Agent::_ready()
{
	// Reset
	spawns = get_node(spawn)->get_child_count();

	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(0));

	set_rotation_degrees(90 + angle);
	set_global_position(nd->get_global_position());

	ppos = get_global_position();

	rays = new RayCast2D *[8];
	for (size_t i = 0; i < 8; i++)
		rays[i] = Object::cast_to <RayCast2D> (get_child(i + 2));

	// Gates
	Node *gate_node = get_node(gates);

	gate_size = gate_node->get_child_count();

	gate_passed = new bool [gate_size];
	gate_list = new Area2D *[gate_size];

	for (size_t i = 0; i < gate_size; i++) {
		gate_passed[i] = false;
		gate_list[i] = Object::cast_to <Area2D> (gate_node->get_child(i));
	}
	
	//==============================================================
	id = size++;
	agents.push_back(this);

	// Put this method into the master _ready function
	rewards.push_back(std::queue <double> ());
	epsilons.push_back(std::queue <double> ());
	episodes.push_back(1);
	flushed.push_back(false);

	std::string fpath = "results/" + dir + "/agent_" + std::to_string(id);
	csv.open(fpath);
	csv << "episode,reward" << std::endl;

	current_state = get_state();
	previous_state = get_state();

	episode = 1;

	crashed = false;
	rewarded = false;
}
