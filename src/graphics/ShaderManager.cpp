#include <stdexcept>
#include "../core/Context.hpp"
#include "ShaderManager.hpp"

void Engine::ShaderManager::init() {
    // Shaders load
    materialPreviewShader = LoadShader(nullptr, nullptr);
    solidShader = LoadShader("../assets/shaders/solid.vs", "../assets/shaders/solid.fs");
    renderShader = LoadShader(TextFormat("../assets/shaders/raylibshaders/lighting.vs"), TextFormat("../assets/shaders/raylibshaders/lighting.fs"));

    if (!IsShaderValid(renderShader) || !IsShaderValid(materialPreviewShader) || !IsShaderValid(solidShader)) {
        throw std::runtime_error("Engine::ShaderManager::init: Failed to load shader!");
    }

    // Void colors
    voidColSolid = { 64, 64, 64, 255 };
    voidColPreview = { 89, 89, 89, 255 };
    voidColRender = { 13, 13, 13, 255 };
    voidColWireframe = { 51, 51, 51, 255 };

    // Solid shader
    lightColor[0] = 1.0f;
    lightColor[1] = 1.0f;
    lightColor[2] = 1.0f;
    lightDir = glm::normalize(glm::vec3{0.625f, 0.159f, 0.917f});
    lightDirection[0] = lightDir.x;
    lightDirection[1] = lightDir.y;
    lightDirection[2] = lightDir.z;
    uLightDirLoc = GetShaderLocation(solidShader, "lightDir"); // shader set as solid by default
    uBaseColorLoc = GetShaderLocation(solidShader, "baseColor");

    // Render shader
    renderShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(renderShader, "viewPos");
    ambientLoc = GetShaderLocation(renderShader, "ambient");
    ambientColor[0] = 0.1f;
    ambientColor[1] = 0.1f;
    ambientColor[2] = 0.1f;
    ambientColor[3] = 1.0f;
}

void Engine::ShaderManager::clean() const {
    UnloadShader(solidShader);
    UnloadShader(renderShader);
}

Shader* Engine::ShaderManager::getShader(int shaderIndex) {
    if (shaderIndex == SOLID) return &solidShader;
    else if (shaderIndex == M_PREVIEW) return &materialPreviewShader;
    else if (shaderIndex == RENDER) return &renderShader;
    else throw std::runtime_error("Engine::ShaderManager::getShader: Invalid shader index!");
}

void Engine::ShaderManager::handleSetShaderValue(int shaderIndex, std::shared_ptr<Context>& p_context) {
    if (shaderIndex == SOLID) {

        SetShaderValue(solidShader, uBaseColorLoc, &lightColor, SHADER_UNIFORM_VEC3);
        SetShaderValue(solidShader, uLightDirLoc, &lightDir[0], SHADER_UNIFORM_VEC3);
    } else if (shaderIndex == RENDER) {

        Vector3 pos = p_context->customCamera->getCamera()->position;
        float cameraPos[3] = { pos.x, pos.y, pos.z };
        SetShaderValue(renderShader, renderShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
        SetShaderValue(renderShader, GetShaderLocation(renderShader, "numLights"), &currLightsCount, SHADER_UNIFORM_INT);
        SetShaderValue(renderShader, ambientLoc, ambientColor, SHADER_UNIFORM_VEC4);
    }
}

void Engine::ShaderManager::handleShaderSelection(int &currentSh, bool ignoreWireframe, bool ignoreRender, bool ignoreMaterial, bool ignoreSolid) {
    if (IsKeyPressed(KEY_F1) && !ignoreSolid) {
        currentSh = SOLID;
    } else if (IsKeyPressed(KEY_F2) && !ignoreMaterial) {
        currentSh = M_PREVIEW;
    } else if (IsKeyPressed(KEY_F3) && !ignoreRender) {
        currentSh = RENDER;
    } else if (IsKeyPressed(KEY_F4) && !ignoreWireframe) {
        currentSh = WIREFRAME;
    }
}

void Engine::ShaderManager::handleBackgroundClearing(int shaderIndex) const {
    if (shaderIndex == SOLID) ClearBackground(voidColSolid);
    else if (shaderIndex == M_PREVIEW) ClearBackground(voidColPreview);
    else if (shaderIndex == RENDER) ClearBackground(voidColRender);
    else ClearBackground(voidColWireframe);
}

void Engine::ShaderManager::changeColor() const {
    SetShaderValue(solidShader, uBaseColorLoc, &lightColor, SHADER_UNIFORM_VEC3);
}

void Engine::ShaderManager::changeDirection() {
    auto newDir = glm::vec3(lightDirection[0], lightDirection[1], lightDirection[2]);
    lightDir = glm::normalize(newDir);
    SetShaderValue(solidShader, uLightDirLoc, &lightDir[0], SHADER_UNIFORM_VEC3);
}

void Engine::ShaderManager::updateCamPos(std::shared_ptr<Context>& p_context) const {
    Vector3 cameraPos = p_context->customCamera->getCamera()->position;
    SetShaderValue(renderShader, renderShader.locs[SHADER_LOC_VECTOR_VIEW], &cameraPos, SHADER_UNIFORM_VEC3);
}

void Engine::ShaderManager::updateLights() const {
    int numLightsLoc = GetShaderLocation(renderShader, "numLights");
    SetShaderValue(renderShader, numLightsLoc, &currLightsCount, SHADER_UNIFORM_INT);
}

void Engine::ShaderManager::updateLightValues(std::shared_ptr<Context>& p_context) const {
    for (int i = 0; i < p_context->shaders->currLightsCount; i++) {
        UpdateLightValues(renderShader, p_context->llights.at(i)->_l_light);
    }
}

void Engine::ShaderManager::updateSingleLight(std::shared_ptr<Context>& p_context, int lightIndex) const {
    if (lightIndex >= 0 && lightIndex < currLightsCount) {
        UpdateLightValues(renderShader, p_context->llights.at(lightIndex)->_l_light);
    }
}
