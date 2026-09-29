#pragma once

#include <vector>
#include "../Render/VBO.h"
#include "TerrainGenerator.h"


class TerrainChunk
{
public:
	TerrainChunk(const TerrainGenerator& generator, int chunkX, int chunkZ);

	void GenerateMesh(const TerrainGenerator& generator);

	const std::vector<Vertex>& GetVertices() const { return _vertices; }
	const std::vector<GLuint>& GetIndices() const { return _indices; }

	static int GetChunkSize() { return CHUNK_SIZE; }

private:
	std::vector<Vertex> _vertices;
	std::vector<GLuint> _indices;

	constexpr static int CHUNK_SIZE = 16;

	int _chunkX;
	int _chunkZ;

};