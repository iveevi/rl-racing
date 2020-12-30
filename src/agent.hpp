#ifndef CAR_H_
#define CAR_H_

#include <Godot.hpp>
#include <KinematicBody2D.hpp>
#include <KinematicCollision2D.hpp>
#include <NodePath.hpp>
#include <Input.hpp>
#include <GlobalConstants.hpp>

namespace godot {

class Agent : public KinematicBody2D {
	
	GODOT_CLASS(Agent, KinematicBody2D);

	NodePath spawn;

	double angle;
	
	bool started;
	
	Vector2 velocity;

public:
	static void _register_methods();

	Agent();
	~Agent();

	void _init();

	void _ready();
	void _process(float delta);

	static const double max_vel;
	static const double min_vel;

	static const Input *input;
};

}

#endif
