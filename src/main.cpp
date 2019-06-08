// Logging
//#include "ThirdParty/easylogging/easylogging/easylogging++.h"
//INITIALIZE_EASYLOGGINGPP
#include <SFML/Window.hpp>

// Profiler
#include "ThirdParty/remotery/Remotery.h"

#include "Simulation.h"


int main(int argc, char* argv[])
{
	// Initialize profiller
	Remotery* rmt;
	rmt_CreateGlobalInstance(&rmt);

	sf::Window window(sf::VideoMode(800, 600), "My window");
	
	Simulation simulation;

	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}
	}

	// Destroy the main instance of Remotery.
	rmt_DestroyGlobalInstance(rmt);
	//LOG(INFO) << "Successful shutdown";
	return 0;
}
