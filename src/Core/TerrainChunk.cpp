#include "TerrainChunk.h"


TerrainChunk::TerrainChunk(const TerrainGenerator& generator, int chunkX, int chunkZ)
	: _chunkX(chunkX), _chunkZ(chunkZ)
{
	GenerateMesh(generator);
}

void TerrainChunk::GenerateMesh(const TerrainGenerator& generator)
{
	_vertices.clear();
	_indices.clear();
	for (int z = 0; z <= CHUNK_SIZE; ++z)
	{
		for (int x = 0; x <= CHUNK_SIZE; ++x)
		{
			int worldX = _chunkX * CHUNK_SIZE + x;
			int worldZ = _chunkZ * CHUNK_SIZE + z;
			int height = generator.GetHeight(worldX, worldZ);
			glm::vec2 texUV(
				static_cast<float>(x) / CHUNK_SIZE,
				static_cast<float>(z) / CHUNK_SIZE
			);
			_vertices.push_back({ glm::vec3(static_cast<float>(x), static_cast<float>(height), static_cast<float>(z)), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f), texUV });
		}
	}

	for (int z = 0; z < CHUNK_SIZE; ++z)
	{
		for (int x = 0; x < CHUNK_SIZE; ++x)
		{
			GLuint topLeft = static_cast<GLuint>(z * (CHUNK_SIZE + 1) + x);
			GLuint topRight = topLeft + 1;
			GLuint bottomLeft = topLeft + CHUNK_SIZE + 1;
			GLuint bottomRight = bottomLeft + 1;

			_indices.insert(_indices.end(), { topLeft, bottomLeft, topRight, topRight, bottomLeft, bottomRight });
		}
	}
}