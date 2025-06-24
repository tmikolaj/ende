#include "ShaderManager.hpp"

void Engine::ShaderManager::init() {
    defaultShader = LoadShader(0, 0);
    solidShader = LoadShader("../shaders/solid.vs", "../solid.fs");
    materialPreviewShader = LoadShader("../shaders/material_preview.vs", "../shaders/material_preview.fs");
}
Shader Engine::ShaderManager::set() {

}
void Engine::ShaderManager::clean() {
    UnloadShader(defaultShader);
    UnloadShader(materialPreviewShader);
    UnloadShader(solidShader);
}
