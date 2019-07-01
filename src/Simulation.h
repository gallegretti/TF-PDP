#pragma once
#include <Remotery.h>
#include <easylogging/easylogging++.h>
#include <mutex>
#include <random>
#include <vector>
#include <atomic>
#include "vec2f.h"

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
	Splitting,	// Splitting itself in two, each with half of it's mass
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
	Simulation(size_t start_n_agents, size_t maximum_agents, int n_iterations, int seed, bool has_visualization);

	// Lock used by the visualization thread
	std::mutex rendering;

	// Simulation main loop, run for the specified amount of steps at simulation construction.
	// Optionally render the simulatino every x steps.
	void run();

	// A step of the simulation updates each agent state and it's position.
	void step(float delta);

	// The simulation data is organized in a data oriented fashion.
	// Each index in the data structures represents an agent.
	// The simulation space is a 2D continuous map centered at (0.0, 0.0).
	std::vector<vec2f> movements;			// Planned movement data.
	std::vector<vec2f> positions;			// Position data.
	std::vector<float> masses;				// Mass data.
	std::vector<std::atomic<State>> states;	// States data.

	// Number of iterations the simulation will run for.
	int n_iterations;

	// Current higher index for a living agent
	size_t last_agent_index;

	// State of the simulation.
	bool is_done = false;

	// Constant map dimension size.
	static constexpr float map_size = 1024;

	// Threshold for an agent go back to incubating state.
	static constexpr float incubating_mass = 1.0f;

	// Threshold for an agent start hunting.
	static constexpr float hunting_mass = 2.0f;

	// Threshold for an agent to split it self in two.
	static constexpr float splitting_mass = 10.0f;

	// Cost in mass for an agent to take an action.
	static constexpr float mass_cost = 0.1f;

private:

	// Respawn a constant amount of dead agents.
	void respawn_agents(float delta);

	// Update states of all living agents.
	void update_states(float delta);

	// Update position of all living agents;
	void update_positions(float delta);

	// Calculate collisions that might have ocurred.
	void update_collisions(float delta);

	// Spawn a new agent at an available position.
	int spawn_agent(vec2f position, float mass, State state);

	// Defines wheter simulation should be rendered.
	bool has_visualization;
};
