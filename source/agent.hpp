#ifndef AGENT_H_
#define AGENT_H_

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

#define cap(x, mn, mx) std::max(std::min(x, mx), mn);

namespace godot {

using namespace zhetapi;

class Agent : public KinematicBody2D {
private:
	int				spawns;
	int				cycles;
	double				velocity;
	Vector2				ppos;
	ml::NeuralNetwork <double>	model;
	ml::Optimizer <double> *	cost;
	ml::Activation <double> *	boltzmann;

	// Sensors
	RayCast2D **			rays;

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

	Vector <double> reward(const Vector <double> &, size_t);
	Vector <double> state();

	void accelerate(size_t);
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

// Godot standard methods
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

	rays = new RayCast2D *[8];

	using namespace std;
	cout << "rays:" << endl;
	for (size_t i = 0; i < 8; i++) {
		rays[i] = Object::cast_to <RayCast2D> (get_child(i + 2));
		cout << "\t" << rays[i] << endl;
	}
}

}

// Exporting
extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
    godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
    godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
    godot::Godot::nativescript_init(handle);

    godot::register_class <godot::Agent> ();
}

#endif
