#include <master.hpp>

// Godot related functions
namespace godot {

//---------------------[METHODS]-------------------

Master::Master()
{
	auto initializer = []() {
		return 0.5 - (rand()/(double) RAND_MAX);
	};

	// Load from JSON file later
	model = ml::NeuralNetwork <double> ({
		{9, new ml::Linear <double> ()},
		{10, new ml::Sigmoid <double> ()},
		{10, new ml::ReLU <double> ()},
		{9, new ml::Linear <double> ()}
	}, initializer);

	cost = new ml::MeanSquaredError <double> ();

	model.randomize();
	model.set_cost(cost);
}

Master::~Master()
{
	delete cost;
}

void Master::_init() {}

Vector <double> acpy;

bool launch_graph = false;
std::vector <TextEdit *> texts;

int c_episode = 1;
double avg_reward = 0;

std::ofstream mout;
void Master::_process(float delta)
{
	if (!launch_graph) {
		launch_graph = true;

		std::string cmd;

		cmd = "python show_agents.py " + dir + " " + std::to_string(size) + " &";

		system(cmd.c_str());
		
		cmd = "python show_average.py " + dir + " &";
		
		system(cmd.c_str());
	}

	bool c_done = true;
	for (size_t i = 0; i < size; i++) {
		if (episodes[i] > c_episode) {
			if (!flushed[i]) {
				flushed[i] = true;

				avg_reward += rewards[i].front();
				rewards[i].pop();
			}
		} else {
			c_done = false;
		}
	}

	if (c_done) {
		for (size_t i = 0; i < size; i++)
			flushed[i] = false;

		avg_reward /= size;

		std::cout << "all done with episode #" << c_episode << ", rew = " << avg_reward << std::endl;

		mout << c_episode << "," << avg_reward << std::endl;

		c_episode++;
		avg_reward = 0;
	}

	// Separate loops or single loop?
	for (size_t i = 0; i < size; i++) {
		agents[i]->cache_state();

		r_deltas[i] = agents[i]->reward_delta();

		double total = r_deltas[i] + model(c_states[i]).max();

		acpy = actions[i];
		acpy[mxs[i]] = total;

		model.train(p_states[i], acpy, 0.0001);

		actions[i] = model(c_states[i]);

		mxs[i] = agents[i]->apply_action(delta);
		
		std::string str = "\n" + std::to_string(agents[i]->get_velocity());

		String text = str.c_str();

		texts[i]->set_text(text);
	}
}

nlohmann::json json;

std::ifstream config("config.json");
void Master::_ready()
{
	// Loading run configuration [new function]
	config >> json;

	rows = json["Rows"];
	cols = json["Columns"];

	// Create folder [new function]
	system("mkdir -p results");

	auto t = time(nullptr);
	auto lt = *std::localtime(&t);

	std::ostringstream oss;

	oss << "run_" << std::put_time(&lt, "%m-%d-%Y:%H:%M:%S");

	dir = oss.str();

	std::string cmd = "mkdir results/" + dir;

	system(cmd.c_str());

	std::string fpath = "results/" + dir + "/main";

	mout.open(fpath);
	mout << "episode,average" << std::endl;

	// Hard code for now, read from a file later
	// rows = 5;
	// cols = 5;

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

			texts.push_back(tedit);

			std::string str = "\n0";

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
