#include <agent.hpp>

namespace godot {

const double Agent::min_vel = -1;
const double Agent::max_vel = 15;
const double Agent::idle_vel = 0.05;

const int Agent::cycle_thresh = 10000;

const double Agent::k_a = 10;
const double Agent::k_b = 0.997;
const double Agent::k_d = 0.999;

const double Agent::eps = 0.7;

Agent::Agent()
{
	srand(clock());

	cycles = 0;
	spawn = 0;
	velocity = 0;
	rt = 0;
}

Agent::~Agent() {}

void Agent::_init() {rt = 0;}

void Agent::rand_reset()
{
	int index = rand() % spawns;
	
	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(index));
	
	set_rotation_degrees(90 + nd->get_rotation_degrees());
	set_global_position(nd->get_global_position());

	ppos = get_global_position();

	rewards[id].push(rt);
	episodes[id]++;

	// std::cout << id << " finished with " << rt << std::endl;

	csv << episode++ << "," << rt << std::endl;

	cycles = 0;
	velocity = 0;
	rt = 0;
}

double Agent::reward_delta()
{
	// Make more complicated later
	double r = velocity
		+ (brake ? -0.15 : 0)
		+ (idle ? -0.05 : 0);

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

	c_states[id] = Vector <double> (9,
		[&](size_t i) {
			if (!i)
				return velocity;

			Vector2 other = rays[i - 1]->get_collision_point();

			return (double) get_global_position().distance_to(other);
		}
	);
}

size_t Agent::apply_action(double delta)
{
	size_t mx = 0;

	// Change to uniform distribution
	double rnd = rand()/((double) RAND_MAX);

	if (rnd > eps)
		mx = actions[id].imax();
	else
		mx = rand() % 9;

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

	if (ref.ptr()) {
		rand_reset();

		return;
	}
}

Vector <double> Agent::get_state()
{
	return Vector <double> (9,
		[&](size_t i) {
			if (!i)
				return velocity;

			Vector2 other = rays[i - 1]->get_collision_point();

			return (double) get_global_position().distance_to(other);
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
		// Brake
		brake = true;
		velocity *= pow(k_b, delta);
		break;
	case 2:
		// Nothing
		idle = true;
		velocity *= pow(k_d, delta);
		break;
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
	episodes.push_back(1);
	flushed.push_back(false);
	actions.push_back(Vector <double> (9, 0.0));

	std::string fpath = "results/" + dir + "/agent_" + std::to_string(id);
	csv.open(fpath);
	csv << "episode,reward" << std::endl;

	episode = 1;
}

}
