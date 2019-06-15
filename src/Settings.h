#pragma once
#include "ThirdParty/args/args.hxx"

class Settings
{
public:
	Settings(int argc, char* argv[])
	{
		args::ArgumentParser parser("Trabalho final de PDP");
		args::Group optional(parser, "", args::Group::Validators::DontCare);
		args::ValueFlag<int> seed(optional, "seed", "Seed for the RNG", { 's', "seed" }, 123456);
		args::ValueFlag<int> threads(optional, "threads", "Maximum number of threads to run", { 't', "threads" }, 1);
		args::ValueFlag<int> iterations(optional, "iterations", "Number of iterations to simulate", { 'i', "iterations" }, 1000000);
		args::Flag headless(optional, "headless", "Should run the simulation without the visualization", { 'h', "headless" });
		parser.ParseCLI(argc, argv);

		this->is_headless = headless.Get();
		this->seed = seed.Get();
		this->n_threads = threads.Get();
		this->n_iterations = iterations.Get();
	}

	bool is_headless;
	int seed;
	int n_threads;
	int n_iterations;
};
