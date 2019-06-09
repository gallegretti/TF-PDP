#include <thread>
#include "ThirdParty/remotery/Remotery.h"
#include "Visualization.h"
#include "Simulation.h"

// TODO: Pass as args
// --headless
// --seed
// --threads
// --iterations
int main(int argc, char* argv[])
{
	int seed = 123456789;
	bool has_visualization = true;
	int iterations = 1000000;
	// Initialize profiller
	Remotery* rmt;
	rmt_CreateGlobalInstance(&rmt);

	// Start simulation
	Simulation simulation(1024, iterations, seed);
	std::thread simulation_thread([&]() {
		simulation.run();
	});

	// Start visualization
	if (has_visualization)
	{
		std::thread([&]() {
			Visualization visualization(&simulation);
			visualization.run();
		});
	}

	// Wait for simulation to end
	simulation_thread.join();

	// Destroy the main instance of Remotery.
	rmt_DestroyGlobalInstance(rmt);
	return 0;
}
