#include "Simulation.h"

Simulation::Simulation(size_t n, int n_iterations, int seed) :
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

	accelerations.reserve(n);
	positions.reserve(n);
	velocities.reserve(n);
	mass.reserve(n);

	for (size_t i = 0; i < n; i++)
	{
		accelerations.push_back(vec2f(0.1f, 0.1f));
		positions.push_back(vec2f(map_distribution(generator), map_distribution(generator)));
		velocities.push_back(vec2f(0.0f, 0.0f));
		mass.push_back(mass_distribution(generator));
		states[i].store(State::Incubating);
	}

	for (size_t i = n; i < maximum_agents; i++)
	{
		states[i].store(State::Empty);
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

	// 0: Update state based on current status
	update_states(delta);

	// 1: Think next action based on
	// current position and surroundings
	think_actions(delta);

	// "Barrier"

	// 2: Update position and velocity
	// based on new acceleration
	update_positions(delta);

	// "Barrier"

	// TODO:
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
	}
}

void Simulation::think_actions(float delta)
{
	for (int i = 0; i < positions.size(); i++)
	{
		switch (states[i].load())
		{
		case State::Incubating:
			mass[i] += mass_cost;
			break;
		case State::Hunting:
			// Update acceleration / position / check collisions
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

void Simulation::update_accelerations(float delta)
{
	// TODO: Code Parallel
}

void Simulation::update_positions(float delta)
{
	// TODO: Parallel
	for (int i = 0; i < positions.size(); i++)
	{
		vec2f& position = positions[i];
		vec2f& velocity = velocities[i];
		vec2f& acceleration = accelerations[i];
		// Handle map collision
		if ((position.x < -map_size) || (position.x > map_size))
		{
			velocity.x = 0.0f;
		}
		if ((position.y < -map_size) || (position.y > map_size))
		{
			velocity.y = 0.0f;
		}

		// Update velocity
		velocity.x += acceleration.x * delta;
		velocity.y += acceleration.y * delta;

		// Limit velocity
		float current_velocity = velocity.squared_length();
		// Avoid calculating square root by comparing with squared maximum velocity
		if (current_velocity > squared_maximum_velocity)
		{
			float factor = 1.0 / maximum_velocity;
			velocity.x *= factor;
			velocity.y *= factor;
		}

		// Update position
		position.x += acceleration.x * delta;
		position.y += acceleration.y * delta;
	}
}
