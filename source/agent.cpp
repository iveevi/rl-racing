#include <agent.hpp>

namespace godot {

double Agent::min_vel = -1;
double Agent::max_vel = 15;
double Agent::idle_vel = 0.05;

// Perhaps use delta instead
int Agent::cycle_thresh = 1000;

double Agent::k_a = 10;
double Agent::k_b = 0.997;
double Agent::k_d = 0.999;

// Make a per-agent property
double Agent::epsilon = 0.9;

size_t Agent::buffer_size = 10;

double Agent::lambda = 0.997;

Agent::Agent()
{
	srand(clock());

	cycles = 0;
	spawn = 0;
	velocity = 0;
	rt = 0;

	meps = 0.01;
	eps = 1.0;
	runs = 0;
	explt = false;
	r_explt = 0;
	r_explr = 0;
}

Agent::~Agent() {}

void Agent::increment_buffer_index()
{
	buffer_index = (buffer_index + 1) % buffer_size;
}

void Agent::set_buffer_reward(double new_reward)
{
	buffer_rewards[buffer_index] = new_reward;
}

void Agent::_init() {rt = 0;}

void Agent::rand_reset()
{
	int index = rand() % spawns;
	
	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(index));
	
	set_rotation_degrees(90 + nd->get_rotation_degrees());
	set_global_position(nd->get_global_position());

	ppos = get_global_position();

	rewards[id].push(rt);
	epsilons[id].push((explt) ? meps : eps);
	episodes[id]++;

	csv << episode++ << "," << rt << std::endl;

	// Determine next episodes setup
	using namespace std;
	runs++;
	if (explt)
		r_explt += rt;
	else
		r_explr += rt;

	if (explt) {
		if (runs % 20 == 0) {
			// Store the constant in the JSON file
			if (r_explt > 1.1 * r_explr) {
				epsilon = cap(epsilon - 0.01, 0.01, 1.0)
			} else {
				epsilon = cap(epsilon + 0.01, 0.01, 1.0)
			}

			r_explt = 0;
			r_explr = 0;

			explt = false;
			runs = 0;
		}
	} else {
		if (runs % 15 == 0) {
			explt = true;
		}
	}

	// TODO: Decrease or increase epsilon

	cycles = 0;
	velocity = 0;
	rt = 0;

	// Train on the replay buffer
	buffer_rewards[buffer_index] = 0;

	DataSet <double> ins;
	DataSet <double> outs;

	size_t e;
	for (size_t i = 0; i < buffer_size; i++) {
		if (buffer_indexes[i] < 0)
			continue;
		
		e = (i + buffer_size - buffer_size) % buffer_size;
		buffer_rewards[i] -= 100 * pow(0.97, e);

		buffer_actions[i][buffer_indexes[i]] = buffer_rewards[i];

		ins.push_back(states[i]);
		outs.push_back(buffer_actions[i]);
	}

	model.train <10> (ins, outs, 0.00025);

	buffer_indexes.assign(buffer_size, -1);
	buffer_index = 0;

	crashed = true;
}

double Agent::reward_delta()
{
	if (crashed) {
		crashed = false;

		return -100;
	}
	
	if (velocity < idle_vel)
		cycles++;
	else
		cycles = 0;

	// Make more complicated later
	double r = velocity;

		/* + (brake ? -0.0015 : 0) do not penalize idle and/or braking
		+ (idle ? -0.0005 : 0); */

	rt += r;

	return r;
}

Vector <double> Agent::reward(const Vector <double> &r, size_t imax)
{
	double distance = get_global_position().distance_to(ppos);

	if (distance < idle_vel)
		cycles++;
	else
		cycles = 0;

	zhetapi::Vector <double> rt = r;

	rt[imax] = distance - max_vel/2;

	return rt;
}

void Agent::cache_state()
{
	p_states[id] = c_states[id];

	c_states[id] = Vector <double> (11,
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

	states[buffer_index] = c_states[id];
}

size_t Agent::apply_action(double delta)
{
	size_t mx = 0;

	// Change to uniform distribution
	double rnd = distribution(generator);

	double e = (explt) ? meps : eps;

	if (rnd > e)
		mx = actions[id].imax();
	else
		mx = rand() % 6;

	buffer_actions[buffer_index] = actions[id];
	buffer_indexes[buffer_index] = mx;

	move(mx, delta);

	return mx;
}

void Agent::move(size_t mx, double delta)
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

		return;
	}
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
	// Buffer
	
	// Assume that an all 0 state is an "empty" or "null" state
	states.assign(buffer_size, Vector <double> (11, 0.0));
	buffer_actions.assign(buffer_size, Vector <double> (6, 0.0));
	buffer_rewards.assign(buffer_size, 0);
	buffer_indexes.assign(buffer_size, -1);

	buffer_index = 0;

	// Rest
	spawns = get_node(spawn)->get_child_count();

	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(0));

	set_rotation_degrees(90 + angle);
	set_global_position(nd->get_global_position());

	ppos = get_global_position();

	rays = new RayCast2D *[8];
	for (size_t i = 0; i < 8; i++)
		rays[i] = Object::cast_to <RayCast2D> (get_child(i + 2));
	
	id = size++;
	agents.push_back(this);
	c_states.push_back(get_state());
	p_states.push_back(get_state());
	r_deltas.push_back(0);
	mxs.push_back(0);
	rewards.push_back(std::queue <double> ());
	epsilons.push_back(std::queue <double> ());
	episodes.push_back(1);
	flushed.push_back(false);
	actions.push_back(Vector <double> (6, 0.0));

	std::string fpath = "results/" + dir + "/agent_" + std::to_string(id);
	csv.open(fpath);
	csv << "episode,reward" << std::endl;

	episode = 1;
}

}
