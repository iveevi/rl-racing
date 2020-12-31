#ifndef MASTER_H_
#define MASTER_H_

// C++ standard headers
#include <vector>

// Godot headers
#include <Camera2D.hpp>
#include <Godot.hpp>
#include <Node2D.hpp>
#include <PackedScene.hpp>
#include <ResourceLoader.hpp>
#include <TextEdit.hpp>

// Source headers
#include <agent.hpp>

namespace godot {

// List of all agents
extern std::vector <Agent *> agents;

extern int i;

// Path to the track scene
extern const char *p_track;

class Master : public Node2D {
private:
	// Instantiate as a Godot class
	GODOT_CLASS(Master, Node2D);
	
	// TODO: Read these values from a configuration file
	int rows;
	int cols;
public:
	// Methods
	static void _register_methods();

	Master();
	~Master();
	
	void _init();

	void _ready();
	void _process();
};

}

#endif
