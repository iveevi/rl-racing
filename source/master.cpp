#include <master.hpp>

Master::Master()
{
	auto initializer = []() {
		return 0.5 - (rand()/(float) RAND_MAX);
	};

	// Load from JSON file later
	model = ml::NeuralNetwork <float> (11, {
		ml::Layer {10, new ml::Sigmoid <float> ()},
		ml::Layer{10, new ml::Sigmoid <float> ()},
		ml::Layer{6, new ml::Linear <float> ()}
	});

	cost = new ml::MeanSquaredError <float> ();

	ml::Optimizer <float> *opt = new ml::Nesterov <float> (0.05);

	model.set_cost(cost);
	model.set_optimizer(opt);

	target = model;
}

Master::~Master()
{
	delete cost;
}

void Master::_init() {}

Vector <float> acpy;

bool launch_graphs = false;

int c_episode = 1;
float avg_reward = 0;
float avg_epsilon = 0;
float avg_td = 0;

std::ofstream mout;

size_t threads = 1;

size_t batch_size;

double lr = 0;
void Master::_process(float delta)
{
	// Put inside a method
	if (!launch_graphs) {
		launch_graphs = true;

		std::string cmd;

		cmd = "python show_agents.py " + dir + " " + std::to_string(size) + " &";

		system(cmd.c_str());
		
		cmd = "python show_average.py " + dir + " &";
		
		system(cmd.c_str());
	}

	// Method
	bool c_done = true;
	for (size_t i = 0; i < size; i++) {
		if (episodes[i] > c_episode) {
			if (!flushed[i]) {
				flushed[i] = true;

				avg_reward += rewards[i].front();
				avg_epsilon += epsilons[i].front();
				avg_td += tds[i].front();

				rewards[i].pop();
				epsilons[i].pop();
				tds[i].pop();
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
		avg_td /= size;

		mout << c_episode << "," << avg_reward << "," << avg_epsilon << "," << avg_td << std::endl;

		c_episode++;
		avg_reward = 0;
		avg_epsilon = 0;
		avg_td = 0;

		if (c_episode % 100 == 0)
			target = model;
	}

	// Real computation (Q learning loop)
	for (size_t i = 0; i < size; i++)
		agents[i]->step(delta);

	if (full) {
		std::vector <experience> batch = sample_batch(batch_size);

		DataSet <float> ins;
		DataSet <float> outs;
		for (auto e : batch) {
			ins.push_back(e.state);

			float rt = e.reward;

			if (!e.done) {
				size_t mx = model(e.transition).imax();

				rt += Agent::lambda * target(e.transition)[mx];
			}

			Vector <float> action = model(e.state);

			action[e.mx] = rt;

			outs.push_back(action);
		}

		model.fit(ins, outs);
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

	batch_size = json["Experience Batch Size"];

	rbf_size = json["Replay Buffer Size"];
	replay_buffer_index = 0;

	replay_buffer = new experience[rbf_size];

	Agent::deps = json["Exploration"]["Delta"];

	lr = json["Alpha"];

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
	mout << "episode,average,epsilon,td" << std::endl;

	ResourceLoader *rl = ResourceLoader::get_singleton();

	Ref <PackedScene> ref = rl->load(p_track);

	float xoff = 0;
	float yoff = 0;
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

	float cx = xoff + 10240;
	float cy = 7000 * cols + 6000;

	float z = std::max(10 * cx/10240, 10 * cy/6000);

	Node *node = get_parent()->get_node("cells");

	Camera2D *cam = Object::cast_to <Camera2D> (node);

	cam->set_zoom(Vector2(z, z));
	cam->set_global_position(Vector2(
				xoff/2 - 5000,
				3500 * cols - 5000
	));
}
