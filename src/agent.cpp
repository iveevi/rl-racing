#include "agent.hpp"

namespace godot {

const double Agent::min_vel = -5;
const double Agent::max_vel = 20;

const double Agent::acceleration = 0.0005;
const double Agent::brake = 0.997;
const double Agent::drag = 0.999;

void Agent::_register_methods()
{
	register_method("_ready", &Agent::_ready);
	register_method("_process", &Agent::_process);

	register_property <Agent, NodePath> ("spawn", &Agent::spawn, NodePath());
	register_property <Agent, double> ("angle", &Agent::angle, 0);
}

Agent::Agent()
{
	srand(clock());

	velocity = 0;
}

Agent::~Agent() {}

void Agent::rand_reset()
{
	using namespace std;
	cout << "RESETING!\n" << endl;

	int index = rand() % spawns;
	
	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(index));

	cout << "\tspawn(2D) -> " << nd << endl;
	
	set_rotation(nd->get_rotation());
	set_global_position(nd->get_global_position());
}

void Agent::_init()
{
}

void Agent::_ready()
{
	using namespace std;
	cout << "=====================" << endl;
	cout << "Initializing agent..." << endl;

	cout << boolalpha << spawn.is_empty() << endl;

	cout << "spawn -> " << get_node(spawn) << endl;

	cout << "spawns available: " << get_node(spawn)->get_child_count() << endl;

	spawns = get_node(spawn)->get_child_count();

	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(0));

	cout << "spawn(2D) -> " << nd << endl;

	Godot::print((String) spawn);
	Godot::print(nd->get_global_position());

	set_rotation(angle);
	set_global_position(nd->get_global_position());
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
}

}
