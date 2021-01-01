#include <global.hpp>

namespace godot {

//--------------------[CONSTANTS]-----------------

// List of all agents
std::vector <Agent *> agents;

// States per each agent (current and previous)
std::vector <Vector <double>> c_states;
std::vector <Vector <double>> p_states;

// Reward deltas
std::vector <double> r_deltas;

// Actions for each agent
std::vector <Vector <double>> actions;
std::vector <int> mxs;

std::vector <std::queue <double>> rewards;
std::vector <int> episodes;
std::vector <bool> flushed;

// Size (to be populated)
int size = 0;

// Path to the track scene
const char *p_track = "res://scenes/track.tscn";

// Model
ml::NeuralNetwork <double> model;

// Cost
ml::Optimizer <double> *cost;

// Directory
std::string dir;

}
