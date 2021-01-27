#include <agent.hpp>

float Agent::min_vel = -1;
float Agent::max_vel = 1;
float Agent::idle_vel = 0.05;

// Perhaps use delta instead
int Agent::cycle_thresh = 1000;

float Agent::k_a = 1;
float Agent::k_b = 0.997;
float Agent::k_d = 0.999;

// Put lambda into JSON file
float Agent::lambda = 0.955;
float Agent::deps = 0.0015;

Agent::Agent()
{
	srand(clock());

	cycles = 0;
	spawn = 0;
	velocity = 0;
	rt = 0;

	// Exploration/exploitation values
	eps = 1;
}

Agent::~Agent() {}

void Agent::best_step(float delta)
{
	size_t action = best_action();

	bool done = move(action, delta);

	current_state = get_state();

	float reward = get_reward();

	rt += reward;
}

void Agent::step(float delta)
{
	// std::pair <size_t, Vector <float>> action = get_action();
	size_t a1 = get_action();

	// bool done = move(action.first, delta);
	bool done = move(a1, delta);

	previous_state = current_state;

	current_state = get_state();

	float reward = get_reward();

	rt += reward;

	double err = fabs(reward + lambda * target(current_state)[a1] - Q_value);

	using namespace std;
	// cout << "err =  " << err << endl;

	total += err;
	// cout << "total = " << total << endl;
	
	experience exp {
		previous_state,
		a1,
		current_state,
		reward,
		err,
		done
	};

	frames++;

	// cout << "eps = " << eps << endl;

	push(exp);

	// cout << "Finished step!" << endl;
	std::string s = "Reliance: " + std::to_string(eps);

	rel->set_text(s.c_str());

	if (frames % 1000 == 0)
		eps = cap(eps - deps, 0.1f, 1.0f);
}

float Agent::get_reward()
{
	if (crashed) {
		crashed = false;

		return -10;
	}

	if (rewarded) {
		rewarded = false;

		return 5;
	}

	return 0;
}

size_t Agent::get_action()
{
	Vector <float> Q_values = model.compute_no_cache(current_state);

	using namespace std;
	cout << "Q_values: " << Q_values << endl;

	size_t mx = 0;

	float rnd = distribution(generator);

	if (rnd > eps) {
		// cout << "Network!" << endl;
		mx = Q_values.imax();
	} else {
		// cout << "Heurestic!" << endl;
		mx = 2;
		if (current_state[3] - current_state[4] > 0.1)
			mx = 1;
		else if (current_state[4] - current_state[3] > 0.1)
			mx = 0;

		mx += (rnd > 0.5) ? 3 : 0;
	}

	Q_value = Q_values[mx];

	return mx;
}

size_t Agent::best_action()
{
	Vector <float> Q_values = model.compute_no_cache(current_state);

	return Q_values.imax();
}

bool Agent::move(size_t mx, float delta)
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

void Agent::_init() {rt = 0;}

void Agent::rand_reset()
{
	int index = rand() % spawns;
	// int index = 0;
	
	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(index));
	
	set_rotation_degrees(90 + nd->get_rotation_degrees());
	set_global_position(nd->get_global_position());

	ppos = get_global_position();

	// Method/function
	rewards[id].push(rt);
	tds[id].push(total/frames);
	epsilons[id].push(eps);
	episodes[id]++;

	csv << episode++ << "," << rt << std::endl;

	cycles = 0;
	velocity = max_vel * rand()/((double) RAND_MAX);
	rt = 0;
	total = 0;
	frames = 0;
}

Vector <float> Agent::get_state()
{
	return Vector <float> (5,
		[&](size_t i) {
			switch (i) {
			case 0:
				return velocity;
			case 1:
				return velocity * (float) cos(get_rotation());
			case 2:
				return velocity * (float) sin(get_rotation());
			default:
				Vector2 other = rays[i - 3]->get_collision_point();

				return (float) get_global_position().distance_to(other);
			}
		}
	);
}

void Agent::accelerate(size_t i, float delta)
{
	// i = 0 is acceleration, i = 1 is nothing (idle)
	if (i)
		velocity *= pow(k_d, delta);
	else
		velocity += k_a * delta;
}

void Agent::steer(size_t i)
{
	// i = 0 is left, i = 1 is right
	if (i == 1)
		set_rotation(get_rotation() + velocity * 0.0005);
	else if (i == 0)
		set_rotation(get_rotation() - velocity * 0.0005);
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

	// Put this into the master _ready function
	rewards.push_back(std::queue <float> ());
	epsilons.push_back(std::queue <float> ());
	tds.push_back(std::queue <float> ());
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

	p_pos = get_global_position();
}
