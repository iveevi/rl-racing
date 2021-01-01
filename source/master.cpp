#include <master.hpp>

// Godot related functions
namespace godot {

Master::Master()
{
	auto initializer = []() {
		return 0.5 - (rand()/(double) RAND_MAX);
	};

	// Load from JSON file later
	model = ml::NeuralNetwork <double> ({
		{11, new ml::Linear <double> ()},
		{10, new ml::Sigmoid <double> ()},
		{10, new ml::ReLU <double> ()},
		{10, new ml::Sigmoid <double> ()},
		{6, new ml::Linear <double> ()}
	}, initializer);

	cost = new ml::MeanSquaredError <double> ();

	model.randomize();
	model.set_cost(cost);

	target = model;
}

Master::~Master()
{
	delete cost;
}

void Master::_init() {}

Vector <double> acpy;

bool launch_graphs = false;

int c_episode = 1;
double avg_reward = 0;
double avg_epsilon = 0;

std::ofstream mout;

size_t threads = 1;
void Master::_process(float delta)
{
	if (!launch_graphs) {
		launch_graphs = true;

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
				avg_epsilon += epsilons[i].front();
				rewards[i].pop();
				epsilons[i].pop();
			}
		} else {
			c_done = false;
		}
	}

	if (c_done) {
		for (size_t i = 0; i < size; i++)
			flushed[i] = false;

		avg_reward /= size;
		avg_epsilon /= size;

		mout << c_episode << "," << avg_reward << "," << avg_epsilon << std::endl;

		c_episode++;
		avg_reward = 0;
		avg_epsilon = 0;
		
		if (c_episode % 50 == 0)
			target = model;
	}

	size_t mi;
	// Separate loops or single loop?
	
	if (threads <= 1) {
		for (size_t i = 0; i < size; i++) {
			agents[i]->cache_state();

			r_deltas[i] = agents[i]->reward_delta();

			size_t imax = model(c_states[i]).imax();

			double total = r_deltas[i] + Agent::lambda * target(c_states[i])[imax];

			agents[i]->buffer_rewards[agents[i]->buffer_index] = total;

			// Method
			agents[i]->buffer_index = (agents[i]->buffer_index + 1) % Agent::buffer_size;

			/* acpy = actions[i];
			acpy[mxs[i]] = total; */
			actions[i][mxs[i]] = total;
		}

		// Train the model on the previous state (critical section)
		model.train <10> (p_states, actions, 0.00001);

		// Generate the next actions
		for (size_t i = 0; i < size; i++) {
			actions[i] = model(c_states[i]);

			mxs[i] = agents[i]->apply_action(delta);
		}
	} else {
		// Instead of multithreading like this, only multithread the
		// model computation

		// Define outside
		auto t1 = [&](size_t offset) {
			for (size_t i = offset; i < size; i += threads) {
				agents[i]->cache_state();

				r_deltas[i] = agents[i]->reward_delta();

				double total = r_deltas[i] + Agent::lambda * model.compute_no_cache(c_states[i]).max();

				agents[i]->set_buffer_reward(total);

				// Method
				agents[i]->increment_buffer_index();

				actions[i][mxs[i]] = total;
			}
		};

		auto t2 = [&](size_t offset) {
			// Generate the next actions
			for (size_t i = offset; i < size; i += threads) {
				actions[i] = model.compute_no_cache(c_states[i]);
			}
		};

		std::vector <std::thread> army;

		for (int i = 0; i < threads; i++)
			army.push_back(std::thread(t1, i));

		for (int i = 0; i < threads; i++)
			army[i].join();

		// Train the model on the previous state (critical section)
		model.train <10> (p_states, actions, 0.00001);

		army.clear();
		for (int i = 0; i < threads; i++)
			army.push_back(std::thread(t2, i));

		for (int i = 0; i < threads; i++)
			army[i].join();

		for (size_t i = 0; i < size; i++)
			mxs[i] = agents[i]->apply_action(delta);
	}
}

nlohmann::json json;

std::ifstream config("config.json");
void Master::_ready()
{
	// Loading run configuration [new function]
	config >> json;

	rows = json["Grid"]["Rows"];
	cols = json["Grid"]["Columns"];
	threads = json["Execution"]["Threads"];

	Agent::buffer_size = json["Buffer Size"];

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
	mout << "episode,average,epsilon" << std::endl;

	ResourceLoader *rl = ResourceLoader::get_singleton();

	Ref <PackedScene> ref = rl->load(p_track);

	double xoff = 0;
	double yoff = 0;
	for (int i = 0; i < rows; i++) {
		yoff = 0;

		for (int j = 0; j < cols; j++) {
			Node *node = ref->instance();

			Node2D *track = Object::cast_to <Node2D> (node);

			track->set_position(Vector2(xoff, yoff));

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
