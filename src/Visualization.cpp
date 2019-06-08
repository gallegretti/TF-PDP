#include "Visualization.h"

Visualization::Visualization(Simulation* simulation) : simulation(simulation)
{
}

void Visualization::run()
{
	sf::Window window(sf::VideoMode(800, 600), "PALS");
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
	}
}
