#include "Xantos.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <atomic>
#include <thread>
#include <cmath>

EventBus events;

std::atomic<bool> running = true;

void Render(Window& window)
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

    // Find some place to puit this in the renderer
    glfwSwapInterval(0);

    auto shader = std::make_shared<Shader>("default_vertex.shader", "default_fragment.shader");

    auto floorTexture = std::make_shared<Texture>("grass.jpg");

    auto floorMaterial = std::make_shared<Material>(shader, floorTexture);
     
    const int chunkSize = TerrainChunk::GetChunkSize();
    constexpr int loadRadius = 10;
    constexpr int unloadRadius = loadRadius + 1;

    TerrainGenerator terrainGenerator;

    Scene scene;

    auto& player = scene.CreateEntity<Player>();

    constexpr int spawnChunk = 16;
    player.transform.position = { static_cast<float>(spawnChunk * chunkSize), 10.0f, static_cast<float>(spawnChunk * chunkSize) };

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 2. Define your angle in degrees and convert to radians
    float angleDegrees = 50.0f;
    float angleRadians = glm::radians(angleDegrees);

    events.Subscribe<WindowResizeEvent>([&](const WindowResizeEvent& event)
        {
            renderer.Resize(event.width, event.height);
        });

    // 1. Initialize variables before the render loop
    double lastTime = glfwGetTime();
    int frameCount = 0;
    std::string fpsText = "FPS: 0\n0.00 ms/frame"; // Holds the text between updates

    // render loop
    // -----------
    while (running)
    {
        // 2. Count frames and check if 1 second has elapsed
        double currentTime = glfwGetTime();
        frameCount++;

        if (currentTime - lastTime >= 1.0)
        {
            double msPerFrame = 1000.0 / double(frameCount);
            int fps = frameCount;

            // Update our cached string representation
            fpsText = "FPS: " + std::to_string(fps) + "\n" + std::to_string(msPerFrame) + " ms/frame";

            frameCount = 0;
            lastTime += 1.0;
        }

        // Handles events.
        events.Dispatch();

        Util::UpdateDeltaTime();

        renderer.BeginFrame();
        
        // For player and camera compatibility
        player.SetOrientation(camera.GetForward());

        scene.Update(Util::GetDeltaTime());

        const auto& position = player.transform.position;
        const int playerChunkX = static_cast<int>(std::floor(position.x / chunkSize));
        const int playerChunkZ = static_cast<int>(std::floor(position.z / chunkSize));

        scene.RemoveTerrainChunksOutsideRadius(playerChunkX, playerChunkZ, unloadRadius);

        for (int dz = -loadRadius; dz <= loadRadius; ++dz)
        {
            for (int dx = -loadRadius; dx <= loadRadius; ++dx)
            {
                if (dx * dx + dz * dz > loadRadius * loadRadius)
                    continue;

                const int chunkX = playerChunkX + dx;
                const int chunkZ = playerChunkZ + dz;
                if (!scene.HasTerrainChunk(chunkX, chunkZ))
                {
                    auto chunk = std::make_unique<TerrainChunk>(terrainGenerator, floorMaterial, chunkX, chunkZ);
                    scene.AddTerrainChunk(chunkX, chunkZ, std::move(chunk));
                }
            }
        }

        const float groundY = terrainGenerator.GetHeight(
            static_cast<int>(std::round(position.x)),
            static_cast<int>(std::round(position.z)));

        const float playerBottom = position.y + player.GetAABB().min.y;
        if (playerBottom < groundY)
        {
            player.transform.position.y = groundY - player.GetAABB().min.y;
			player.SetGrounded(true);
        }
        else
        {
			player.SetGrounded(false);
        }

        camera.SetPosition(player.transform.position);
        camera.Rotate(player.GetYaw(), player.GetPitch());

        
        renderer.Submit(
            {
                fpsText,
                {20.0f, 30.0f},
                0.5f,
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
        1280,
        720,
        "Xantos",
        events
    );

    std::thread renderThread(Render, std::ref(window));
     
    while (running)
    {
        glfwPollEvents();
        window.ApplyPendingMouseCapture();


        if (window.ShouldClose())
        {
            running = false;
        }
    }

    renderThread.join();

    glfwTerminate();
    return 0;
}

