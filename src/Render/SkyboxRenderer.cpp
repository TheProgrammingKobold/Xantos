#include "SkyboxRenderer.h"

#include "Camera.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

std::vector<float> SKYBOX_VERTICES =
{
    // Back
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    // Left
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    // Right
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

     // Front
     -1.0f, -1.0f,  1.0f,
     -1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,
      1.0f, -1.0f,  1.0f,
     -1.0f, -1.0f,  1.0f,

     // Top
     -1.0f,  1.0f, -1.0f,
      1.0f,  1.0f, -1.0f,
      1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,
     -1.0f,  1.0f,  1.0f,
     -1.0f,  1.0f, -1.0f,

     // Bottom
     -1.0f, -1.0f, -1.0f,
     -1.0f, -1.0f,  1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
     -1.0f, -1.0f,  1.0f,
      1.0f, -1.0f,  1.0f
};


SkyboxRenderer::SkyboxRenderer()
    : _shader(
        "skybox_vertex.shader",
        "skybox_fragment.shader"
    ),
	_vbo(
		SKYBOX_VERTICES
	)
{

    _vao.Bind();
    _vbo.Bind();

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        nullptr
    );

    glEnableVertexAttribArray(0);

    _vao.Unbind();
    _vbo.Unbind();
}

void SkyboxRenderer::Render(
    const Camera& camera,
    const Cubemap& cubemap
)
{
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    _shader.Activate();

    glm::mat4 view =
        glm::mat4(
            glm::mat3(camera.GetViewMatrix())
        );

    glm::mat4 projection =
        camera.GetPerspectiveProjection();

    glm::mat4 camMatrix =
        projection * view;

    _shader.SetMatrix(
        camMatrix,
        "camMatrix"
    );

    cubemap.Bind(0);

    _shader.SetInt(
        0,
        "skybox"
    );

    _vao.Bind();

    glDrawArrays(
        GL_TRIANGLES,
        0,
        36
    );

    _vao.Unbind();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}