#pragma once
#include <easylogging/easylogging++.h>
#include <random>
#include <vector>
#include <atomic>
#include "SpatialIndex.h"
#include "vec2f.h"
#include "Settings.h"

/*
	PALS
		Parallel programming through an
		Artificial
		Life
		Simulation

	Diego Dasso Migotto, Gabriel Allegretti

	About the simulation
		The agents operate in a two-dimensional space
		The agents move at constant speed and can grow in size
		The agents must spend their mass to move
		An agent can incubate in an unmovable state, growing it's mass
		An agent can hunt other agents to prey on them
		An agent can split itself in two when it grows too big

	About the parallellization
		The agents will update it's state, plan their actions and move in parallel
		Collision computing will be done sequentially
*/

// Possible states for each agent
enum class State {
	Incubating,	// Standing still and gaining mass
	Hunting,	// Moving, trying to absorb other agents, spending mass
	Dead		// Dead
};

struct EntityActionResult {
	vec2f movement;
	bool divide;
};

// Contains all simulation logic and data
class Simulation
{
public:

	// Constructor, takes starting number of agents, maximum number of agents,
	// number of iterations, random seed and a flag enabling rendering.
	Simulation(Settings simulation_settings);

	// Simulation main loop, run for the specified amount of steps at simulation construction.
	// Optionally render the simulatino every x steps.
	void run();

	// A step of the simulation updates each agent state and it's position.
	void step(float delta);

	// The simulation data is organized in a data oriented fashion.
	// Each index in the data structures represents an agent.
	// The simulation space is a 2D continuous map centered at (0.0, 0.0).
	std::vector<vec2f> movements;           // Planned movement data.
	std::vector<vec2f> positions;           // Position data.
	std::vector<float> masses;              // Mass data.
	std::vector<size_t> eaten;              // Eaten entity index.
	std::vector<std::atomic<State>> states;	// States data.

	// Numebr of threads the simulation will use
	int n_threads;

	// Number of iterations the simulation will run for.
	int n_iterations;

	// Current higher index for a living agent
	std::mutex last_agent_index_mutex;
	size_t last_agent_index;

	// State of the simulation.
	bool is_done = false;

	// Constant map dimension size.
	// Map goes from [-map_size, +map_size]
	static constexpr float map_size = 512;

	// Threshold for an agent go back to incubating state.
	static constexpr float incubating_mass = 1.0f;

	// Threshold for an agent start hunting.
	static constexpr float hunting_mass = 2.0f;

	// Threshold for an agent to split it self in two.
	static constexpr float splitting_mass = 10.0f;

	// Cost in mass for an agent to move.
	static constexpr float move_mass_cost = 0.01f;

	// Reward in mass for an agent to incubate
	static constexpr float incubate_mass_reward = 0.1f;

	// Maximum distance between two agents so one can eat the other
	static constexpr float max_eat_distance = 1.0f;

private:

	const size_t no_agent = std::numeric_limits<size_t>::max();

	// Spatial index for efficient position-based lookups
	SpatialIndex spatial_index;

	// Update eaten agents.
	void update_eaten_agents(float delta);

	// Update states of all living agents.
	void update_states(float delta);

	// Update position of all living agents;
	void update_positions(float delta);

	// Spawn a new agent at an available position.
	int spawn_agent(vec2f position, float mass, State state);

	// Defines wheter simulation should be rendered.
	bool has_visualization;

	inline void simulate_hunting(size_t index);

	inline void simulate_incubating(size_t index);

	inline void simulate_splitting(size_t index);
};
