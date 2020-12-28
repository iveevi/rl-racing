#include "ai.hpp"

namespace godot {

void car::_register_methods()
{
	register_method("_process", &car::_process);
}

car::car() {}

car::~car() {}

void car::_init() {}

void car::_process(float delta)
{
}

}
