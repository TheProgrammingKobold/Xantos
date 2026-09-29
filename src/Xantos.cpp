#include "Xantos.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <atomic>
#include <thread>

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

    auto shader = std::make_shared<Shader>("default_vertex.shader", "default_fragment.shader");


    TextureInfo ratButtInfo(GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE, "rat_butt.jpg");
    auto ratButtTexture = std::make_shared<Texture>(ratButtInfo);

    auto ratCubeMaterial = std::make_shared<Material>(shader, ratButtTexture);

    const int worldWidth = 32;
    const int worldDepth = 32;
    const int chunkSize = TerrainChunk::GetChunkSize();

    TerrainGenerator terrainGenerator(worldWidth * chunkSize + 1, worldDepth * chunkSize + 1);
	terrainGenerator.Generate();

    Scene scene;

	std::vector<std::shared_ptr<Mesh>> terrainChunks;
	terrainChunks.reserve(worldWidth* worldDepth);
    
    for (int z = 0; z < worldDepth; ++z)
    {
		for (int x = 0; x < worldWidth; ++x)
		{
			TerrainChunk chunk(terrainGenerator, x, z);
			auto mesh = std::make_unique<Mesh>(chunk.GetVertices(), chunk.GetIndices());
			terrainChunks.push_back(std::move(mesh));
		}
    }

	for (int z = 0; z < worldDepth; ++z)
	{
		for (int x = 0; x < worldWidth; ++x)
		{
			auto& terrainEntity = scene.CreateEntity<Entity>();
			terrainEntity.mesh = terrainChunks[x + z * worldWidth];
			terrainEntity.material = ratCubeMaterial;
            terrainEntity.transform.position = { static_cast<float>(x * chunkSize), 0.0f, static_cast<float>(z * chunkSize) };
		}
	}

    auto& player = scene.CreateEntity<Player>();

    player.material = ratCubeMaterial;

    player.transform.position = { static_cast<float>(worldWidth * chunkSize / 2), 0.0f, static_cast<float>(worldDepth * chunkSize / 2) };

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
        1920,
        1080,
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

