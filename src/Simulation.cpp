#include "Simulation.h"

Simulation::Simulation(size_t start_n_agents, size_t maximum_agents, int n_iterations, int seed) :
	states(std::vector<std::atomic<State>>(maximum_agents))
{
	_rmt_SetCurrentThreadName("Simulation");
	this->n_iterations = n_iterations;
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
	mass.reserve(start_n_agents);

	for (size_t i = 0; i < start_n_agents; i++)
	{
		movements.push_back(vec2f(0.1f, 0.1f));
		positions.push_back(vec2f(map_distribution(generator), map_distribution(generator)));
		mass.push_back(mass_distribution(generator));
		states[i].store(State::Incubating);
	}

	for (size_t i = start_n_agents; i < maximum_agents; i++)
	{
		states[i].store(State::Dead);
	}
}

void Simulation::run()
{
	for (int i = 0; i < n_iterations / 128; i++)
	{
		// Only lock/unlock mutex every few simulation steps reduce the lock overhead
		// TODO: Make mutex optional if headless
		LOG(INFO) << "Simulation waiting for lock";
		rendering.lock();
		LOG(INFO) << "Simulation got lock";
		for (int j = 0; j < 128; j++)
		{
			step(0.1f);
		}
		rendering.unlock();
		LOG(INFO) << "Simulation released lock";
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
	for (int i = 0; i < positions.size(); i++)
	{
		switch (states[i].load())
		{
		case State::Incubating:
			if (mass[i] >= hunting_mass)
				states[i].store(State::Hunting);
			break;

		case State::Hunting:
			if (mass[i] <= incubating_mass)
				states[i].store(State::Incubating);
			else if (mass[i] >= splitting_mass)
				states[i].store(State::Splitting);
			break;

		case State::Splitting:
			if (mass[i] >= hunting_mass)
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
			mass[i] += mass_cost;
			break;

		case State::Hunting:
			// Update acceleration
			mass[i] -= mass_cost;
			break;

		case State::Splitting:
			// Spawn new agent
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
	// TODO: Parallel
	for (int i = 0; i < positions.size(); i++)
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
