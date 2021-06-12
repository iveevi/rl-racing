#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <RichTextLabel.hpp>

// C++ standard headers
#include <queue>
#include <random>
#include <set>

// Zhetapi headers
#include <dnn.hpp>
#include <erf.hpp>
#include <vector.hpp>
#include <training.hpp>
#include <std/optimizers.hpp>

// Debugging
#define pass_check(msg, code)			\
	std::cout << msg << std::endl;		\
	code					\
	std::cout << "\tPassed." << std::endl;

using namespace zhetapi;
using namespace godot;

// Experience: (s, a, s', r)
struct experience {
	Vector <float> state;		// Current state (s)
	int mx;				// Index of action chosen
	Vector <float> transition;	// Next state (s')
	float reward;			// Reward received (r)
	float td;			// TD error
	bool done;			// Last step flag

	bool operator<(const experience &a) const {
		return td < a.td;
	}

	bool operator>(const experience &a) const {
		return td > a.td;
	}
};

template <class T>
class pbuffer : public std::priority_queue <T, std::vector <T>> {
public:
	void remove_bottom(const experience &exp) {
		auto it_min = std::min_element(this->c.begin(), this->c.end());

		if (it_min->td < exp.td) {
			*it_min = exp;

			std::make_heap(this->c.begin(), this->c.end(), this->comp);
		}
	}

	// Add random access later
};


// Buffer
extern pbuffer <experience> q;

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

extern RichTextLabel *rel;

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
extern ml::DNN <float> model;
extern ml::DNN <float> target;

// Cost function
extern ml::Erf <float> *cost;
extern ml::Adam <float> *opt;

// Running directory
extern std::string dir;

extern std::default_random_engine generator;
extern std::uniform_real_distribution <float> distribution;

size_t base(Vector <double>);

#endif
