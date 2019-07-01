#include "Simulation.h"

// size_t start_n_agents, size_t maximum_agents, int n_iterations, int seed, bool has_visualization
Simulation::Simulation(Settings settings) :
	states(std::vector<std::atomic<State>>(settings.n_maximum_agents)),
	n_iterations(settings.n_iterations),
	has_visualization(!settings.is_headless),
	n_threads(settings.n_threads),
	spatial_index(map_size)
{
	std::default_random_engine generator;
	generator.seed(settings.seed);

	std::uniform_real_distribution<float> map_distribution(
		-map_size + splitting_mass,
		map_size - splitting_mass
	);

	std::uniform_real_distribution<float> mass_distribution(
		0.1f,
		1.0f
	);

	movements.reserve(settings.n_start_agents);
	positions.reserve(settings.n_start_agents);
	masses.reserve(settings.n_start_agents);

	for (size_t i = 0; i < settings.n_start_agents; i++)
	{
		vec2f position(map_distribution(generator), map_distribution(generator));
		positions.push_back(position);
		spatial_index.set(i, position);
		movements.push_back(vec2f(0.1f, 0.1f));
		masses.push_back(mass_distribution(generator));
		states[i].store(State::Incubating);
	}

	last_agent_index = settings.n_start_agents;

	for (size_t i = settings.n_start_agents; i < settings.n_maximum_agents; i++)
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
	#pragma omp parallel for num_threads(n_threads)
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
	#pragma omp parallel for num_threads(n_threads)
	for (int i = 0; i < last_agent_index; i++)
	{
		vec2f& position = positions[i];
		vec2f& movement = movements[i];
		vec2f old_position = position;

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

		// Update index
		spatial_index.moved(i, old_position, position);
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