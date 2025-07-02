#include "ShaderManager.hpp"

void Engine::ShaderManager::init() {
    materialPreviewShader = LoadShader(nullptr, nullptr);
    solidShader = LoadShader("../shaders/solid.vs", "../shaders/solid.fs");
    renderShader = LoadShader(TextFormat("../shaders/raylibshaders/lighting.vs"), TextFormat("../shaders/raylibshaders/lighting.fs"));

    if (!IsShaderValid(renderShader) || !IsShaderValid(materialPreviewShader) || !IsShaderValid(solidShader)) {
        throw std::runtime_error("ShaderManager::Engine::ShaderManager::init: Failed to load shader!");
    }

    uLightDirLoc = GetShaderLocation(solidShader, "lightDir");
    uBaseColorLoc = GetShaderLocation(solidShader, "baseColor");

    lightColor[0] = 1.0f;
    lightColor[1] = 1.0f;
    lightColor[2] = 1.0f;

    lightDir = glm::normalize(glm::vec3{-1.0f, 1.0f, -1.0f});

    lightDirection[0] = lightDir.x;
    lightDirection[1] = lightDir.y;
    lightDirection[2] = lightDir.z;

    renderShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(renderShader, "viewPos");
}

Shader Engine::ShaderManager::initShader() {
    return solidShader;
}

void Engine::ShaderManager::set(Shader& shader) {
    if (IsKeyPressed(KEY_F1)) {
        SetShaderValue(solidShader, uBaseColorLoc, &lightColor, SHADER_UNIFORM_VEC3);
        SetShaderValue(solidShader, uLightDirLoc, &lightDir[0], SHADER_UNIFORM_VEC3);
        shader = solidShader;
    } else if (IsKeyPressed(KEY_F2)) {
        shader = materialPreviewShader;
    } else if (IsKeyPressed(KEY_F3)) {
        shader = renderShader;
    }
}

void Engine::ShaderManager::clean() {
    UnloadShader(renderShader);
    UnloadShader(solidShader);
}
