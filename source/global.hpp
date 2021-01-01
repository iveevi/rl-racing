#ifndef GLOBAL_H_
#define GLOBAL_H_

// C++ standard headers
#include <queue>
#include <random>

// Zhetapi headers
#include <network.hpp>
#include <optimizer.hpp>
#include <vector.hpp>

namespace godot {

using namespace zhetapi;

// Forward declare the agent class
class Agent;

// Path to the track scene
extern const char *p_track;

// Size
extern int size;

// List of all agents
extern std::vector <Agent *> agents;

// States per each agent
extern std::vector <Vector <double>> c_states;
extern std::vector <Vector <double>> p_states;

// Reward deltas (per state)
extern std::vector <double> r_deltas;

// Queue of episode results
extern std::vector <std::queue <double>> rewards;
extern std::vector <std::queue <double>> epsilons;
extern std::vector <int> episodes;
extern std::vector <bool> flushed;

// Actions for each agent
extern std::vector <Vector <double>> actions;
extern std::vector <int> mxs;

// Global model
extern ml::NeuralNetwork <double> model;

// Cost function
extern ml::Optimizer <double> *cost;

// Running directory
extern std::string dir;

extern std::default_random_engine generator;
extern std::uniform_real_distribution <double> distribution;

}

#endif
