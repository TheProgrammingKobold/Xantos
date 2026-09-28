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
        12, 13, 14,
        14, 13, 15,

        // Left
        16, 18, 17,
        17, 18, 19,

        // Right
        20, 22, 21,
        21, 22, 23
    };

    // VERY IMPORTNAT!! CAMERA AND RENDERER MUST BE CALLED FIRST BEFORE ANY MESH IS CREATED (duh)
    Camera camera(window.GetWidth(), window.GetHeight(), glm::vec3(0.0f, 0.0f, 2.0f));

    Renderer renderer(window, camera);

    auto cube = std::make_shared<Mesh>(vertices, indices);

    Transform transform;

    auto shader = std::make_shared<Shader>("default_vertex.shader", "default_fragment.shader");


    TextureInfo ratButtInfo(GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE, "rat_butt.jpg");
    auto ratButtTexture = std::make_shared<Texture>(ratButtInfo);

    auto ratCubeMaterial = std::make_shared<Material>(shader, ratButtTexture);

    Scene scene;
    Entity& ratCube1 = scene.CreateEntity<Entity>();
    ratCube1.mesh = cube;
    ratCube1.material = ratCubeMaterial;
    ratCube1.transform = transform;

    Entity& ratCube2 = scene.CreateEntity<Entity>();
    ratCube2.mesh = cube;
    ratCube2.material = ratCubeMaterial;
    ratCube2.transform = transform;
    ratCube2.transform.position = { 3.0f, 0.0f, -2.0f };

    Entity& ratCube3 = scene.CreateEntity<Entity>();
    ratCube3.mesh = cube;
    ratCube3.material = ratCubeMaterial;
    ratCube3.transform = transform;
    ratCube3.transform.position = { -3.0f, 0.0f, -2.0f };

    auto& player = scene.CreateEntity<Player>();

    player.mesh = cube;
    player.material = ratCubeMaterial;

    player.transform.position = { -10.0f, 0.0f, 1.0f };

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 2. Define your angle in degrees and convert to radians
    float angleDegrees = 50.0f;
    float angleRadians = glm::radians(angleDegrees);

    events.Subscribe<WindowResizeEvent>([&](const WindowResizeEvent& event)
        {
            renderer.Resize(event.width, event.height);
        });

    // render loop
    // -----------
    while (running)
    {
        events.Dispatch();

		Util::updateDeltaTime();

        renderer.BeginFrame();
        
        player.SetOrientation(camera.GetForward());

        scene.Update(Util::getDeltaTime());

        camera.setPosition(player.transform.position);
        camera.Rotate(player.GetYaw(), player.GetPitch());

        
        renderer.Submit(
            {
                "Testing of cool text rendering\nTesting of all kinds of cool stuff!\nHello every nyan!",
                {20.0f, 30.0f},
                1.0f,
                glm::vec3(1.0f)
            });
        
        scene.Render(renderer);

        renderer.ExecuteCommands();

        renderer.EndFrame();

        Input::EndFrame();
    }
}

int main()
{
    Input::Initialize(events);
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

