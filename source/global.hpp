#ifndef GLOBAL_H_
#define GLOBAL_H_

// C++ standard headers
#include <queue>
#include <random>
#include <set>

// Zhetapi headers
#include <network.hpp>
#include <erf.hpp>
#include <vector.hpp>

using namespace zhetapi;

// Experience: (s, a, s', r)
struct experience {
	Vector <float> state;		// Current state (s)
	// Vector <float> action;		// Approximated Q values (a)
	int mx;				// Index of action chosen
	Vector <float> transition;	// Next state (s')
	float reward;			// Reward received (r)
	bool done;			// Last step flag

	static experience def() {
		return {
			Vector <float> (11, 0.0f),
			// Vector <float> (9, 0.0f),
			0,
			Vector <float> (11, 0.0f),
			0,
			false
		};
	}
};

// Buffer
extern experience *replay_buffer;

// Buffer properties
extern size_t rbf_size;
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
extern std::vector <std::queue <float>> rewards;
extern std::vector <std::queue <float>> tds;
extern std::vector <std::queue <float>> epsilons;
extern std::vector <int> episodes;
extern std::vector <bool> flushed;

// Global model
extern ml::NeuralNetwork <float> model;
extern ml::NeuralNetwork <float> target;

// Cost function
extern ml::Erf <float> *cost;

// Running directory
extern std::string dir;

extern std::default_random_engine generator;
extern std::uniform_real_distribution <float> distribution;

size_t base(const Vector <double> &);

#endif
