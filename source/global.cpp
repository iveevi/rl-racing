#include <global.hpp>

// Replay buffer variables and operations
pbuffer <experience> q;

size_t replay_buffer_index = 0;
size_t rbf_size = 0;

bool full = false;

void push(const experience &exp)
{
	using namespace std;
	full = (q.size() == rbf_size);

	/*
	cout << "---------------" << endl;
	cout << "rbf_size = " << rbf_size << endl;

	cout << "size = " << q.size() << endl; */

	// cout << "psize = " << q.size() << endl;
	if (full) {
		// cout << "FULL" << endl;
		q.remove_bottom(exp);
	} else {
		q.push(exp);
		// cout << "NOT FULL" << endl;
	}
	/* cout << "prsize = " << q.size() << endl;
	
	cout << "max err = " << q.top().td << endl;
	cout << "pushing td = " << exp.td << endl; */
}

std::vector <experience> sample_batch(size_t batch_size)
{
	std::vector <experience> batch;
	for (size_t i = 0; i < batch_size; i++) {
		batch.push_back(q.top());
		q.pop();
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
std::vector <std::queue <float>> tds;
std::vector <std::queue <float>> epsilons;
std::vector <int> episodes;
std::vector <bool> flushed;

// Size (to be populated)
int size = 0;

// Path to the track scene
const char *p_track = "res://scenes/track3.tscn";

// Model
ml::NeuralNetwork <float> model;
ml::NeuralNetwork <float> target;

// Cost
ml::Erf <float> *cost;

// Directory
std::string dir;

std::default_random_engine generator;
std::uniform_real_distribution <float> distribution(0.0, 1.0);

// Base model
size_t base(Vector <double> x)
{
	size_t dir = 2;
	using namespace std;
	cout << "current = " << x << endl;
	if (x[0] - x[1] > 10)
		dir = 0;
	else if (x[1] - x[0] > 10)
		dir = 1;

	return dir;
}
