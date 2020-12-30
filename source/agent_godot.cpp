#include <agent.hpp>

// Godot related functions
namespace godot {

void Agent::_register_methods()
{
	register_method("_ready", &Agent::_ready);
	register_method("_process", &Agent::run);

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

void Agent::_ready()
{
	spawns = get_node(spawn)->get_child_count();

	Node2D *nd = Object::cast_to <Node2D> (get_node(spawn)->get_child(0));

	set_rotation(angle);
	set_global_position(nd->get_global_position());

	ppos = get_global_position();
}

}
