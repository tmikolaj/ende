#include "ShaderManager.hpp"

void Engine::ShaderManager::init() {
    materialPreviewShader = LoadShader(0, 0);
    solidShader = LoadShader("../shaders/solid.vs", "../shaders/solid.fs");
    renderShader = LoadShader("../shaders/raylibshaders/lighting.vs", "../shaders/raylibshaders/lighting.fs");

    if (!IsShaderValid(renderShader) || !IsShaderValid(materialPreviewShader) || !IsShaderValid(solidShader)) {
        throw std::runtime_error("ShaderManager::Engine::ShaderManager::init: Failed to load shader!");
    }
}
Shader Engine::ShaderManager::set(const Shader& prev) {
    if (IsKeyPressed(KEY_F1)) {
        return solidShader;
    } else if (IsKeyPressed(KEY_F2)) {
        return materialPreviewShader;
    } else if (IsKeyPressed(KEY_F3)) {
        return renderShader;
    }

    return prev;
}
void Engine::ShaderManager::clean() {
    UnloadShader(renderShader);
    UnloadShader(solidShader);
}
