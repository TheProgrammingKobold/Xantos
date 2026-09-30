#pragma once

#include <vector>
#include "../Render/VBO.h"
#include "TerrainGenerator.h"
#include "../Render/Mesh.h"
#include "../Render/Material.h"
#include "../Render/Renderer.h"
#include "AABB.h"


class TerrainChunk
{
public:
	TerrainChunk(const TerrainGenerator& generator, std::shared_ptr<Material> material, int chunkX, int chunkZ);

	void GenerateMesh(const TerrainGenerator& generator);

	DrawCommand GetDrawCommand() const { return _drawCommand; }
	AABB GetChunkBounds() const { return _aabb; }

	static int GetChunkSize() { return CHUNK_SIZE; }

private:

	DrawCommand _drawCommand;

	AABB _aabb;

	constexpr static int CHUNK_SIZE = 16;

	int _chunkX;
	int _chunkZ;

};