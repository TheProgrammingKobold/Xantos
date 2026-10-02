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

    // VERY IMPORTNAT!! CAMERA AND RENDERER MUST BE CALLED FIRST BEFORE ANY MESH IS CREATED (duh)
    Camera camera(window.GetWidth(), window.GetHeight(), glm::vec3(0.0f, 0.0f, 2.0f));

    Renderer renderer(window, camera);

    // Find some place to puit this in the renderer
    glfwSwapInterval(0);

    auto shader = std::make_shared<Shader>("default_vertex.shader", "default_fragment.shader");

    auto floorTexture = std::make_shared<Texture>("grass.jpg");

    auto floorMaterial = std::make_shared<Material>(shader, floorTexture);

    Scene scene(floorMaterial);

    auto& player = scene.CreateEntity<Player>();

    player.transform.position = { 0.0f, 10.0f, 0.0f };

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    events.Subscribe<WindowResizeEvent>([&](const WindowResizeEvent& event)
        {
            renderer.Resize(event.width, event.height);
        });

    // 1. Initialize variables before the render loop
    double lastTime = glfwGetTime();
    int frameCount = 0;
    std::string fpsText = "FPS: 0\n0.00 ms/frame"; // Holds the text between updates
    constexpr float FIXED_DELTA_TIME = 1.0f / 60.0f;
    float physicsAccumulator = 0.0f;

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
        const float deltaTime = Util::GetDeltaTime();
        
        // For player and camera compatibility
        player.SetOrientation(camera.GetForward());

        scene.Update(deltaTime);

        physicsAccumulator += deltaTime;
        while (physicsAccumulator >= FIXED_DELTA_TIME)
        {
            scene.PhysicsUpdate(FIXED_DELTA_TIME);
            physicsAccumulator -= FIXED_DELTA_TIME;
        }

        renderer.BeginFrame();

        const auto& position = player.transform.position;

        scene.UpdateTerrain(position.x, position.z);

        camera.SetPosition(player.transform.position);
        camera.Rotate(player.GetYaw(), player.GetPitch());

        
        renderer.Submit(
            {
                fpsText,
                {20.0f, 50.0f},
                0.5f,
                glm::vec3(1.0f)
            });


        std::string playerPositon = 
            "Player X: " + 
            std::to_string(position.x) + 
            "\nPlayer Y: " + 
            std::to_string(position.y) + 
            "\nPlayer Z: " +
            std::to_string(position.z);

        renderer.Submit(
            {
                playerPositon,
                {20.0f, 200.0f},
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

