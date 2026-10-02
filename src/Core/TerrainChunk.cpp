#include "TerrainChunk.h"

#include  <glm/gtc/matrix_transform.hpp>

TerrainChunk::TerrainChunk(const TerrainGenerator& generator, std::shared_ptr<Material> material, int chunkX, int chunkZ)
	: _chunkX(chunkX), _chunkZ(chunkZ)
{
	_drawCommand.material = std::move(material);
	_drawCommand.transform = glm::translate(glm::mat4(1.0f), glm::vec3(_chunkX * CHUNK_SIZE, 0.0f, _chunkZ * CHUNK_SIZE));
	GenerateGreedyMesh(generator);
}

void TerrainChunk::GenerateGreedyMesh(const TerrainGenerator& generator)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    TransformIntoGreedyMesh(vertices, indices, generator);

    float minTerrainHeight = vertices[0].position.y;
    float maxTerrainHeight = vertices[0].position.y;

    for (const auto& vertex : vertices)
    {
        minTerrainHeight = std::min(minTerrainHeight, vertex.position.y);
        maxTerrainHeight = std::max(maxTerrainHeight, vertex.position.y);
    }

    _aabb = {
        {
            _chunkX * CHUNK_SIZE,
            minTerrainHeight,
            _chunkZ * CHUNK_SIZE
        },
        {
            (_chunkX + 1) * CHUNK_SIZE,
            maxTerrainHeight,
            (_chunkZ + 1) * CHUNK_SIZE
        }
    };

    _drawCommand.mesh =
        std::make_shared<Mesh>(vertices, indices);
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

void TerrainChunk::TransformIntoGreedyMesh(
    std::vector<Vertex>& vertices,
    std::vector<GLuint>& indices,
    const TerrainGenerator& generator)
{
    vertices.clear();
    indices.clear();

    constexpr float EPSILON = 0.0001f;

    const int size = CHUNK_SIZE;
    const int vertexWidth = size + 1;

    // ------------------------------------------------------------
    // Generate height + normal data first.
    // ------------------------------------------------------------

    std::vector<float> heights(vertexWidth * vertexWidth);
    std::vector<glm::vec3> normals(vertexWidth * vertexWidth);

    auto vertexIndex = [vertexWidth](int x, int z)
        {
            return z * vertexWidth + x;
        };

    for (int z = 0; z <= size; ++z)
    {
        for (int x = 0; x <= size; ++x)
        {
            const int worldX = _chunkX * size + x;
            const int worldZ = _chunkZ * size + z;

            const float height =
                generator.GetHeight(worldX, worldZ);

            heights[vertexIndex(x, z)] = height;

            normals[vertexIndex(x, z)] =
                glm::normalize(glm::vec3(
                    generator.GetHeight(worldX - 1, worldZ)
                    - generator.GetHeight(worldX + 1, worldZ),

                    2.0f,

                    generator.GetHeight(worldX, worldZ - 1)
                    - generator.GetHeight(worldX, worldZ + 1)
                ));
        }
    }

    // ------------------------------------------------------------
    // A cell is considered mergeable if its four corners are
    // coplanar.
    //
    // Plane equation:
    //
    //     y = a*x + b*z + c
    //
    // ------------------------------------------------------------

    struct Plane
    {
        float a;
        float b;
        float c;
        bool valid;
    };

    auto getPlane = [&](int x, int z) -> Plane
        {
            const float h00 = heights[vertexIndex(x, z)];
            const float h10 = heights[vertexIndex(x + 1, z)];
            const float h01 = heights[vertexIndex(x, z + 1)];
            const float h11 = heights[vertexIndex(x + 1, z + 1)];

            const float a = h10 - h00;
            const float b = h01 - h00;
            const float c = h00 - a * x - b * z;

            const float expectedH11 =
                a * (x + 1) +
                b * (z + 1) +
                c;

            const bool coplanar =
                std::abs(expectedH11 - h11) < EPSILON;

            return {
                a,
                b,
                c,
                coplanar
            };
        };

    auto samePlane = [&](const Plane& a, const Plane& b)
        {
            if (a.valid != b.valid)
                return false;

            if (!a.valid)
                return false;

            return
                std::abs(a.a - b.a) < EPSILON &&
                std::abs(a.b - b.b) < EPSILON &&
                std::abs(a.c - b.c) < EPSILON;
        };

    // ------------------------------------------------------------
    // Track which cells have already been consumed.
    // ------------------------------------------------------------

    std::vector<bool> used(size * size, false);

    auto cellIndex = [size](int x, int z)
        {
            return z * size + x;
        };

    // ------------------------------------------------------------
    // Add a rectangular terrain patch.
    // ------------------------------------------------------------

    auto addQuad =
        [&](int x0, int z0, int x1, int z1)
        {
            const int startVertex =
                static_cast<int>(vertices.size());

            auto makeVertex =
                [&](int x, int z)
                {
                    return Vertex{
                        glm::vec3(
                            static_cast<float>(x),
                            heights[vertexIndex(x, z)],
                            static_cast<float>(z)
                        ),

                        normals[vertexIndex(x, z)],

                        glm::vec3(1.0f),

                        glm::vec2(
                            static_cast<float>(x),
                            static_cast<float>(z)
                        )
                    };
                };

            // Same winding order as your original mesh.
            vertices.push_back(makeVertex(x0, z0));
            vertices.push_back(makeVertex(x0, z1));
            vertices.push_back(makeVertex(x1, z0));
            vertices.push_back(makeVertex(x1, z1));

            indices.insert(
                indices.end(),
                {
                    static_cast<GLuint>(startVertex + 0),
                    static_cast<GLuint>(startVertex + 1),
                    static_cast<GLuint>(startVertex + 2),

                    static_cast<GLuint>(startVertex + 2),
                    static_cast<GLuint>(startVertex + 1),
                    static_cast<GLuint>(startVertex + 3)
                }
            );
        };

    // ------------------------------------------------------------
    // Greedy rectangle merging.
    // ------------------------------------------------------------

    for (int z = 0; z < size; ++z)
    {
        for (int x = 0; x < size; ++x)
        {
            const int currentCell =
                cellIndex(x, z);

            if (used[currentCell])
                continue;

            const Plane basePlane =
                getPlane(x, z);

            // Non-planar cells cannot be merged.
            if (!basePlane.valid)
            {
                addQuad(x, z, x + 1, z + 1);

                used[currentCell] = true;
                continue;
            }

            // ----------------------------------------------------
            // Find maximum width.
            // ----------------------------------------------------

            int width = 1;

            while (x + width < size)
            {
                bool valid = true;

                for (int xx = x; xx <= x + width; ++xx)
                {
                    const int idx =
                        cellIndex(xx, z);

                    if (used[idx])
                    {
                        valid = false;
                        break;
                    }

                    const Plane plane =
                        getPlane(xx, z);

                    if (!samePlane(basePlane, plane))
                    {
                        valid = false;
                        break;
                    }
                }

                if (!valid)
                    break;

                ++width;
            }

            // ----------------------------------------------------
            // Find maximum height for that width.
            // ----------------------------------------------------

            int height = 1;

            while (z + height < size)
            {
                bool valid = true;

                for (int zz = z; zz <= z + height; ++zz)
                {
                    for (int xx = x; xx < x + width; ++xx)
                    {
                        const int idx =
                            cellIndex(xx, zz);

                        if (used[idx])
                        {
                            valid = false;
                            break;
                        }

                        const Plane plane =
                            getPlane(xx, zz);

                        if (!samePlane(basePlane, plane))
                        {
                            valid = false;
                            break;
                        }
                    }

                    if (!valid)
                        break;
                }

                if (!valid)
                    break;

                ++height;
            }

            // ----------------------------------------------------
            // Mark the rectangle as consumed.
            // ----------------------------------------------------

            for (int zz = z; zz < z + height; ++zz)
            {
                for (int xx = x; xx < x + width; ++xx)
                {
                    used[cellIndex(xx, zz)] = true;
                }
            }

            // ----------------------------------------------------
            // Generate one quad for the whole rectangle.
            // ----------------------------------------------------

            addQuad(
                x,
                z,
                x + width,
                z + height
            );
        }
    }
}
