#ifndef MASTER_H_
#define MASTER_H_

// C++ standard headers
#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

// Godot headers
#include <Camera2D.hpp>
#include <Godot.hpp>
#include <Node2D.hpp>
#include <PackedScene.hpp>
#include <ResourceLoader.hpp>
#include <TextEdit.hpp>

// Zhetapi headers
#include <json/json.hpp>

// Source headers
#include <agent.hpp>
#include <global.hpp>

using namespace godot;

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
	void _process(float);
};

#endif
