// Profiler
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
	// Initialize profiller
	Remotery* rmt;
	rmt_CreateGlobalInstance(&rmt);

	// Start simulation
	Simulation simulation(1024, 10000000, seed);
	simulation.run();

	// Start visualization
	if (has_visualization)
	{
		Visualization visualization(&simulation);
		visualization.run();
	}

	// Destroy the main instance of Remotery.
	rmt_DestroyGlobalInstance(rmt);
	return 0;
}
