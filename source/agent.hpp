#ifndef AGENT_H_
#define AGENT_H_

// C++ standard headers
#include <vector>

// Godot headers
#include <Area2D.hpp>
#include <GlobalConstants.hpp>
#include <Godot.hpp>
#include <Input.hpp>
#include <KinematicBody2D.hpp>
#include <KinematicCollision2D.hpp>
#include <NodePath.hpp>
#include <RayCast2D.hpp>

// Zhetapi headers
#include <network.hpp>
#include <std/activations.hpp>
#include <std/erfs.hpp>

// Source headers
#include <global.hpp>

#define cap(x, mn, mx) std::max(std::min(x, mx), mn);

using namespace godot;
using namespace zhetapi;

class Agent : public KinematicBody2D {
private:
	Vector <double>			current_state;
	Vector <double>			previous_state;

	bool				crashed;
	bool				rewarded;

	int				spawns;
	int				cycles;
	double				velocity;
	Vector2				ppos;

	// Logging information
	std::ofstream			csv;
	size_t				episode;
	double				rt;

	// Sensors
	RayCast2D **			rays;

	Area2D **			gate_list;
	size_t				gate_size;
	bool *				gate_passed;

	// Index
	size_t				id;

	// Exploration strategy
	double				eps;

	// Instantiate as a Godot class
	GODOT_CLASS(Agent, KinematicBody2D);
public:
	void step(double);
	void best_step(double);
	std::pair <size_t, Vector <double>> get_action();
	size_t best_action();
	double get_reward();
	bool move(size_t, double);
	bool passed_gate() const;

	// Accessible from GDScript classes
	NodePath spawn;
	NodePath gates;

	double angle;
	
	// Methods
	static void _register_methods();

	Agent();
	~Agent();

	void rand_reset();

	Vector <double> get_state();

	void accelerate(size_t, double);
	void steer(size_t);

	void _init();

	void _ready();

	static double min_vel;
	static double max_vel;
	static double idle_vel;		// Maximum velocity considered to be "idle"

	static int cycle_thresh;	// Number of "idle" cycles before reseting
	
	static double k_a;		// Acceleration constant
	static double k_b;		// Brake constant
	static double k_d;		// Drag/friction constant

	static double lambda;		// Discount factor
};

#endif
