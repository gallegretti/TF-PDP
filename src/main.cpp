#include <thread>
#include <optional>
#include "ThirdParty/remotery/Remotery.h"
#include "ThirdParty/easylogging/easylogging/easylogging++.h"
INITIALIZE_EASYLOGGINGPP
#include "Visualization.h"
#include "Simulation.h"
#include "Settings.h"

int main(int argc, char* argv[])
{
	// Initialize
	LOG(INFO) << "Started";
	Remotery* rmt;
	rmt_CreateGlobalInstance(&rmt);
	_rmt_SetCurrentThreadName("Main");

	// Read args
	Settings settings(argc, argv);

	// Start simulation
	Simulation simulation(settings.n_start_agents, settings.n_maximum_agents, settings.n_iterations, settings.seed);
	std::thread simulation_thread([&]() {
		simulation.run();
	});

	// Start visualization
	std::optional<std::thread> visualization;
	if (!settings.is_headless)
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
	LOG(INFO) << "Finished";
	return 0;
}
