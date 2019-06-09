#include "Visualization.h"

Visualization::Visualization(Simulation* simulation) : 
	simulation(simulation),
	window(sf::RenderWindow(sf::VideoMode(800, 600), "PALS"))
{
}

void Visualization::run()
{
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		simulation->rendering.lock();
		render_visualization();
		simulation->rendering.unlock();
	}
}

void Visualization::render_visualization()
{
	sf::CircleShape circle;
	for (int i = 0; i < simulation->positions.size(); i++)
	{
		vec2f& agent_position = simulation->positions[i];
		float agent_size = simulation->sizes[i];
		circle.setPosition({ agent_position.x, agent_position.y });
		// TODO: Change color per agent
		circle.setFillColor(sf::Color::Blue);
		circle.setRadius(agent_size);
		window.draw(circle);
	}
}
