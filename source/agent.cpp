#include <agent.hpp>

namespace godot {

const double Agent::min_vel = -1;
const double Agent::max_vel = 5;
const double Agent::idle_vel = 0.05;

const int Agent::cycle_thresh = 10000;

const double Agent::k_a = 0.0002;
const double Agent::k_b = 0.997;
const double Agent::k_d = 0.999;

const double Agent::eps = 0.7;

std::ofstream csv("data.csv");

size_t iter = 0;

Agent::Agent()
{
	srand(clock());

	cycles = 0;
	spawn = 0;
	velocity = 0;

	auto initializer = []() {
		return 0.5 - (rand()/(double) RAND_MAX);
	};

	// Load from JSON file later
	model = ml::NeuralNetwork <double> ({
		{9, new ml::Linear <double> ()},
		{10, new ml::Sigmoid <double> ()},
		{10, new ml::ReLU <double> ()},
		{9, new ml::Linear <double> ()}
	}, initializer);

	cost = new ml::MeanSquaredError <double> ();
	boltzmann = new ml::SoftmaxInterval <double> ();

	model.randomize();
	model.set_cost(cost);

	csv << "iter,error" << std::endl;
}

Agent::~Agent()
{
	delete cost;
	delete boltzmann;
}

void Agent::rand_reset()
{
	int index = rand() % spawns;
	
	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(index));
	
	set_rotation(nd->get_rotation());
	set_global_position(nd->get_global_position());

	ppos = get_global_position();

	cycles = 0;
	velocity = 0;
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

Vector <double> Agent::state()
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
void Agent::accelerate(size_t i)
{
	switch (i) {
	case 0:
		// Accelerate
		velocity += k_a;
		break;
	case 1:
		// Brake (exclude for now)
		// velocity *= k_b;
		// break;
	case 2:
		// Nothing
		velocity *= k_d;
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

void Agent::_init()
{
}

void Agent::run(float delta)
{
	Ref <KinematicCollision2D> ref = nullptr;

	// Cap the velocity from both sides
	velocity = cap(velocity, min_vel, max_vel);

	// Move the car
	ref = move_and_collide(
		Vector2(
			velocity * cos(get_rotation()),
			velocity * sin(get_rotation())
	));

	if (ref.ptr() || cycles >= cycle_thresh) {
		rand_reset();

		return;
	}

	Vector <double> st = state();

	Vector <double> rewards = model(st);

	double rng = rand()/((double) RAND_MAX);

	size_t mx = 0;
	if (rng > eps) {
		mx = rewards.imax();
	} else {
		mx = rand() % 9;
	}

	// This is not the correct reward
	Vector <double> rt = reward(rewards, mx);

	// std::cout << (*cost)(rt, rewards) << std::endl;
	iter++;

	if (!(iter % 1000))
		csv << iter << "," << (cost->compute(rt, rewards))[0] << std::endl;

	model.train(st, rt, 0.001);
	
	accelerate(mx / 3);
	steer(mx % 3);

	ppos = get_global_position();
}

}
