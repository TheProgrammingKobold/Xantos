#include "TerrainChunk.h"

#include  <glm/gtc/matrix_transform.hpp>

TerrainChunk::TerrainChunk(const TerrainGenerator& generator, std::shared_ptr<Material> material, int chunkX, int chunkZ)
	: _chunkX(chunkX), _chunkZ(chunkZ)
{
	_drawCommand.material = std::move(material);
	_drawCommand.transform = glm::translate(glm::mat4(1.0f), glm::vec3(_chunkX * CHUNK_SIZE, 0.0f, _chunkZ * CHUNK_SIZE));
	GenerateMesh(generator);
}

void TerrainChunk::GenerateMesh(const TerrainGenerator& generator)
{
	std::vector<Vertex> _vertices;
	std::vector<GLuint> _indices;

	float minTerrainHeight = 0.0f;
	float maxTerrainHeight = 0.0f;

	for (int z = 0; z <= CHUNK_SIZE; ++z)
	{
		for (int x = 0; x <= CHUNK_SIZE; ++x)
		{
			int worldX = _chunkX * CHUNK_SIZE + x;
			int worldZ = _chunkZ * CHUNK_SIZE + z;
			float height = generator.GetHeight(worldX, worldZ);
			glm::vec3 normal = glm::normalize(glm::vec3(
				generator.GetHeight(worldX - 1, worldZ) - generator.GetHeight(worldX + 1, worldZ),
				2.0f,
				generator.GetHeight(worldX, worldZ - 1) - generator.GetHeight(worldX, worldZ + 1)
			));
			glm::vec2 texUV(
				static_cast<float>(x),
				static_cast<float>(z)
			);
			_vertices.push_back({ glm::vec3(static_cast<float>(x), height, static_cast<float>(z)), normal, glm::vec3(1.0f), texUV });

			if (minTerrainHeight > height)
				minTerrainHeight = height;
			if (maxTerrainHeight < height)
				maxTerrainHeight = height;
		}
	}

	_aabb = { {_chunkX * CHUNK_SIZE, minTerrainHeight, _chunkZ * CHUNK_SIZE}, {(_chunkX + 1) * CHUNK_SIZE, maxTerrainHeight, (_chunkZ + 1) * CHUNK_SIZE} };

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

	_drawCommand.mesh = std::make_shared<Mesh>(_vertices, _indices);
}
