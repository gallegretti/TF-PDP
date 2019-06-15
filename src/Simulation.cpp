#include "Simulation.h"

Simulation::Simulation(size_t n, int n_iterations, int seed)
{
	_rmt_SetCurrentThreadName("Simulation");
	this->n_iterations = n_iterations;
	std::default_random_engine generator;
	generator.seed(seed);
	
	std::uniform_real_distribution<float> map_distribution(
		-map_size + maximum_size,
		map_size - maximum_size
	);

	std::uniform_real_distribution<float> size_distribution(
		5.0f,
		10.5f
	);

	std::uniform_real_distribution<float> mass_distribution(
		5.0f,
		10.5f
	);

	is_alive.reserve(n);
	accelerations.reserve(n);
	positions.reserve(n);
	velocities.reserve(n);
	mass.reserve(n);
	sizes.reserve(n);

	for (size_t i = 0; i < n; i++)
	{
		is_alive.push_back(true);
		accelerations.push_back(vec2f(0.1f, 0.1f));
		positions.push_back(vec2f(map_distribution(generator), map_distribution(generator)));
		velocities.push_back(vec2f(0.0f, 0.0f));
		mass.push_back(mass_distribution(generator));
		sizes.push_back(size_distribution(generator));
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

	// 1: Decide acceleration vector based on 
	// current position and surroundings
	update_accelerations(delta);

	// "Barrier"

	// 2: Update position and velocity
	// based on new acceleration
	update_positions(delta);

	// "Barrier"

	// TODO:
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
