#include "Xantos.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <atomic>
#include <thread>

EventBus events;

std::atomic<bool> running = true;

void render(Window& window)
{
    Renderer renderer(window);

    auto shader = std::make_shared<Shader>("default_vertex.shader", "default_fragment.shader");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<Vertex> vertices =
    {
        //		Position						Normals						Color						TexCoords
        // Front
        Vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f, 0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3(0.5f, -0.5f, 0.5f) , glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3(0.5f,  0.5f, 0.5f) , glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},

        // Back
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3(0.5f, -0.5f, -0.5f) , glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3(0.5f,  0.5f, -0.5f) , glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},

        // Top
        Vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f,  0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f,  0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3(0.5f,  0.5f,  0.5f) , glm::vec3(0.0f, 1.0f,  0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3(0.5f,  0.5f, -0.5f) , glm::vec3(0.0f, 1.0f,  0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},

        // Bottom :3
        Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,-1.0f,  0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,-1.0f,  0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3(0.5f, -0.5f,  0.5f) , glm::vec3(0.0f,-1.0f,  0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3(0.5f, -0.5f, -0.5f) , glm::vec3(0.0f,-1.0f,  0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},

        // Left
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},

        // Right
        Vertex{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f,  0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f,  0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f,  0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.0f,  0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
    };


    std::vector<GLuint> indices =
    {
        // Front
        0, 2, 1,
        1, 2, 3,

        // Back
        4, 5, 6,
        5, 7, 6,

        // Top
        8, 10, 9,
        9, 10, 11,

        // Bottom
        12, 14, 13,
        13, 14, 15,

        // Left
        16, 18, 17,
        17, 18, 19,

        // Right
        20, 22, 21,
        21, 22, 23
    };

    Mesh cube(vertices, indices);

    glm::mat4 model = glm::mat4(1.0f);

    Camera camera(window.GetWidth(), window.GetHeight(), glm::vec3(0.0f, 0.0f, 2.0f));

    TextRenderer textRenderer(
        "Assets/Fonts/SpaceMono-Regular.ttf",
        256,
        400
    ); 

    textRenderer.SetProjection(camera.getOrthoProjection());
    textRenderer.SetViewportSize(window.GetWidth(), window.GetHeight());

    TextureInfo ratButtInfo(GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE, "rat_butt.jpg");
    auto ratButtTexture = std::make_shared<Texture>(ratButtInfo);

    auto ratCubeMaterial = std::make_shared<Material>(shader, ratButtTexture);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 2. Define your angle in degrees and convert to radians
    float angleDegrees = 50.0f;
    float angleRadians = glm::radians(angleDegrees);

    events.Subscribe<WindowResizeEvent>([&](const WindowResizeEvent& event)
        {
            glViewport(0, 0, event.width, event.height);
            camera.setWidthHeight(event.width, event.height);

            textRenderer.SetProjection(camera.getOrthoProjection());
            textRenderer.SetViewportSize(event.width, event.height);
        });

    // render loop
    // -----------
    while (running)
    {
        events.Dispatch();

		Util::updateDeltaTime();

        renderer.BeginFrame();

        renderer.Submit([&]()
            {
                ratCubeMaterial->Bind();
                cube.Bind();

                ratCubeMaterial->GetShader().setMatrix(camera.getMatrix(), "camMatrix");
                ratCubeMaterial->GetShader().setMatrix(model, "model");

                //ratButtTexture.texUnit(shader, "tex", 0);

                cube.Draw();

                model = glm::rotate(model, Util::getDeltaTime() * angleRadians, glm::vec3(0.3f, 1.0f, 0.5f));
            });

        renderer.Submit([&]()
            {
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);

                textRenderer.RenderTextRelative(
                    "Testing of cool text rendering\nTesting of all kinds of cool stuff!\nHello every nyan!",
                    0.02f,
                    0.5f,
                    0.05f,
                    glm::vec3(1.0f)
                );

                textRenderer.RenderTextRelative(
                    "This will be cool =3",
                    0.02f,
                    0.2f,
                    0.05f,
                    glm::vec3(1.0f, 0.0f, 1.0f)
                );

                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);
            });


        renderer.ExecuteCommands();

        renderer.EndFrame();


    }
}

int main()
{
    Window window(
        800,
        600,
        "Xantos",
        events
    );

    std::thread renderThread(render, std::ref(window));
     
    while (running)
    {
        glfwPollEvents();


        if (window.ShouldClose())
        {
            running = false;
        }
    }

    renderThread.join();

    glfwTerminate();
    return 0;
}

