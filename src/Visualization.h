#pragma once
#include <thread>
#include <SFML/Graphics.hpp>
#include "Simulation.h"

class Visualization
{
public:

	Visualization(Simulation* simulation);

	void run();

	void render_visualization();

private:
	Simulation* simulation;

	sf::RenderWindow window;
};