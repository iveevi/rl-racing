#include <agent.hpp>

namespace godot {

const double Agent::min_vel = -5;
const double Agent::max_vel = 20;

const double Agent::acceleration = 0.00015;
const double Agent::brake = 0.997;
const double Agent::drag = 0.999;

void Agent::_register_methods()
{
	register_method("_ready", &Agent::_ready);
	register_method("_process", &Agent::_process);

	register_property <Agent, NodePath> ("spawn", &Agent::spawn, NodePath());
	register_property <Agent, double> ("angle", &Agent::angle, 0);
	
	register_property <Agent, double> ("fright", &Agent::fright, 0);
	register_property <Agent, double> ("fleft", &Agent::fleft, 0);
	register_property <Agent, double> ("bright", &Agent::bright, 0);
	register_property <Agent, double> ("bleft", &Agent::bleft, 0);
	register_property <Agent, double> ("front", &Agent::front, 0);
	register_property <Agent, double> ("back", &Agent::back, 0);
	register_property <Agent, double> ("right", &Agent::right, 0);
	register_property <Agent, double> ("left", &Agent::left, 0);
}

Agent::Agent()
{
	srand(clock());

	velocity = 0;

	auto initializer = []() {
		return 0.5 - (rand()/(double) RAND_MAX);
	};

	model = zhetapi::ml::NeuralNetwork <double> ({
		{8, new zhetapi::ml::Linear <double> ()},
		{10, new zhetapi::ml::Sigmoid <double> ()},
		{10, new zhetapi::ml::ReLU <double> ()},
		{9, new zhetapi::ml::Linear <double> ()}
	}, initializer);

	model.randomize();
}

Agent::~Agent() {}

void Agent::rand_reset()
{
	int index = rand() % spawns;
	
	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(index));
	
	set_rotation(nd->get_rotation());
	set_global_position(nd->get_global_position());
}

void Agent::_init()
{
}

void Agent::_ready()
{
	spawns = get_node(spawn)->get_child_count();

	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(0));

	set_rotation(angle);
	set_global_position(nd->get_global_position());

	state = zhetapi::Vector <double> (8, 0.0);
}

void Agent::_process(float delta)
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

	// Determine the next step
	if(Input::get_singleton()->is_key_pressed(GlobalConstants::KEY_UP)) {
		velocity += acceleration;
	} else if (Input::get_singleton()->is_key_pressed(GlobalConstants::KEY_SPACE)) {
		velocity *= brake;
	} else if (Input::get_singleton()->is_key_pressed(GlobalConstants::KEY_DOWN)) {
		velocity -= acceleration;
	} else {
		velocity *= drag;
	}

	if (Input::get_singleton()->is_key_pressed(GlobalConstants::KEY_LEFT)) {
		set_rotation(get_rotation() - velocity * 0.0025);
	} else if (Input::get_singleton()->is_key_pressed(GlobalConstants::KEY_RIGHT)) {
		set_rotation(get_rotation() + velocity * 0.0025);
	}

	zhetapi::Vector st = {
		fright,
		fleft,
		bright,
		bleft,
		front,
		back,
		right,
		left
	};

	std::cout << "state = " << st << std::endl;
}

}
