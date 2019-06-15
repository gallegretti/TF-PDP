#include <thread>
#include <optional>
#include "ThirdParty/remotery/Remotery.h"
#include "ThirdParty/easylogging/easylogging/easylogging++.h"
INITIALIZE_EASYLOGGINGPP
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
	
	LOG(INFO) << "Started";
	// Initialize profiller
	Remotery* rmt;
	rmt_CreateGlobalInstance(&rmt);
	_rmt_SetCurrentThreadName("Main");

	// Start simulation
	Simulation simulation(1024, iterations, seed);
	std::thread simulation_thread([&]() {
		simulation.run();
	});

	// Start visualization
	std::optional<std::thread> visualization;
	if (has_visualization)
	{
		visualization = std::thread([&]() {
			Visualization visualization(&simulation);
			visualization.run();
		});
	}

	// Wait for simulation to end
	simulation_thread.join();
	if (visualization.has_value())
	{
		visualization->join();
	}

	// Destroy the main instance of Remotery.
	rmt_DestroyGlobalInstance(rmt);
	return 0;
}
