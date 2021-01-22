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
	Vector2				p_pos;

	Vector <float>			current_state;
	Vector <float>			previous_state;

	bool				crashed;
	bool				rewarded;

	int				spawns;
	int				cycles;
	float				velocity;
	Vector2				ppos;

	// Logging information
	std::ofstream			csv;
	size_t				episode;
	float				rt;

	// Sensors
	RayCast2D **			rays;

	Area2D **			gate_list;
	size_t				gate_size;
	bool *				gate_passed;

	// Index
	size_t				id;

	// Exploration strategy
	float				eps;

	// TD error
	size_t				frames;
	float				total;

	float				Q_value;

	// Instantiate as a Godot class
	GODOT_CLASS(Agent, KinematicBody2D);
public:
	void step(float);
	void best_step(float);
	size_t get_action();
	size_t best_action();
	float get_reward();
	bool move(size_t, float);
	bool passed_gate() const;

	// Accessible from GDScript classes
	NodePath spawn;
	NodePath gates;

	float angle;
	
	// Methods
	static void _register_methods();

	Agent();
	~Agent();

	void rand_reset();

	Vector <float> get_state();

	void accelerate(size_t, float);
	void steer(size_t);

	void _init();

	void _ready();

	static float min_vel;
	static float max_vel;
	static float idle_vel;		// Maximum velocity considered to be "idle"

	static int cycle_thresh;	// Number of "idle" cycles before reseting
	
	static float k_a;		// Acceleration constant
	static float k_b;		// Brake constant
	static float k_d;		// Drag/friction constant

	static float lambda;		// Discount factor
	
	static float deps;
};

#endif
