#include <master.hpp>

// Godot related functions
namespace godot {

// List of all agents
std::vector <Agent *> agents;

// Path to the track scene
const char *p_track = "res://scenes/track.tscn";

Master::Master() {}

Master::~Master() {}

void Master::_init()
{
	using namespace std;
	std::cout << "Setting:" << endl;
	rows = 1;
	cols = 1;
}

void Master::_process()
{
	using namespace std;
	cout << "agents:" << endl;
	for (auto ag : agents)
		cout << "\t" << ag << endl;
}

// Godot standard methods
void Master::_register_methods()
{
	register_method("_init", &Master::_init);
	register_method("_ready", &Master::_ready);
	register_method("_process", &Master::_process);

	register_property <Master, int> ("rows", &Master::rows, 1);
	register_property <Master, int> ("cols", &Master::cols, 1);
}

void Master::_ready()
{
	// Hard code for now, read from a file later
	rows = 1;
	cols = 1;

	ResourceLoader *rl = ResourceLoader::get_singleton();

	Ref <PackedScene> ref = rl->load(p_track);

	using namespace std;
	cout << "ref: " << boolalpha << ref->can_instance() << endl;

	cout << "rows = " << rows << endl;
	cout << "cols = " << cols << endl;

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
