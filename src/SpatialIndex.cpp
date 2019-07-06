#include "SpatialIndex.h"

SpatialIndex::SpatialIndex(float map_size) : map_size(map_size)
{
	divisions_over_two = divisions_per_dimension / 2;
	chunk_size = (map_size * 2.0f) / divisions_per_dimension;

	for (int i = 0; i < divisions_per_dimension * divisions_per_dimension; i++)
	{
		chunks.push_back(std::vector<size_t>());
	}
}

void SpatialIndex::set(size_t index, vec2f position)
{
	std::scoped_lock(critial_region);
	chunks.at(chunk_index(position)).push_back(index);
}

void SpatialIndex::remove(size_t index, vec2f position)
{
	std::scoped_lock(critial_region);
	int old_chunk_index = chunk_index(position);
	auto& old_chunk = chunks.at(old_chunk_index);
	auto remove_it = std::remove(old_chunk.begin(), old_chunk.end(), index);
	if (remove_it == old_chunk.end())
	{
		LOG(INFO) << "Trying to remove missing index " << index;
		return;
	}
	old_chunk.erase(remove_it);
}

void SpatialIndex::moved(size_t index, vec2f old_position, vec2f new_position)
{
	int old_chunk_index = chunk_index(old_position);
	int new_chunk_index = chunk_index(new_position);

	if (old_chunk_index == new_chunk_index)
	{
		// Chunk is the same, skip
		return;
	}

	{
		std::scoped_lock(critial_region);

		// Remove from old chunk
		auto& old_chunk = chunks.at(old_chunk_index);
		auto remove_it = std::remove(old_chunk.begin(), old_chunk.end(), index);
		if (remove_it == old_chunk.end())
		{
			LOG(INFO) << "Trying to move missing index " << index;
			return;
		}
		old_chunk.erase(remove_it);

		// Add to new chunk
		chunks.at(chunk_index(new_position)).push_back(index);
	}
}

const std::vector<size_t>& SpatialIndex::close_to(vec2f position)
{
	return chunks.at(chunk_index(position));
}

int SpatialIndex::chunk_index(vec2f position)
{
	int x_pos = divisions_over_two + position.x / chunk_size;
	int y_pos = divisions_over_two + position.y / chunk_size;
	return x_pos + y_pos * divisions_per_dimension;
}
