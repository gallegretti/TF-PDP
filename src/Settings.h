#pragma once
#include "ThirdParty/args/args.hxx"

class Settings
{
public:
	Settings(int argc, char* argv[]);

	bool debug;
	bool is_headless;
	int seed;
	int n_threads;
	int n_iterations;
	int n_start_agents;
	int n_maximum_agents;
};
