#include <agent.hpp>
#include <master.hpp>

namespace godot {

// Godot standard methods
void Agent::_register_methods()
{
	register_method("_init", &Agent::_init);
	register_method("_ready", &Agent::_ready);

	register_property <Agent, NodePath> ("spawn", &Agent::spawn, NodePath());
	register_property <Agent, double> ("angle", &Agent::angle, 0);
}

// Godot standard methods
void Master::_register_methods()
{
	register_method("_init", &Master::_init);
	register_method("_ready", &Master::_ready);
	register_method("_process", &Master::_process);

	register_property <Master, int> ("rows", &Master::rows, 1);
	register_property <Master, int> ("cols", &Master::cols, 1);
}

}
