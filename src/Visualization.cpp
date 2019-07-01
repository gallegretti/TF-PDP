#include "Visualization.h"

Visualization::Visualization(Simulation* simulation) :
	simulation(simulation),
	window(sf::RenderWindow(sf::VideoMode(800, 600), "PALS"))
{
}

void Visualization::run()
{
	sf::Clock last_frame;
	while (window.isOpen())
	{
		sf::Event event;
		while (true)
		{
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
					return;
				}
			}
			// Keep the window responsive while waiting for
			// the time to draw the next frame
			if (last_frame.getElapsedTime().asMilliseconds() > 100)
			{
				break;
			}
		}

		LOG(DEBUG) << "Visualization waiting for lock";
		// Critical region
		{
			std::scoped_lock lock(simulation->rendering);
			LOG(DEBUG) << "Visualization got lock";
			if (simulation->is_done)
			{
				return;
			}
			// Wait to clear after getting a lock, so the screen always has the last frame
			window.clear();
			render_visualization();
		}

		LOG(DEBUG) << "Visualization released lock";
		window.display();
		last_frame.restart();
	}
}

void Visualization::render_visualization()
{
	sf::CircleShape circle;
	for (int i = 0; i < simulation->positions.size(); i++)
	{
		const vec2f& agent_position = simulation->positions[i];
		const float& agent_size = simulation->masses[i];
		circle.setPosition({ agent_position.x, agent_position.y });
		// TODO: Change color per agent
		circle.setFillColor(sf::Color::Blue);
		circle.setRadius(agent_size);
		window.draw(circle);
	}
}
