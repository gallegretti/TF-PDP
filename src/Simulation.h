#pragma once
#include <random>
#include <vector>

struct vec2f {
	vec2f() = default;
	vec2f(float x, float y) : x(x), y(y)
	{
	}
	float x;
	float y;
};

struct EntityActionResult {
	vec2f movement;
	bool divide;
};

class Simulation
{
public:

	Simulation(size_t n, int n_iterations, int seed);

	// Each entity
	//  Decide movement, update velocity and position
	// Each entity
	//  If size > x, divide

	EntityActionResult update_entity();

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
	std::vector<vec2f> positions;
	std::vector<vec2f> velocities;
	std::vector<float> mass;
	std::vector<float> sizes;

	int n_iterations;

	static constexpr float map_size = 1024;

	static constexpr float maximum_velocity = 1.0f;

	static constexpr float maximum_size = 10.0f;
};
