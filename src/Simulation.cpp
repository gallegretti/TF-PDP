#include "Simulation.h"

Simulation::Simulation(size_t start_n_agents, size_t maximum_agents, int n_iterations, int seed, bool has_visualization) :
	states(std::vector<std::atomic<State>>(maximum_agents)), n_iterations(n_iterations), has_visualization(has_visualization)
{
	_rmt_SetCurrentThreadName("Simulation");
	std::default_random_engine generator;
	generator.seed(seed);

	std::uniform_real_distribution<float> map_distribution(
		-map_size + splitting_mass,
		map_size - splitting_mass
	);

	std::uniform_real_distribution<float> mass_distribution(
		0.1f,
		1.0f
	);

	movements.reserve(start_n_agents);
	positions.reserve(start_n_agents);
	masses.reserve(start_n_agents);

	for (size_t i = 0; i < start_n_agents; i++)
	{
		movements.push_back(vec2f(0.1f, 0.1f));
		positions.push_back(vec2f(map_distribution(generator), map_distribution(generator)));
		masses.push_back(mass_distribution(generator));
		states[i].store(State::Incubating);
	}

	last_agent_index = start_n_agents;

	for (size_t i = start_n_agents; i < maximum_agents; i++)
	{
		movements.push_back(vec2f(0.0f, 0.0f));
		positions.push_back(vec2f(0.0f, 0.0f));
		masses.push_back(0.0f);
		states[i].store(State::Dead);
	}
}

void Simulation::run()
{
	// Only lock/unlock mutex every few simulation steps reduce the lock overhead
	for (int i = 0; i < n_iterations / 128; i++)
	{
		if (has_visualization)
		{
			LOG(DEBUG) << "Simulation waiting for lock";
			rendering.lock();
			LOG(DEBUG) << "Simulation got lock";
		}
		for (int j = 0; j < 128; j++)
		{
			step(0.1f);
		}
		if (has_visualization)
		{
			rendering.unlock();
			LOG(DEBUG) << "Simulation released lock";
		}
	}
	is_done = true;
}

void Simulation::step(float delta)
{
	rmt_ScopedCPUSample(Simulation_Step, 0);

	// 0: Create new agents
	respawn_agents(delta);

	// 1: Update state based on current status
	update_states(delta);

	// "Barrier"

	// 2: Update position based on movement
	update_positions(delta);

	// "Barrier"

	// 3: Calculate collisions
	update_collisions(delta);
}

void Simulation::respawn_agents(float delta)
{

}

void Simulation::update_states(float delta)
{
	rmt_ScopedCPUSample(update_states, 0);
	#pragma omp parallel for
	for (int i = 0; i < last_agent_index; i++)
	{
		switch (states[i].load())
		{
		case State::Incubating:
			if (masses[i] >= hunting_mass)
				states[i].store(State::Hunting);
			break;

		case State::Hunting:
			if (masses[i] <= incubating_mass)
				states[i].store(State::Incubating);
			else if (masses[i] >= splitting_mass)
				states[i].store(State::Splitting);
			break;

		case State::Splitting:
			if (masses[i] >= hunting_mass)
				states[i].store(State::Hunting);
			else
				states[i].store(State::Incubating);
			break;

		case State::Dead:
			break;

		default:
			break;
		}

		switch (states[i].load())
		{
		case State::Incubating:
			movements[i] = vec2f(0.0f, 0.0f);
			masses[i] += mass_cost;
			break;

		case State::Hunting:
			// Update position at random
			masses[i] -= mass_cost;
			break;

		case State::Splitting:
			spawn_agent(
				vec2f(positions[i].x + 1.0f, positions[i].y + 1.0f),
				masses[i] / 2,
				State::Splitting);
			positions[i].x -= 1.0f;
			positions[i].y -= 1.0f;
			masses[i] = masses[i] / 2;
			break;

		case State::Dead:
			break;

		default:
			break;
		}
	}
}

void Simulation::update_positions(float delta)
{
	rmt_ScopedCPUSample(update_positions, 0);
	#pragma omp parallel for
	for (int i = 0; i < last_agent_index; i++)
	{
		vec2f& position = positions[i];
		vec2f& movement = movements[i];

		// Update position
		position.x += movement.x * delta;
		position.y += movement.y * delta;

		// Map collision
		if (position.x < -map_size)
		{
			position.x = -map_size;
		}
		if (position.x > map_size)
		{
			position.x = map_size;
		}
		if (position.y < -map_size)
		{
			position.y = -map_size;
		}
		if (position.y > map_size)
		{
			position.y = map_size;
		}
	}
}

void Simulation::update_collisions(float delta)
{
	// TODO: Code Parallel
}

int Simulation::spawn_agent(vec2f position, float mass, State state)
{
	for (int i = 0; i < positions.size(); i++)
	{
		State dead = State::Dead;
		if (states[i].compare_exchange_strong(dead, state))
		{
			positions[i] = position;
			masses[i] = mass;
			return i;
		}
	}

	LOG(WARNING) << "Failed to create new agent";
	return -1;
}