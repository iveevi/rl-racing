#include <global.hpp>

// Replay buffer variables and operations
experience *replay_buffer;

size_t replay_buffer_index = 0;
size_t rbf_size = 0;

bool full = false;

void push(const experience &exp)
{
	replay_buffer[replay_buffer_index] = exp;

	replay_buffer_index = (replay_buffer_index + 1) % rbf_size;

	if (!full && !replay_buffer_index)
		full = true;
}

std::vector <experience> sample_batch(size_t batch_size)
{
	std::vector <experience> batch;

	std::set <int> in;

	while (in.size() < batch_size) {
		size_t i = rbf_size * distribution(generator);

		i %= rbf_size;

		if (in.find(i) == in.end()) {
			batch.push_back(replay_buffer[i]);

			in.insert(in.begin(), i);
		}
	}

	return batch;
}

// List of all agents
std::vector <Agent *> agents;

// States per each agent (current and previous)
std::vector <Vector <float>> c_states;
DataSet <float> p_states;

// Reward deltas
std::vector <float> r_deltas;

// Actions for each agent
DataSet <float> actions;
std::vector <int> mxs;

std::vector <std::queue <float>> rewards;
std::vector <std::queue <float>> epsilons;
std::vector <int> episodes;
std::vector <bool> flushed;

// Size (to be populated)
int size = 0;

// Path to the track scene
const char *p_track = "res://scenes/track1.tscn";

// Model
ml::NeuralNetwork <float> model;
ml::NeuralNetwork <float> target;

// Cost
ml::Erf <float> *cost;

// Directory
std::string dir;

std::default_random_engine generator;
std::uniform_real_distribution <float> distribution(0.0, 1.0);
