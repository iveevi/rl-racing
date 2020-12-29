#ifndef CAR_H_
#define CAR_H_

#include <Godot.hpp>
#include <KinematicBody2D.hpp>
#include <NodePath.hpp>

namespace godot {

class Agent : public KinematicBody2D {
	
	GODOT_CLASS(Agent, KinematicBody2D);

	NodePath spawn;

	double angle;

public:
	static void _register_methods();

	Agent();
	~Agent();

	void _init();

	void _process(float delta);
};

}

#endif
