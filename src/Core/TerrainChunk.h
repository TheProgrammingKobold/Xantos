#pragma once

#include <vector>
#include "../Render/VBO.h"
#include "TerrainGenerator.h"
#include "../Render/Mesh.h"
#include "../Render/Material.h"
#include "../Render/Renderer.h"


class TerrainChunk
{
public:
	TerrainChunk(const TerrainGenerator& generator, std::shared_ptr<Material> material, int chunkX, int chunkZ);

	void GenerateMesh(const TerrainGenerator& generator);

	void Render(Renderer& renderer) const;

	static int GetChunkSize() { return CHUNK_SIZE; }

private:

	DrawCommand _drawCommand;

	constexpr static int CHUNK_SIZE = 16;

	int _chunkX;
	int _chunkZ;

};