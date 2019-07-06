#pragma once
#include <easylogging/easylogging++.h>
#include <mutex>
#include <algorithm>
#include <vector>
#include <array>
#include "vec2f.h"

// Thread-safe spatial index
class SpatialIndex
{
public:

	SpatialIndex(float map_size);

	// New agent 'index' is at position
	void set(size_t index, vec2f position);

	// Remove agent 'index'
	void remove(size_t index, vec2f position);

	// Agent 'index' moved from old_position to new_position
	void moved(size_t index, vec2f old_position, vec2f new_position);

	const std::vector<size_t>& close_to(vec2f position);

	int chunk_index(vec2f position);

	static constexpr int divisions_per_dimension = 16;

	int divisions_over_two;

	float chunk_size;

	float map_size;

private:

	std::mutex critial_region;

	// Chunks in the index. Each chunk has a vector of agents
	std::vector<std::vector<size_t>> chunks;
};