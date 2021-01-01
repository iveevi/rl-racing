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

	bool crashed;

	std::vector <Vector <double>>	states;

	// Sensors
	RayCast2D **			rays;

	// Index
	size_t				id;

	// Exploration strategy
	double				eps;
	size_t				runs;
	bool				explt;
	double				meps;
	double				r_explt;
	double				r_explr;

	// Instantiate as a Godot class
	GODOT_CLASS(Agent, KinematicBody2D);
public:
	size_t				buffer_index;
	std::vector <double>		buffer_rewards;
	std::vector <int>		buffer_indexes;
	std::vector <Vector <double>>	buffer_actions;
	
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

	void increment_buffer_index();
	void set_buffer_reward(double);
	
	void _init();

	void _ready();
	void run(float delta);		// Main function

	static double min_vel;
	static double max_vel;
	static double idle_vel;		// Maximum velocity considered to be "idle"

	static int cycle_thresh;	// Number of "idle" cycles before reseting
	
	static double k_a;		// Acceleration constant
	static double k_b;		// Brake constant
	static double k_d;		// Drag/friction constant
	
	static double epsilon;

	static size_t buffer_size;

	static double lambda;		// Discount factor
};

}

#endif
