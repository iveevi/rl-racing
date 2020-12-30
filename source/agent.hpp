#ifndef CAR_H_
#define CAR_H_

// Godot headers
#include <GlobalConstants.hpp>
#include <Godot.hpp>
#include <Input.hpp>
#include <KinematicBody2D.hpp>
#include <KinematicCollision2D.hpp>
#include <NodePath.hpp>

// Zhetapi headers
#include <network.hpp>
#include <std/activation_classes.hpp>

namespace godot {

class Agent : public KinematicBody2D {
	double velocity;

	int spawns;

	zhetapi::Vector <double> state;
	
	zhetapi::ml::NeuralNetwork <double> model;

	// Instantiate as a Godot class
	GODOT_CLASS(Agent, KinematicBody2D);

public:
	// Accessible from GDScript classes
	NodePath spawn;

	double angle;

	double fright;
	double fleft;
	double bright;
	double bleft;
	double front;
	double back;
	double right;
	double left;
	
	// Methods
	static void _register_methods();

	Agent();
	~Agent();

	void rand_reset();
	
	void _init();

	void _ready();
	void _process(float delta);

	static const double min_vel;
	static const double max_vel;
	
	static const double acceleration;
	static const double brake;
	static const double drag;
};

}

#endif
