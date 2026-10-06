// Xantos.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <atomic>
#include <exception>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <utility>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Render/VBO.h"
#include "Render/VAO.h"
#include "Render/EBO.h"
#include "Render/Shader.h"
#include "Render/Camera.h"
#include "Render/Texture.h"
#include "Render/TextRenderer.h"
#include "Render/Renderer.h"
#include "Render/Mesh.h"
#include "Render/Material.h"
#include "Core/RenderAssetManager.h"
#include "Core/RenderQueue.h"
#include "Core/RenderResourceQueue.h"
#include "Core/Scene.h"
#include "Core/Window.h"
#include "Core/Input.h"
#include "Core/TerrainGenerator.h"
#include "Core/TerrainChunk.h"
#include "Game/Player/Player.h"
#include "Events/EventTypes.h"
#include "Events/EventBus.h"
#include "Util/File.h"
#include "Util/Time.h"

// TODO: Reference additional headers your program requires here.
