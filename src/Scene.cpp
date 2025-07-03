#include "Scene.hpp"

Scene::Scene(std::shared_ptr<Context>& context) :
m_context(context),
camera({ 0 }),
openRenamePopup(false) {

}

void Scene::init() {
    materialPreviewShader = LoadShader(nullptr, nullptr);
    solidShader = LoadShader("../shaders/solid.vs", "../shaders/solid.fs");
    renderShader = LoadShader(TextFormat("../shaders/raylibshaders/lighting.vs"), TextFormat("../shaders/raylibshaders/lighting.fs"));

    if (!IsShaderValid(renderShader) || !IsShaderValid(materialPreviewShader) || !IsShaderValid(solidShader)) {
        throw std::runtime_error("ShaderManager::Engine::ShaderManager::init: Failed to load shader!");
    }

    SetWindowSize(1920, 1080);
    SetWindowPosition(0, 0);
    m_context->states->setWindowState(NONE);

    camera.position = { 5, 0, 5 };
    camera.target = { 0, 0, 0 };
    camera.up = { 0, 1, 0 };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    m_context->entities->push_back(Entity(GenMeshCube(2.0f, 2.0f, 2.0f), "cube"));

    // render/shader variables init
    selectedEntity = -1;
    currentSh = 0;
    curr_m = "SOLID";
    onSelectionMeshColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    onSelectionWiresColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

    // shader related variables init
    // solid shader
    lightColor[0] = 1.0f;
    lightColor[1] = 1.0f;
    lightColor[2] = 1.0f;
    lightDir = glm::normalize(glm::vec3{-1.0f, 1.0f, -1.0f});
    lightDirection[0] = lightDir.x;
    lightDirection[1] = lightDir.y;
    lightDirection[2] = lightDir.z;
    uLightDirLoc = GetShaderLocation(solidShader, "lightDir"); // shader set as solid by default
    uBaseColorLoc = GetShaderLocation(solidShader, "baseColor");
    // render shader
    renderShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(renderShader, "viewPos");

    // camera related variables init
    zoomSpeed = 1.0f;
    distance = 10.0f;

    // scene controls init
    chunkSize = 2.5f;
    showWires = false;
    toggleWireframe = false;

    // collision (to check if the entity was hit) init
    Ray ray = { 0 };

    SetShaderValue(solidShader, uBaseColorLoc, &lightColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(solidShader, uLightDirLoc, &lightDir[0], SHADER_UNIFORM_VEC3);
}

void Scene::process() {
    distance -= GetMouseWheelMove() * zoomSpeed;

    Vector3 camPos = {
        0 + distance,
        0 + distance,
        0 + distance
    };
    camera.position = camPos;

    ray = GetMouseRay(GetMousePosition(), camera);
    if (!ImGui::GetIO().WantCaptureMouse && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        selectedEntity = -1;
        float closestHit = FLT_MAX;

        for (int i = 0; i < m_context->entities->size(); i++) {
            RayCollision hit = GetRayCollisionBox(ray, m_context->entities->at(i).e_boundingBox);
            if (hit.hit && hit.distance < closestHit) {
                closestHit = hit.distance;
                selectedEntity = i;
            }
        }
    }

    if (IsKeyPressed(KEY_F1)) {

        curr_m = "SOLID";
        currentSh = SOLID;
        SetShaderValue(solidShader, uBaseColorLoc, &lightColor, SHADER_UNIFORM_VEC3);
        SetShaderValue(solidShader, uLightDirLoc, &lightDir[0], SHADER_UNIFORM_VEC3);
        toggleWireframe = false;
    } else if (IsKeyPressed(KEY_F2)) {

        curr_m = "MATERIAL PREVIEW";
        currentSh = M_PREVIEW;
        toggleWireframe = false;
    } else if (IsKeyPressed(KEY_F3)) {

        curr_m = "RENDER";
        currentSh = RENDER;

        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(renderShader, renderShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
        toggleWireframe = false;
    } else if (IsKeyPressed(KEY_F4)) {

        curr_m = "WIREFRAME";
        currentSh = WIREFRAME;
        toggleWireframe = true;
    }
}

void Scene::draw() {
    if (WindowShouldClose()) {
        m_context->states->setWindowState(EXIT);
        return;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (int i = 0; i < m_context->entities->size(); i++) {
        Entity& e = m_context->entities->at(i);
        if (currentSh == SOLID) {
            e.e_model.materials[0].shader = solidShader;
        } else if (currentSh == M_PREVIEW) {
            e.e_model.materials[0].shader = materialPreviewShader;
        } else {
            e.e_model.materials[0].shader = renderShader;
        }
    }

    BeginMode3D(camera);

    for (int i = 0; i < m_context->entities->size(); i++) {
        auto epos = static_cast<Vector3>(m_context->entities->at(i).e_position[0],
                                            m_context->entities->at(i).e_position[1],
                                            m_context->entities->at(i).e_position[2]);
        if (selectedEntity == i) {
            if (currentSh != WIREFRAME) DrawModel(m_context->entities->at(i).e_model, epos, 1.0f, ImVecToColor(onSelectionMeshColor));
            if (currentSh != SOLID) DrawModelWires(m_context->entities->at(i).e_model, epos, 1.0f, ImVecToColor(onSelectionWiresColor));
        } else {
            if (currentSh != WIREFRAME) DrawModel(m_context->entities->at(i).e_model, epos, 1.0f, m_context->entities->at(i).e_color);
            if (currentSh != SOLID && (showWires || toggleWireframe)) DrawModelWires(m_context->entities->at(i).e_model, epos, 1.0f, RED);
        }
    }

    DrawGrid(100, chunkSize);

    EndMode3D();

    rlImGuiBegin();

    int mw = GetScreenWidth();

    ImGui::SetNextWindowPos(ImVec2(mw - 400, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, 1080), ImGuiCond_Once);

    ImGui::Begin("Scene Manager", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::PushItemWidth(200);

    ImGui::SetWindowFontScale(2.0f);
    ImGui::Text("Scene Entities");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Dummy(ImVec2(0, 5));

    if (m_context->entities->empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No entities found!");
    }

    for (int i = 0; i < m_context->entities->size(); i++) {
        if (ImGui::Selectable(m_context->entities->at(i).e_name.c_str(), selectedEntity == i)) {
            selectedEntity = i;
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            selectedEntity = i;
            ImGui::OpenPopup("Context");
        }
    }
    if (ImGui::BeginPopup("Context")) {
        if (ImGui::MenuItem("Delete")) {
            if (selectedEntity >= 0 && selectedEntity < m_context->entities->size()) {

                m_context->entities->erase(m_context->entities->begin() + selectedEntity);
                selectedEntity = -1;
            }
        }
        if (ImGui::MenuItem("Rename")) {
            if (selectedEntity >= 0 && selectedEntity < m_context->entities->size()) {

                strncpy(renameBuffer, m_context->entities->at(selectedEntity).e_name.c_str(), sizeof(renameBuffer));
                renameBuffer[sizeof(renameBuffer) - 1] = '\0';
                openRenamePopup = true;
            }
        }
        ImGui::EndPopup();
    }
    if (openRenamePopup) {
        openRenamePopup = false;
        ImGui::OpenPopup("RenameEntity");
    }

    if (ImGui::BeginPopupModal("RenameEntity", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("New Name");
        ImGui::SameLine();
        ImGui::InputText("##RenameInput", renameBuffer, sizeof(renameBuffer));
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("OK")) {
            if (selectedEntity >= 0 && selectedEntity < m_context->entities->size()) {
                m_context->entities->at(selectedEntity).e_name = std::string(renameBuffer);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }

    ImGui::End();
    rlImGuiEnd();

    std::string text = "Current mode: " + curr_m;
    DrawText(text.c_str(), 20, 10, 20, BLACK);

    EndDrawing();
}

void Scene::clean() {
    UnloadShader(solidShader);
    UnloadShader(renderShader);
    CloseWindow();
}

Color Scene::ImVecToColor(ImVec4 _color) {
    return (Color){
        static_cast<unsigned char>(_color.x * 255.0f),
        static_cast<unsigned char>(_color.y * 255.0f),
        static_cast<unsigned char>(_color.z * 255.0f),
        255
    };
}