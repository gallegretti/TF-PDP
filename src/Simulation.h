#pragma once
#include <Remotery.h>
#include <easylogging/easylogging++.h>
#include <mutex>
#include <random>
#include <vector>
#include "vec2f.h"

struct EntityActionResult {
	vec2f movement;
	bool divide;
};

class Simulation
{
public:

	Simulation(size_t n, int n_iterations, int seed);

	// Lock used by the visualization thread
	std::mutex rendering;

	std::condition_variable cv;

	/*
	Sobre a simulação:
    -Os organismos vivem em um ambiente finito de duas dimensões
    -Um organismo pode se alimentar de outro, caso seja maior e esteja sobre o outro
    -Um organismo pode se alimentar de nutrientes (aparecem aleatoriamente no ambiente)
    -Após atingir certo tamanho, um organismo de divide em dois
    -Com o passar do tempo, organismos gastam energia e perdem tamanho
    -Um organismo pode se mover em qualquer direção
    Sobre a paralelização:
    -Os organismos vão tomar ações, atualizar suas posições e velocidades ao mesmo tempo.
    -Barreira de sincronização
    -Atualização de colisões, reprodução e alimentação
	*/

	void run();

	void step(float delta);

	std::vector<bool> is_alive;
	// The map is centered at (0, 0)
	std::vector<vec2f> accelerations;
	std::vector<vec2f> positions;
	std::vector<vec2f> velocities;
	std::vector<float> mass;
	std::vector<float> sizes;

	int n_iterations;

	bool is_done = false;

	static constexpr float map_size = 1024;

	static constexpr float maximum_velocity = 1.0f;

	static constexpr float squared_maximum_velocity = maximum_velocity * maximum_velocity;

	static constexpr float maximum_size = 10.0f;

private:

	void update_accelerations(float delta);

	void update_positions(float delta);
};
