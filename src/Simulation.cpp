#include "Simulation.h"

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
	masses.reserve(settings.n_maximum_agents);
	eaten.reserve(settings.n_maximum_agents);

	for (size_t i = 0; i < settings.n_start_agents; i++)
	{
		vec2f position(map_distribution(generator), map_distribution(generator));
		positions.push_back(position);
		spatial_index.set(i, position);

		movements.push_back(vec2f(0.1f, 0.1f));
		masses.push_back(mass_distribution(generator));
		eaten.push_back(std::numeric_limits<size_t>::max());
		states[i].store(State::Incubating);
	}

	last_agent_index = settings.n_start_agents;

	for (size_t i = settings.n_start_agents; i < settings.n_maximum_agents; i++)
	{
		movements.push_back(vec2f(0.0f, 0.0f));
		positions.push_back(vec2f(0.0f, 0.0f));
		masses.push_back(0.0f);
		eaten.push_back(std::numeric_limits<size_t>::max());
		states[i].store(State::Dead);
	}
}

void Simulation::run()
{
	// Only lock/unlock mutex every few simulation steps reduce the lock overhead
	for (int i = 0; i < n_iterations / 128; i++)
	{
		for (int j = 0; j < 128; j++)
		{
			step(0.05f);
		}
	}
	is_done = true;
}

void Simulation::step(float delta)
{
	// 0: Update eaten agents on last step
	update_eaten_agents(delta);

	// 1: Update state based on current status
	update_states(delta);

	// 2: Update position based on movement and update spatial index
	update_positions(delta);
}

void Simulation::update_eaten_agents(float delta)
{
	#pragma omp parallel for num_threads(n_threads)
	for (int i = 0; i < last_agent_index; i++)
	{
		size_t eaten_index = eaten[i];
		if (eaten_index != no_agent)
		{
			State incubating = State::Incubating;
			// Two agents could try to eat the same incubating agent at once. Only one will get it
			if (states[eaten_index].compare_exchange_strong(incubating, State::Dead))
			{
				masses[i] += masses[eaten_index];
				spatial_index.remove(eaten_index, positions[eaten_index]);
			}
			eaten[i] = no_agent;
		}
	}
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

		// Update state and take action
		switch (state.load())
		{
		case State::Incubating:
			if (mass >= hunting_mass)
			{
				state.store(State::Hunting);
				simulate_hunting(i);
			}
			else
			{
				simulate_incubating(i);
			}
			break;

		case State::Hunting:
			if (mass <= incubating_mass)
			{
				state.store(State::Incubating);
				simulate_incubating(i);
			}
			else if (masses[i] >= splitting_mass)
			{
				simulate_splitting(i);
			}
			else
			{
				simulate_hunting(i);
			}
			break;

		case State::Splitting:
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

	// No one else nearby
	if (nearby.empty() || (nearby.size() == 1 && nearby[0] == index))
	{
		movement = vec2f(0.0f, 0.0f);
		return;
	}

	size_t closer_agent_index = no_agent;
	float closer_distance_squared = std::numeric_limits<float>::infinity();
	for (size_t agent : nearby)
	{
		if (agent == index || states[agent] != State::Incubating)
		{
			continue;
		}
		float squared_distance = vec2f::squared_distance(position, positions[agent]);
		if (squared_distance < closer_distance_squared)
		{
			closer_agent_index = agent;
			closer_distance_squared = squared_distance;
		}
	}
	
	mass -= move_mass_cost;

	// No target nearby
	if (closer_agent_index == no_agent)
	{
		movement = vec2f(0.0f, 0.0f);
		return;
	}

	// 'Eat' incubating agent if close enough
	if (closer_distance_squared < (max_eat_distance * max_eat_distance))
	{
		eaten[index] = closer_agent_index;
		movement = vec2f(0.0f, 0.0f);
		return;
	}

	vec2f destination_pos = positions[closer_agent_index];
	movement = (position - destination_pos);
	movement.normalize();
}

inline void Simulation::simulate_incubating(size_t index)
{
	movements[index] = vec2f(0.0f, 0.0f);
	masses[index] += incubate_mass_reward;
}

inline void Simulation::simulate_splitting(size_t index)
{
	float& mass = masses[index];
	mass = mass / 2.0f;
	vec2f new_position = positions[index] + vec2f(1.0f, 1.0f);
	spawn_agent(
		new_position,
		mass,
		State::Splitting);
}

void Simulation::update_positions(float delta)
{
	#pragma omp parallel for num_threads(n_threads)
	for (int i = 0; i < last_agent_index; i++)
	{
		State current_state = states[i].load();
		if (current_state == State::Hunting)
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
		if (current_state == State::Splitting)
		{
			states[i].store(State::Hunting);
			vec2f& position = positions[i];
			spatial_index.set(i, position);
		}
	}
	
}

int Simulation::spawn_agent(vec2f position, float mass, State state)
{
	for (int i = 0; i < positions.size(); i++)
	{
		State dead = State::Dead;
		if (states[i].compare_exchange_strong(dead, state))
		{
			{
				std::unique_lock<std::mutex>(last_agent_index_mutex);
				if (i > last_agent_index)
				{
					last_agent_index = i;
				}
			}
			// Can't modify spatial index, other threads are reading it
			// or have references to chunks
			positions[i] = position;
			masses[i] = mass;
			return i;
		}
	}

	LOG(WARNING) << "Failed to create new agent";
	return -1;
}
