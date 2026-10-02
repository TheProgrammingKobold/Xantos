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

	void GenerateGreedyMesh(const TerrainGenerator& generator);

	// Here in case I wanna test the old mesh making algorithm against the new one. 
	void GenerateMesh(const TerrainGenerator& generator);


	const DrawCommand& GetDrawCommand() const { return _drawCommand; }
	AABB GetChunkBounds() const { return _aabb; }

	static int GetChunkSize() { return CHUNK_SIZE; }

private:

	void TransformIntoGreedyMesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, const TerrainGenerator& generator);

private:

	DrawCommand _drawCommand;

	AABB _aabb;

	constexpr static int CHUNK_SIZE = 16 * 2;

	int _chunkX;
	int _chunkZ;

};