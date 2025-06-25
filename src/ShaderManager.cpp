#include "ShaderManager.hpp"

void Engine::ShaderManager::init() {
    defaultShader = LoadShader(0, 0);
    solidShader = LoadShader("../shaders/solid.vs", "../shaders/solid.fs");
    materialPreviewShader = LoadShader("../shaders/raylibshaders/lighting.vs", "../shaders/raylibshaders/lighting.fs");

    if (!IsShaderValid(materialPreviewShader) || !IsShaderValid(defaultShader) || IsShaderValid(solidShader)) {
        throw std::runtime_error("ShaderManager::Engine::ShaderManager::init: Failed to load shader!");
    }
}
Shader Engine::ShaderManager::set(const Shader& prev) {
    if (IsKeyPressed(KEY_F1)) {
        return solidShader;
    } else if (IsKeyPressed(KEY_F2)) {
        return defaultShader;
    } else if (IsKeyPressed(KEY_F3)) {
        return materialPreviewShader;
    }

    return prev;
}
void Engine::ShaderManager::clean() {
    UnloadShader(defaultShader);
    UnloadShader(materialPreviewShader);
    UnloadShader(solidShader);
}
