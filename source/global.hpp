#ifndef GLOBAL_H_
#define GLOBAL_H_

// C++ standard headers
#include <queue>
#include <random>

// Zhetapi headers
#include <network.hpp>
#include <optimizer.hpp>
#include <vector.hpp>

using namespace zhetapi;

// Experience: (s, a, s', r)
struct experience {
	Vector <double> state;		// Current state (s)
	Vector <double> action;		// Approximated Q values (a)
	int mx;				// Index of action chosen
	Vector <double> transition;	// Next state (s')
	double reward;			// Reward received (r)
	bool done;			// Last step flag

	static experience def() {
		return {
			Vector <double> (11, 0.0),
			Vector <double> (9, 0.0),
			0,
			Vector <double> (11, 0.0),
			0,
			false
		};
	}
};

// Buffer
extern experience *replay_buffer;

// Buffer properties
extern size_t replay_buffer_size;
extern size_t replay_buffer_index;

extern bool full;

// Pushing into the buffer
void push(const experience &);

// Sampling a batch (contiguous)
std::vector <experience> sample_batch(size_t);

//==================================

// Forward declare the agent class
class Agent;

// Path to the track scene
extern const char *p_track;

// Size
extern int size;

// List of all agents
extern std::vector <Agent *> agents;

// Queues and status' for average statistics
extern std::vector <std::queue <double>> rewards;
extern std::vector <std::queue <double>> epsilons;
extern std::vector <int> episodes;
extern std::vector <bool> flushed;

// Global model
extern ml::NeuralNetwork <double> model;
extern ml::NeuralNetwork <double> target;

// Cost function
extern ml::Optimizer <double> *cost;

// Running directory
extern std::string dir;

extern std::default_random_engine generator;
extern std::uniform_real_distribution <double> distribution;

#endif
