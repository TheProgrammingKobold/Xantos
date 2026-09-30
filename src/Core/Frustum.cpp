#include "Frustum.h"

void Frustum::Update(const glm::mat4& matrix)
{
    _planes[0] = glm::vec4(
        matrix[0][3] + matrix[0][0],
        matrix[1][3] + matrix[1][0],
        matrix[2][3] + matrix[2][0],
        matrix[3][3] + matrix[3][0]
    );

    _planes[1] = glm::vec4(
        matrix[0][3] - matrix[0][0],
        matrix[1][3] - matrix[1][0],
        matrix[2][3] - matrix[2][0],
        matrix[3][3] - matrix[3][0]
    );

    _planes[2] = glm::vec4(
        matrix[0][3] + matrix[0][1],
        matrix[1][3] + matrix[1][1],
        matrix[2][3] + matrix[2][1],
        matrix[3][3] + matrix[3][1]
    );

    _planes[3] = glm::vec4(
        matrix[0][3] - matrix[0][1],
        matrix[1][3] - matrix[1][1],
        matrix[2][3] - matrix[2][1],
        matrix[3][3] - matrix[3][1]
    );

    _planes[4] = glm::vec4(
        matrix[0][3] + matrix[0][2],
        matrix[1][3] + matrix[1][2],
        matrix[2][3] + matrix[2][2],
        matrix[3][3] + matrix[3][2]
    );

    _planes[5] = glm::vec4(
        matrix[0][3] - matrix[0][2],
        matrix[1][3] - matrix[1][2],
        matrix[2][3] - matrix[2][2],
        matrix[3][3] - matrix[3][2]
    );
}

bool Frustum::Intersects(const AABB& bounds) const
{
    for (int i = 0; i < 6; i++)
    {
        glm::vec3 normal = glm::vec3(_planes[i]);

        glm::vec3 positiveVertex;

        positiveVertex.x =
            normal.x >= 0.0f ? bounds.max.x : bounds.min.x;

        positiveVertex.y =
            normal.y >= 0.0f ? bounds.max.y : bounds.min.y;

        positiveVertex.z =
            normal.z >= 0.0f ? bounds.max.z : bounds.min.z;

        if (glm::dot(normal, positiveVertex) + _planes[i].w < 0.0f)
        {
            return false;
        }
    }

    return true;
}