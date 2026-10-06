#include "Xantos.h"

EventBus events;

std::atomic<bool> running = true;

void Render(
    Window& window,
    RenderQueue& renderQueue,
    RenderResourceQueue& resourceQueue,
    std::promise<MaterialID> startupPromise
)
{
    bool startupPromiseSet = false;

    try
    {
        Renderer renderer(
            window
        );

        RenderAssetManager& assets =
            renderer.GetAssetManager();

        //
        // These are GPU assets.
        //
        // They are therefore created by the
        // render thread.
        //

        ShaderID defaultShader =
            assets.LoadShader(
                "default_vertex.shader",
                "default_fragment.shader"
            );

        TextureID floorTexture =
            assets.LoadTexture(
                "grass.jpg"
            );

        MaterialID terrainMaterial =
            assets.LoadMaterial(
                defaultShader,
                floorTexture
            );

        //
        // The main thread needs this ID in order
        // to construct the game-side Scene.
        //

        startupPromise.set_value(
            terrainMaterial
        );

        startupPromiseSet = true;

        RenderPacket currentPacket;

        bool hasPacket = false;

        while (running)
        {
            RenderPacket newPacket;

            if (
                renderQueue.TryConsume(
                    newPacket
                )
                )
            {
                currentPacket =
                    std::move(
                        newPacket
                    );

                hasPacket = true;
            }

            if (hasPacket)
            {
                renderer.Render(
                    currentPacket
                );
            }
            else
            {
                std::this_thread::yield();
            }

            //
            // GPU uploads/destruction also happen
            // exclusively on this thread.
            //

            renderer.ProcessResourceRequests(
                resourceQueue,
                4
            );
        }
    }
    catch (...)
    {
        const std::exception_ptr exception =
            std::current_exception();

        if (!startupPromiseSet)
        {
            startupPromise.set_exception(
                exception
            );
        }
        else
        {
            try
            {
                std::rethrow_exception(
                    exception
                );
            }
            catch (
                const std::exception& error
                )
            {
                std::cerr
                    << "Render thread exception: "
                    << error.what()
                    << std::endl;
            }
            catch (...)
            {
                std::cerr
                    << "Render thread exception: "
                    << "unknown exception"
                    << std::endl;
            }
        }

        running = false;
    }
}

int main()
{
    Input::Initialize(
        events
    );

    {
        Window window(
            1280,
            720,
            "Xantos",
            events
        );

        //
        // Game-side Camera.
        //
        // This Camera never goes to the render
        // thread.
        //

        Camera camera(
            window.GetWidth(),
            window.GetHeight(),
            glm::vec3(
                0.0f,
                0.0f,
                2.0f
            )
        );

        RenderQueue renderQueue;

        RenderResourceQueue resourceQueue;

        //
        // The renderer must initialize its OpenGL
        // resources before the main thread can
        // construct the Scene with the terrain
        // MaterialID.
        //

        std::promise<MaterialID>
            startupPromise;

        std::future<MaterialID>
            startupFuture =
            startupPromise.get_future();

        std::thread renderThread(
            Render,
            std::ref(window),
            std::ref(renderQueue),
            std::ref(resourceQueue),
            std::move(startupPromise)
        );

        MaterialID terrainMaterial;

        try
        {
            terrainMaterial =
                startupFuture.get();
        }
        catch (
            const std::exception& error
            )
        {
            std::cerr
                << "Failed to initialize renderer: "
                << error.what()
                << std::endl;

            running = false;

            renderThread.join();

            return -1;
        }

        //
        // Everything below this point is game-side.
        //

        Scene scene(
            terrainMaterial,
            resourceQueue
        );

        auto& player =
            scene.CreateEntity<Player>();

        player.transform.position = {
            0.0f,
            10.0f,
            0.0f
        };

        //
        // GLFW resize callback should ultimately
        // update the game-side Camera.
        //
        // Event dispatch now happens on the main
        // thread.
        //

        events.Subscribe<WindowResizeEvent>(
            [&](const WindowResizeEvent& event)
            {
                camera.SetWidthHeight(
                    event.width,
                    event.height
                );
            }
        );

        double lastFPSUpdate =
            glfwGetTime();

        int frameCount = 0;

        std::string fpsText =
            "FPS: 0\n0.00 ms/frame";

        constexpr float
            FIXED_DELTA_TIME =
            1.0f / 60.0f;

        float physicsAccumulator =
            0.0f;

        while (running)
        {
            //
            // GLFW belongs on the main thread.
            //

            glfwPollEvents();

            window.ApplyPendingMouseCapture();

            if (
                window.ShouldClose()
                )
            {
                running = false;
                break;
            }

            //
            // EventBus is now dispatched by the
            // game/main thread too.
            //

            events.Dispatch();

            Util::UpdateDeltaTime();

            const float deltaTime =
                Util::GetDeltaTime();

            ++frameCount;

            const double currentTime =
                glfwGetTime();

            if (
                currentTime -
                lastFPSUpdate >= 1.0
                )
            {
                const double msPerFrame =
                    1000.0 /
                    static_cast<double>(
                        frameCount
                        );

                fpsText =
                    "FPS: " +
                    std::to_string(
                        frameCount
                    ) +
                    "\n" +
                    std::to_string(
                        msPerFrame
                    ) +
                    " ms/frame";

                frameCount = 0;

                lastFPSUpdate += 1.0;
            }

            //
            // GAME UPDATE
            //

            player.SetOrientation(
                camera.GetForward()
            );

            scene.Update(
                deltaTime
            );

            physicsAccumulator +=
                deltaTime;

            while (
                physicsAccumulator >=
                FIXED_DELTA_TIME
                )
            {
                scene.PhysicsUpdate(
                    FIXED_DELTA_TIME
                );

                physicsAccumulator -=
                    FIXED_DELTA_TIME;
            }

            camera.SetPosition(
                player.transform.position
            );

            camera.Rotate(
                player.GetYaw(),
                player.GetPitch()
            );

            const auto& position =
                player.transform.position;

            //
            // CPU terrain generation.
            //
            // No OpenGL happens here.
            //

            scene.UpdateTerrain(
                position.x,
                position.z
            );

            //
            // BUILD RENDER PACKET
            //

            RenderPacket packet;

            packet.camera.position =
                position;

            packet.camera.yaw =
                player.GetYaw();

            packet.camera.pitch =
                player.GetPitch();

            packet.camera.width =
                window.GetWidth();

            packet.camera.height =
                window.GetHeight();

            scene.BuildRenderPacket(
                packet,
                camera
            );

            //
            // UI is just data now.
            //

            packet.textCommands.push_back({
                fpsText,
                {20.0f, 50.0f},
                0.5f,
                glm::vec3(1.0f)
                });

            const std::string playerPosition =
                "Player X: " +
                std::to_string(position.x) +
                "\nPlayer Y: " +
                std::to_string(position.y) +
                "\nPlayer Z: " +
                std::to_string(position.z);

            packet.textCommands.push_back({
                playerPosition,
                {20.0f, 200.0f},
                0.5f,
                glm::vec3(1.0f)
                });

            //
            // HAND OFF TO RENDER THREAD
            //

            renderQueue.Submit(
                std::move(packet)
            );

            Input::EndFrame();
        }

        running = false;

        //
        // Once this returns, all OpenGL resources
        // owned by Renderer are destroyed on the
        // render thread while the context is still
        // alive.
        //

        renderThread.join();
    }

    glfwTerminate();

    return 0;
}