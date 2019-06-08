#pragma once
#include <SFML/Window.hpp>
#include "Simulation.h"

class Visualization
{
public:

	Visualization(Simulation* simulation);

	void run();

private:
	Simulation* simulation;
};