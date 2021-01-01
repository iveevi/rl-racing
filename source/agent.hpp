#ifndef AGENT_H_
#define AGENT_H_

// C++ standard headers
#include <vector>

// Godot headers
#include <GlobalConstants.hpp>
#include <Godot.hpp>
#include <Input.hpp>
#include <KinematicBody2D.hpp>
#include <KinematicCollision2D.hpp>
#include <NodePath.hpp>
#include <RayCast2D.hpp>

// Zhetapi headers
#include <network.hpp>
#include <std/activation_classes.hpp>
#include <std/optimizer_classes.hpp>

// Source headers
#include <global.hpp>

#define cap(x, mn, mx) std::max(std::min(x, mx), mn);

namespace godot {

using namespace zhetapi;

class Agent : public KinematicBody2D {
private:
	int				spawns;
	int				cycles;
	double				velocity;
	Vector2				ppos;

	std::ofstream			csv;
	size_t				episode;

	double				rt;

	bool brake;
	bool idle;

	// Sensors
	RayCast2D **			rays;

	// Index
	size_t				id;

	// Instantiate as a Godot class
	GODOT_CLASS(Agent, KinematicBody2D);
public:
	// Accessible from GDScript classes
	NodePath spawn;

	double angle;
	
	// Methods
	static void _register_methods();

	Agent();
	~Agent();

	void rand_reset();

	double get_velocity() const {return velocity;}

	double reward_delta();
	Vector <double> reward(const Vector <double> &, size_t);

	void cache_state();
	size_t apply_action(double);	// Returns the action index
	void move(size_t, double);

	Vector <double> get_state();

	void accelerate(size_t, double);
	void steer(size_t);
	
	void _init();

	void _ready();
	void run(float delta);		// Main function

	static const double min_vel;
	static const double max_vel;
	static const double idle_vel;	// Maximum velocity considered to be "idle"

	static const int cycle_thresh;	// Number of "idle" cycles before reseting
	
	static const double k_a;	// Acceleration constant
	static const double k_b;	// Brake constant
	static const double k_d;	// Drag/friction constant
	
	static const double eps;
};

}

#endif
