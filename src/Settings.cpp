#include "Settings.h"

Settings::Settings(int argc, char * argv[])
{
	args::ArgumentParser parser("Trabalho final de PDP");
	args::Group optional(parser, "", args::Group::Validators::DontCare);
	args::ValueFlag<int> start_agents_number(optional, "start-n", "Starting number of agents", { "start-n" }, 1024 * 1024);
	args::ValueFlag<int> maximum_agents_number(optional, "max-n", "Maximum number of agents", { "max-n" }, 1024 * 1024 * 2);
	args::ValueFlag<int> seed(optional, "seed", "Seed for the RNG", { 's', "seed" }, 123456);
	args::ValueFlag<int> threads(optional, "threads", "Maximum number of threads to run", { 't', "threads" }, 1);
	args::ValueFlag<int> iterations(optional, "iterations", "Number of iterations to simulate", { 'i', "iterations" }, 1000);
	args::Flag headless(optional, "headless", "Should run the simulation without the visualization", { 'h', "headless" });
	args::Flag debug(optional, "debug", "Show debug information", { "debug" }, false);
	parser.ParseCLI(argc, argv);

	this->debug = debug.Get();
	this->is_headless = headless.Get();
	this->seed = seed.Get();
	this->n_threads = threads.Get();
	this->n_iterations = iterations.Get();
	this->n_start_agents = start_agents_number.Get();
	this->n_maximum_agents = maximum_agents_number.Get();
}
