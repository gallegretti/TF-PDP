#include "Simulation.h"

void Simulation::step(float delta)
{
	// 1: Update position
	for (int i = 0; i < positions.size(); i++)
	{
		vec2f& position = positions[i];
		vec2f& velocity = velocities[i];
		// Handle map collision
		if ((position.x < map_size) || (position.x > map_size))
		{
			velocity.x = 0.0f;
		}
		if ((position.y < map_size) || (position.y > map_size))
		{
			velocity.y = 0.0f;
		}

		// TODO: Decide direction
		vec2f acceleration;

		// Update velocity
		velocity.x += acceleration.x * delta;
		velocity.y += acceleration.y * delta;

		// Limit velocity
		float current_velocity = sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y));
		if (current_velocity > maximum_velocity)
		{
			current_velocity = maximum_velocity;
		}
		float factor = 1.0 / current_velocity;
		velocity.x *= factor;
		velocity.y *= factor;
		
		// Update position
		position.x += acceleration.x * delta;
		position.y += acceleration.y * delta;

	}
}
