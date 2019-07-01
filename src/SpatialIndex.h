#pragma once
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

	// Agent 'index' moved from old_position to new_position
	void moved(size_t index, vec2f old_position, vec2f new_position);

	int chunk_index(vec2f position);

	int divisions_per_dimension;

	float chunk_size;

	float map_size;

private:

	std::mutex critial_region;

	// Chunks in the index. Each chunk has a vector of agents
	std::vector<std::vector<size_t>> chunks;
};