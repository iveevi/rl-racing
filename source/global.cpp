#include <global.hpp>

namespace godot {

//--------------------[CONSTANTS]-----------------

// List of all agents
std::vector <Agent *> agents;

// States per each agent (current and previous)
std::vector <Vector <double>> c_states;
DataSet <double> p_states;

// Reward deltas
std::vector <double> r_deltas;

// Actions for each agent
DataSet <double> actions;
std::vector <int> mxs;

std::vector <std::queue <double>> rewards;
std::vector <std::queue <double>> epsilons;
std::vector <int> episodes;
std::vector <bool> flushed;

// Size (to be populated)
int size = 0;

// Path to the track scene
const char *p_track = "res://scenes/track.tscn";

// Model
ml::NeuralNetwork <double> model;
ml::NeuralNetwork <double> target;

// Cost
ml::Optimizer <double> *cost;

// Directory
std::string dir;

std::default_random_engine generator;
std::uniform_real_distribution <double> distribution(0.0, 1.0);

}
