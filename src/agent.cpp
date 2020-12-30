#include "agent.hpp"

namespace godot {

const double Agent::max_vel = 20;
const double Agent::min_vel = -5;

void Agent::_register_methods()
{
	register_method("_ready", &Agent::_ready);
	register_method("_process", &Agent::_process);

	register_property <Agent, NodePath> ("spawn", &Agent::spawn, NodePath());
	register_property <Agent, double> ("angle", &Agent::angle, 0);
}

Agent::Agent() {}

Agent::~Agent() {}

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

	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(0));

	cout << "spawn(2D) -> " << nd << endl;

	Godot::print((String) spawn);
	Godot::print(nd->get_global_position());

	set_rotation(angle);
	set_global_position(nd->get_global_position());
}

void Agent::_process(float delta)
{
	if (Input::get_singleton()->is_key_pressed(GlobalConstants::KEY_UP))
		move_and_collide(Vector2(0, 0.01));
}

}
