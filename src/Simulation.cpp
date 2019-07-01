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

	movements.reserve(settings.n_maximum_agents);
	positions.reserve(settings.n_maximum_agents);
	last_positions.reserve(settings.n_maximum_agents);
	masses.reserve(settings.n_maximum_agents);

	for (size_t i = 0; i < settings.n_start_agents; i++)
	{
		vec2f position(map_distribution(generator), map_distribution(generator));
		positions.push_back(position);
		last_positions.push_back(position);
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
		last_positions.push_back(vec2f(0.0f, 0.0f));
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

	// 2: Update position based on movement
	update_positions(delta);

	// 3: Update spatial index
	update_spatial_index(delta);
}

void Simulation::respawn_agents(float delta)
{

}

void Simulation::update_states(float delta)
{
	#pragma omp parallel for num_threads(n_threads)
	for (int i = 0; i < last_agent_index; i++)
	{
		float& mass = masses[i];
		std::atomic<State>& state = states[i];
		vec2f& position = positions[i];
		vec2f& movement = movements[i];

		// Update last position
		last_positions[i] = position;

		// Change state
		switch (state.load())
		{
		case State::Incubating:
			if (mass >= hunting_mass)
			{
				state.store(State::Hunting);
			}
			break;

		case State::Hunting:
			if (mass <= incubating_mass)
			{
				state.store(State::Incubating);
			}
			else if (masses[i] >= splitting_mass)
			{
				state.store(State::Splitting);
			}
			break;

		case State::Splitting:
			if (mass >= hunting_mass)
			{
				state.store(State::Hunting);
			}
			else
			{
				state.store(State::Incubating);
			}
			break;

		case State::Dead:
			break;

		default:
			break;
		}

		// Take action
		switch (state.load())
		{
		case State::Incubating:
			simulate_incubating(i);
			break;

		case State::Hunting:
			simulate_hunting(i);
			break;

		case State::Splitting:
			simulate_splitting(i);
			break;

		case State::Dead:
			break;

		default:
			break;
		}
	}
}

inline void Simulation::simulate_hunting(size_t index)
{
	vec2f& position = positions[index];
	vec2f& movement = movements[index];
	float& mass = masses[index];

	// Move to the closer incubating agent
	const std::vector<size_t>& nearby = spatial_index.close_to(position);

	size_t closer_agent = 0;
	float closer_distance = 0.0f;
	for (size_t agent : nearby)
	{
		if (agent == index)
		{
			continue;
		}
		// TODO: Break if close enough
		float distance = vec2f::squared_distance(position, positions[agent]);
		if (distance < closer_distance)
		{
			closer_agent = agent;
			closer_distance = distance;
		}
	}

	vec2f destination_pos = positions[closer_agent];
	movement = (position - destination_pos);
	movement.normalize();

	mass -= move_mass_cost;
}

inline void Simulation::simulate_incubating(size_t index)
{
	vec2f& movement = movements[index];
	float& mass = masses[index];
	movement = vec2f(0.0f, 0.0f);
	mass += incubate_mass_reward;
}

inline void Simulation::simulate_splitting(size_t index)
{
	vec2f& position = positions[index];
	float& mass = masses[index];

	spawn_agent(
		vec2f(position.x + 1.0f, position.y + 1.0f),
		mass / 2.0f,
		State::Splitting);
	position.x -= 1.0f;
	position.y -= 1.0f;
	mass = mass / 2.0f;
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

void Simulation::update_spatial_index(float delta)
{
	#pragma omp parallel for num_threads(n_threads)
	for (int i = 0; i < last_positions.size(); i++)
	{
		spatial_index.moved(i, last_positions[i], positions[i]);
	}
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
