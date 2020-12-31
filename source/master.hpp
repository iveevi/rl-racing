#ifndef NETWORK_H_
#define NETWORK_H_

// Godot headers
#include <Camera2D.hpp>
#include <Godot.hpp>
#include <Node2D.hpp>
#include <PackedScene.hpp>
#include <ResourceLoader.hpp>
#include <TextEdit.hpp>

namespace godot {

class Master : public Node2D {
private:
	// Instantiate as a Godot class
	GODOT_CLASS(Master, Node2D);

public:
	// GDScript accessible variables
	int rows;
	int cols;

	// Methods
	static void _register_methods();

	Master() {};
	~Master() {};
	
	void _init() {rows = 1; cols = 1;};

	void _ready();
	void _process();
};

// Godot standard methods
void Master::_register_methods()
{
	register_method("_ready", &Master::_ready);
	register_method("_process", &Master::_process);

	register_property <Master, int> ("rows", &Master::rows, 1);
	register_property <Master, int> ("cols", &Master::cols, 1);
}

void Master::_ready()
{
	ResourceLoader *rl = ResourceLoader::get_singleton();

	Ref <PackedScene> ref = rl->load("res://scenes/track.tscn");

	double xoff = 0;
	double yoff = 0;
	for (int i = 0; i < rows; i++) {
		yoff = 0;

		for (int j = 0; j < cols; j++) {
			Node *node = ref->instance();

			Node2D *track = Object::cast_to <Node2D> (node);

			track->set_position(Vector2(xoff, yoff));

			node = track->get_node("stats");

			TextEdit *tedit = Object::cast_to <TextEdit> (node);

			std::string str = "\n(" + std::to_string(i + 1) + ", " + std::to_string(j + 1) + ")";

			String text = str.c_str();

			tedit->set_text(text);

			add_child(track);

			yoff += 7000;
		}

		xoff += 11000;
	}

	double cx = xoff + 10240;
	double cy = 7000 * cols + 6000;

	double z = std::max(10 * cx/10240, 10 * cy/6000);

	Node *node = get_parent()->get_node("cells");

	Camera2D *cam = Object::cast_to <Camera2D> (node);

	cam->set_zoom(Vector2(z, z));
	cam->set_global_position(Vector2(
				xoff/2 - 5000,
				3500 * cols - 5000
	));
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

    godot::register_class <godot::Master> ();
}

#endif
