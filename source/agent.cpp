#include <agent.hpp>

namespace godot {

const double Agent::min_vel = -1;
const double Agent::max_vel = 5;

const double Agent::k_a = 0.00005;
const double Agent::k_b = 0.997;
const double Agent::k_d = 0.999;

Agent::Agent()
{
	srand(clock());

	velocity = 0;

	auto initializer = []() {
		return 0.5 - (rand()/(double) RAND_MAX);
	};

	// Load from JSON file later
	model = ml::NeuralNetwork <double> ({
		{8, new ml::Linear <double> ()},
		{10, new ml::Sigmoid <double> ()},
		{10, new ml::ReLU <double> ()},
		{9, new ml::Linear <double> ()}
	}, initializer);

	cost = new ml::MeanSquaredError <double> ();

	model.randomize();
	model.set_cost(cost);
}

Agent::~Agent()
{
	delete cost;
}

void Agent::rand_reset()
{
	int index = rand() % spawns;
	
	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(index));
	
	set_rotation(nd->get_rotation());
	set_global_position(nd->get_global_position());

	ppos = get_global_position();

	velocity = 0;
}

Vector <double> Agent::reward(const Vector <double> &r, size_t imax) const
{
	double distance = get_global_position().distance_to(ppos);

	zhetapi::Vector <double> rt = r;

	rt[imax] = distance - max_vel/2;

	return rt;
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
	velocity = std::max(std::min(velocity, max_vel), min_vel);

	// Move the car
	ref = move_and_collide(
		Vector2(
			velocity * cos(get_rotation()),
			velocity * sin(get_rotation())
	));

	if (ref.ptr()) {
		rand_reset();

		return;
	}

	Vector <double> st = {
		fright,
		fleft,
		bright,
		bleft,
		front,
		back,
		right,
		left
	};

	// std::cout << "state = " << st << std::endl;
	Vector <double> rewards = model(st);

	size_t mx = rewards.imax();

	std::cout << "rewards = " << rewards << std::endl;
	std::cout << "\tmaxi = " << mx << " [gas=" << mx/3 << ", steer=" << mx % 3 << "]" << std::endl;
	
	accelerate(mx / 3);
	steer(mx % 3);

	std::cout << "\ttrue reward: " << reward(rewards, mx) << std::endl;

	ppos = get_global_position();
}

}
