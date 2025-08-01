#include "SceneEditorState.hpp"
#define RLIGHTS_IMPLEMENTATION
#include "raymath.h"
#include "rlgl.h"
#include "rlights.h"

#include "external/rlImGui/rlImGui.h"
#include "ImGuiFileDialog.h"

#include "external/glm/glm.hpp"

#include "../entities/TerrainEntity.hpp"
#include "../entities/RockEntity.hpp"

#include "../shapers/Shaper.hpp"
#include "../shapers/SubdivisionShaper.hpp"

SceneEditorState::SceneEditorState() :
openRenamePopup(false) {

}

void SceneEditorState::init(std::shared_ptr<Context>& p_context) {
    SetWindowSize(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
    SetWindowPosition(0, 0);
    p_context->states->setWindowState(NONE);

    ToggleFullscreen();

    // lights init
    typeToAdd = -1;

    // render/draw variables init
    sceneTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    gizmoTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    selectedLight = -1;
    curr_m = "SOLID";
    onSelectionMeshColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    onSelectionWiresColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    entityToAdd = -1;
    isGizmoDragged = false;
    isGizmoHovered = false;
    shader = p_context->shaders->getShader(SOLID);

    // scene controls init
    chunkSize = 2.5f;
    showGrid = true;

    // void colors init
    voidColSol = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    voidColMat = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    voidColRen = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);
    voidColWir = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);

    // solid shader init
    colorChanged = false;
    dirChanged = false;

    // mesh controls init
    positionChanged = false;
    showWires = false;
    toggleWireframe = false;

    // program settings init
    useStrictSearch = false;
    showEdgeNormals = false;
    showVertexNormals = false;
    showFaceNormals = false;
    length = 0.1f;
    normalsColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    showUV = false;

    shouldOpenContextPopup = false;
    contextForEntity = true;
    hoverDelay = 1.0f;

    p_context->shaders->handleSetShaderValue(SOLID, p_context);
    p_context->customCamera->init();
}

void SceneEditorState::process(std::shared_ptr<Context>& p_context) {

    p_context->customCamera->update();

    int btn;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        btn = MOUSE_BUTTON_LEFT;
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        btn = MOUSE_BUTTON_RIGHT;
    }
    if (!isGizmoDragged && !isGizmoHovered) HandleMouseSelection(btn, p_context->selectedEntity, shouldOpenContextPopup, *p_context->customCamera->getCamera(), p_context, ray);

    p_context->shaders->handleShaderSelection(p_context->currentSh);

    if (p_context->currentSh == RENDER) p_context->shaders->updateCamPos(p_context);

    static bool assignedTexture = false;
    static int indexAssignedTo = -1;
    if (showUV && (p_context->selectedEntity >= 0 && p_context->selectedEntity < p_context->entities.size())) {
        if (!assignedTexture) {
            Texture2D checker = uvController.GenerateCheckerTexture();
            Texture2D curr = p_context->entities.at(p_context->selectedEntity)->e_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture;
            uvController.swapTexture(curr, checker);
            p_context->entities.at(p_context->selectedEntity)->e_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = checker;
            indexAssignedTo = p_context->selectedEntity;
            assignedTexture = true;
        }
    } else if (!showUV && (indexAssignedTo >= 0 && indexAssignedTo < p_context->entities.size())) {
        p_context->entities.at(indexAssignedTo)->e_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = uvController.swappedTexture;
        assignedTexture = false;
    }
}

void SceneEditorState::draw(std::shared_ptr<Context>& p_context) {
    if (WindowShouldClose()) {
        p_context->states->setWindowState(EXIT);
        return;
    }

    BeginDrawing();

    // -----------------
    //   SCENE DRAWING
    // -----------------
    BeginTextureMode(sceneTexture);

    p_context->shaders->updateLightValues(p_context);

    p_context->shaders->handleBackgroundClearing(p_context->currentSh);

    // apply shader to all entities
    if (!p_context->entities.empty()) {
        for (const auto& entityPtr : p_context->entities) {
            Entity& e = *entityPtr;
            if (p_context->currentSh == SOLID) {
                shader = p_context->shaders->getShader(SOLID);
                p_context->shaders->handleSetShaderValue(SOLID, p_context);
                e.e_model.materials[0].shader = *shader;
            } else if (p_context->currentSh == M_PREVIEW || p_context->currentSh == WIREFRAME) {
                shader = p_context->shaders->getShader(M_PREVIEW);
                e.e_model.materials[0].shader = *shader;
            } else {
                shader = p_context->shaders->getShader(RENDER);
                p_context->shaders->handleSetShaderValue(RENDER, p_context);
                e.e_model.materials[0].shader = *shader;
            }
        }
    }

    if (colorChanged) p_context->shaders->changeColor();
    if (dirChanged) p_context->shaders->changeDirection();

    BeginMode3D(*p_context->customCamera->getCamera());

    // draw entities
    for (int i = 0; i < p_context->entities.size(); i++) {
        if (!p_context->entities.at(i)->e_visible) continue;

        Vector3 epos(p_context->entities.at(i)->e_position[0], p_context->entities.at(i)->e_position[1], p_context->entities.at(i)->e_position[2]);

        if (p_context->selectedEntity == i) {
            if ((p_context->currentSh != WIREFRAME && !toggleWireframe) || p_context->currentSh == SOLID) DrawModel(p_context->entities.at(i)->e_model, epos, 1.0f, ImVecToColor(onSelectionMeshColor));
            if (p_context->currentSh != SOLID) DrawModelWires(p_context->entities.at(i)->e_model, epos, 1.0f, ImVecToColor(onSelectionWiresColor));
        } else {
            if ((p_context->currentSh != WIREFRAME && !toggleWireframe) || p_context->currentSh == SOLID) DrawModel(p_context->entities.at(i)->e_model, epos, 1.0f, p_context->entities.at(i)->e_color);
            if (p_context->currentSh != SOLID && (showWires || toggleWireframe || p_context->currentSh == WIREFRAME)) DrawModelWires(p_context->entities.at(i)->e_model, epos, 1.0f, RED);
        }
    }

    if ((selectedLight >= 0 && selectedLight < p_context->llights.size()) && p_context->llights.at(selectedLight)->_l_light.enabled) {
        DrawSphere(p_context->llights.at(selectedLight)->_l_light.position, 0.5f, ImVecToColor(onSelectionMeshColor));
    }

    if (showGrid && p_context->currentSh != RENDER) DrawGrid(100, chunkSize);

    EndMode3D();

    // ------------------
    // END SCENE DRAWING
    // ------------------
    EndTextureMode();

    // ---------------
    //  GIZMO DRAWING
    // ---------------
    BeginTextureMode(gizmoTexture);
    ClearBackground((Color){0, 0, 0, 0});

    BeginMode3D(*p_context->customCamera->getCamera());
    rlDisableDepthTest();

    if (p_context->selectedEntity >= 0 && p_context->selectedEntity < p_context->entities.size()) {
        if (showEdgeNormals) normalController.DrawEdgeNormals(*p_context->entities.at(p_context->selectedEntity)->e_mesh, length, ImVecToColor(normalsColor));
        if (showFaceNormals) normalController.DrawFaceNormals(*p_context->entities.at(p_context->selectedEntity)->e_mesh, length, ImVecToColor(normalsColor));
        if (showVertexNormals) normalController.DrawVertexNormals(*p_context->entities.at(p_context->selectedEntity)->e_mesh, length, ImVecToColor(normalsColor));
    }

    static int dragAxis; // X = 0, Y = 1, Z = 2
    static glm::vec3 dragStartEntityPos;
    static glm::vec3 dragStartHitPoint;

    if (p_context->selectedEntity >= 0 && p_context->selectedEntity < p_context->entities.size()) {
        float size = 0.05f * (p_context->customCamera->getDistance() /  5.0f);

        float lineLength = size * 8.0f;

        constexpr Vector3 rlAxisX = { 1, 0, 0 };
        constexpr Vector3 rlAxisY = { 0, 1, 0 };
        constexpr Vector3 rlAxisZ = { 0, 0, 1 };

        constexpr glm::vec3 glAxisX = { 1, 0, 0 };
        constexpr glm::vec3 glAxisY = { 0, 1, 0 };
        constexpr glm::vec3 glAxisZ = { 0, 0, 1 };

        Vector3 rlEpos(p_context->entities.at(p_context->selectedEntity)->e_position[0], p_context->entities.at(p_context->selectedEntity)->e_position[1], p_context->entities.at(p_context->selectedEntity)->e_position[2]);
        glm::vec3 glEpos(rlEpos.x, rlEpos.y, rlEpos.z);

        BoundingBox boxX;
        float boxThickness = 0.5f * size;
        boxX.min = { fminf(rlEpos.x, rlEpos.x + lineLength) - boxThickness, rlEpos.y - boxThickness, rlEpos.z - boxThickness };
        boxX.max = { fmaxf(rlEpos.x, rlEpos.x + lineLength) + boxThickness, rlEpos.y + boxThickness, rlEpos.z + boxThickness };
        BoundingBox boxY;
        boxY.min = { rlEpos.x - boxThickness, fminf(rlEpos.y, rlEpos.y + lineLength) - boxThickness, rlEpos.z - boxThickness };
        boxY.max = { rlEpos.x + boxThickness, fmaxf(rlEpos.y, rlEpos.y + lineLength) + boxThickness, rlEpos.z + boxThickness };
        BoundingBox boxZ;
        boxZ.min = { rlEpos.x - boxThickness, rlEpos.y - boxThickness, fminf(rlEpos.z, rlEpos.z + lineLength) - boxThickness };
        boxZ.max = { rlEpos.x + boxThickness, rlEpos.y + boxThickness, fmaxf(rlEpos.z, rlEpos.z + lineLength) + boxThickness };

        DrawLine3D(rlEpos, Vector3Add(rlEpos, Vector3Scale(rlAxisX, lineLength)), RED);
        DrawCube(Vector3Add(rlEpos, Vector3Scale(rlAxisX, lineLength)), size, size, size, RED);

        DrawLine3D(rlEpos, Vector3Add(rlEpos, Vector3Scale(rlAxisY, lineLength)), GREEN);
        DrawCube(Vector3Add(rlEpos, Vector3Scale(rlAxisY, lineLength)), size, size, size, GREEN);

        DrawLine3D(rlEpos, Vector3Add(rlEpos, Vector3Scale(rlAxisZ, lineLength)), BLUE);
        DrawCube(Vector3Add(rlEpos, Vector3Scale(rlAxisZ, lineLength)), size, size, size, BLUE);

        Ray gizmoRay = GetScreenToWorldRay(GetMousePosition(), *p_context->customCamera->getCamera());
        Vector2 mousePos = GetMousePosition();

        RayCollision hitX = GetRayCollisionBox(gizmoRay, boxX);
        RayCollision hitY = GetRayCollisionBox(gizmoRay, boxY);
        RayCollision hitZ = GetRayCollisionBox(gizmoRay, boxZ);

        glm::vec3 closestPoint;

        isGizmoHovered = GetRayCollisionBox(gizmoRay, boxX).hit || GetRayCollisionBox(gizmoRay, boxY).hit || GetRayCollisionBox(gizmoRay, boxZ).hit;

        if (!isGizmoDragged && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (hitX.hit) {
                isGizmoDragged = true;
                dragAxis = 0; // X axis = 0
                dragStartEntityPos = glEpos;
                dragStartHitPoint = glm::vec3(hitX.point.x, hitX.point.y, hitX.point.z);
            } else if (hitY.hit) {
                isGizmoDragged = true;
                dragAxis = 1; // Y axis = 1
                dragStartEntityPos = glEpos;
                dragStartHitPoint = glm::vec3(hitY.point.x, hitY.point.y, hitY.point.z);
            } else if (hitZ.hit) {
                isGizmoDragged = true;
                dragAxis = 2; // Z axis = 2
                dragStartEntityPos = glEpos;
                dragStartHitPoint = glm::vec3(hitZ.point.x, hitZ.point.y, hitZ.point.z);
            }
        }

        if (isGizmoDragged && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Ray ray = GetScreenToWorldRay(GetMousePosition(), *p_context->customCamera->getCamera());
            glm::vec3 rayOrigin = glm::vec3(ray.position.x, ray.position.y, ray.position.z);
            glm::vec3 rayDirection = glm::normalize(glm::vec3(ray.direction.x, ray.direction.y, ray.direction.z));

            glm::vec3 axisDirection;
            if (dragAxis == 0) axisDirection = glAxisX;
            else if (dragAxis == 1) axisDirection = glAxisY;
            else if (dragAxis == 2) axisDirection = glAxisZ;

            glm::vec3 planeNormal = glm::cross(glm::cross(rayDirection, axisDirection), axisDirection);
            float denominator = glm::dot(rayDirection, planeNormal);

            if (fabs(denominator) > 1e-6f) {
                float t = glm::dot(dragStartHitPoint - rayOrigin, planeNormal) / denominator;
                glm::vec3 hitPoint = rayOrigin + rayDirection * t;

                float offset = glm::dot(hitPoint - dragStartHitPoint, axisDirection);
                glm::vec3 newPos = dragStartEntityPos + axisDirection * offset;

                p_context->entities.at(p_context->selectedEntity)->e_position[0] = newPos.x;
                p_context->entities.at(p_context->selectedEntity)->e_position[1] = newPos.y;
                p_context->entities.at(p_context->selectedEntity)->e_position[2] = newPos.z;

                p_context->entities.at(p_context->selectedEntity)->e_boundingBox = p_context->entities.at(p_context->selectedEntity)->GenMeshBoundingBox(*p_context->entities.at(p_context->selectedEntity)->e_mesh, p_context->entities.at(p_context->selectedEntity)->e_position);
            }
        }

        if (isGizmoDragged && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isGizmoDragged = false;
            dragAxis = -1;
        }
    }

    rlEnableDepthTest();
    EndMode3D();

    // --------------------
    //  END GIZMO DRAWING
    // --------------------
    EndTextureMode();

    Rectangle srcRect = { 0, 0, static_cast<float>(GetScreenWidth()), -static_cast<float>(GetScreenHeight()) };
    Rectangle dstRect = { 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) };

    DrawTexturePro(sceneTexture.texture, srcRect, dstRect, {0, 0}, 0.0f, WHITE);
    DrawTexturePro(gizmoTexture.texture, srcRect, dstRect, {0, 0}, 0.0f, WHITE);


    // -------------------------------
    //             UI
    // -------------------------------

    rlImGuiBegin();

    static float startHoverFreq = 0.0f;
    static float startHoverAmp = 0.0f;

    static float startHoverOct = 0.0f;
    static float startHoverPer = 0.0f;
    static float startHoverLac = 0.0f;

    static char searchBuffer[40] = "";
    static bool inputActive = false;
    static bool shouldFocus = false;

    static bool shouldUpdateBuffers = false;

    static bool searchEnterPressed = false;
    static bool searchJustActivated = false;

    const char* shapers[] = { "", "Subdivision" };
    static int selectedShaper = 0;

    const char* noiseTypes[] = { "Simple Pattern", "Fractal Noise", "Value Noise", "Perlin Noise" };
    const char* rockTypes[] = { "Basic Rock" };
    static int selectedNoiseType = 0;
    static int selectedRockType = 0;

    static const char* categories[] = { "General", "Camera", "Rendering", "Scene", "Colors" };
    static int selectedCategory = 0;

    static bool maxLightsPopupOpen = false;
    static bool settingsPopupOpen = false;

    int code = p_context->ui->DrawMainMenuBar(p_context, p_context->currentSh);

    if (code != -1) {
        if (code == OPEN_SETTINGS) ImGui::OpenPopup("Settings");
        else if (code == ADD_TERRAIN) entityToAdd = TERRAIN;
        else if (code == ADD_ROCK) entityToAdd = ROCK;
        else if (code == ADD_POINT) typeToAdd = LIGHT_POINT;
        else if (code == ADD_DIRECTIONAL) typeToAdd = LIGHT_DIRECTIONAL;
        else if (code == OPEN_EXPORTER) ImGui::OpenPopup("Exporter");
    }

    // ---------
    // SETTINGS
    // ---------
    if (ImGui::BeginPopupModal("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {

        ImGui::Dummy(ImVec2(0, 2.5f));
        p_context->fontMgr.setXL();
        ImGui::Text("Preferences");
        ImGui::PopFont();

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(390, 0));
        ImGui::SameLine();

        if (ImGui::Button("X")) {
            ImGui::CloseCurrentPopup();
            settingsPopupOpen = false;
        }
        ImGui::Dummy(ImVec2(0, 5));

        ImGui::BeginChild("Sidebar", ImVec2(150, 300), true, ImGuiWindowFlags_NoScrollbar);

        for (int i = 0; i < IM_ARRAYSIZE(categories); i++) {
            if (ImGui::Selectable(categories[i], selectedCategory == i)) {
                selectedCategory = i;
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("SettingsContent", ImVec2(400, 300), true);

        switch (selectedCategory) {
            case 0: {
                ImGui::Checkbox("Strict Search", &useStrictSearch);
                break;
            }
            case 1: {
                static float newZoom = 2.0f;
                static float newPan = 0.001f;
                static float newRot = 0.002f;

                bool changedZ = p_context->uiManager->FloatInput("Zoom Speed", newZoom, true, 1.0f, 50.0f);
                if (changedZ) p_context->customCamera->changeZoomSpeed(newZoom);

                bool changedP = p_context->uiManager->FloatInput("Pan Speed", newPan, true, 0.0001f, 0.01f);
                if (changedP) p_context->customCamera->changePanSpeed(newPan);

                bool changedR = p_context->uiManager->FloatInput("Rotation Speed", newRot, true, 0.001f, 0.01f);
                if (changedR) p_context->customCamera->changeRotateSpeed(newRot);

                break;
            }
            case 2: {
                ImGui::Text("Solid Shader");
                colorChanged = ImGui::ColorEdit3("##SolidLightColor", p_context->shaders->lightColor);
                ImGui::Dummy(ImVec2(0, 2.5f));
                ImGui::Text("Light Direction");
                dirChanged = ImGui::SliderFloat3("##SolidLightDirection", p_context->shaders->lightDirection, -1.0f, 1.0f);

                break;
            }
            case 3: {
                ImGui::Checkbox("Show Vertex Normals", &showVertexNormals);
                ImGui::Checkbox("Show Face Normals", &showFaceNormals);
                ImGui::Checkbox("Show Edge Normals", &showEdgeNormals);
                if (!(showEdgeNormals || showFaceNormals || showVertexNormals)) ImGui::BeginDisabled();
                p_context->uiManager->FloatSlider("Normals Length", length, 0.1f, 5.0f);
                if (!(showEdgeNormals || showFaceNormals || showVertexNormals)) ImGui::EndDisabled();
                ImGui::Checkbox("Check UV (selected entity)", &showUV);
                break;
            }
            case 4: {
                ImGui::ColorEdit3("Selection Mesh", reinterpret_cast<float*>(&onSelectionMeshColor));
                ImGui::ColorEdit3("Selection Wires", reinterpret_cast<float*>(&onSelectionWiresColor));
                break;
            }
            default: {
                break;
            }
        }

        ImGui::EndChild();

        ImGui::EndPopup();
    }

    // -------------
    //   EXPORTER
    // -------------
    if (ImGui::BeginPopupModal("Exporter", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
        if (!(p_context->selectedEntity >= 0 && p_context->selectedEntity < p_context->entities.size())) {
            p_context->fontMgr.setXL();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning!");
            ImGui::PopFont();

            ImGui::SameLine();
            ImGui::Dummy(ImVec2(50, 0));
            ImGui::SameLine();
            if (ImGui::Button("X")) {
                ImGui::CloseCurrentPopup();
            }

            p_context->uiManager->Section("Please select an entity first to export it!");

            ImGui::EndPopup();

        } else {
            static std::string path = "";
            static char buffer[30] = "";

            p_context->fontMgr.setXL();
            ImGui::Text("Entity Exporter");
            ImGui::PopFont();

            ImGui::SameLine();
            ImGui::Dummy(ImVec2(80, 0));
            ImGui::SameLine();
            if (ImGui::Button("X")) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::Dummy(ImVec2(0, 5));

            if (ImGui::Button("Select Save Location")) {
                ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose Directory", nullptr);
            }

            if (ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey")) {
                if (ImGuiFileDialog::Instance()->IsOk()) {

                    path = ImGuiFileDialog::Instance()->GetCurrentPath();
                }
                ImGuiFileDialog::Instance()->Close();
            }

            if (path.empty() && buffer[0] == '\0') ImGui::BeginDisabled();
            ImGui::Text("Name");
            ImGui::InputText("##Name", buffer, IM_ARRAYSIZE(buffer));

            if (ImGui::Button("Export")) {
                exporter.ExportOBJ(p_context->entities.at(p_context->selectedEntity), path + '/' + buffer + ".obj");
                ImGui::CloseCurrentPopup();
            }
            if (path.empty() && buffer[0] == '\0') ImGui::EndDisabled();

            ImGui::EndPopup();
        }
    }

    // -----------
    // ENTITY ADD
    // -----------
    if (typeToAdd != -1) {

        if (p_context->shaders->currLightsCount >= p_context->shaders->MAX_LIGHTS_COUNT) {
            maxLightsPopupOpen = true;

        } else {
            Vector3 pos = { 10.0f, 10.0f, 10.0f };
            Vector3 target = { 0.0f, 0.0f, 0.0f };
            Color color = WHITE;
            bool reused = false;

            for (auto& lightPtr : p_context->llights) {
                if (lightPtr->deleted) {
                    lightPtr->deleted = false;
                    lightPtr->_l_light.type = typeToAdd;
                    lightPtr->_l_light.position = pos;
                    lightPtr->_l_light.color = color;
                    lightPtr->_l_light.target = target;
                    lightPtr->_l_light.enabled = true;
                    lightPtr->name = typeToAdd == LIGHT_POINT ? "point light" : "directional light";

                    UpdateLightValues(*p_context->shaders->getShader(RENDER), lightPtr->_l_light);
                    p_context->shaders->currLightsCount++;
                    reused = true;
                    break;
                }
            }

            if (!reused) {
                Light rawLight = CreateLight(typeToAdd, pos, target, color, *p_context->shaders->getShader(RENDER));
                p_context->llights.push_back(std::make_unique<lLight>(typeToAdd == LIGHT_POINT ? "point light" : "directional light", rawLight));
                p_context->shaders->currLightsCount++;
            }

            p_context->shaders->updateLights();
        }

        typeToAdd = -1;
    }

    if (maxLightsPopupOpen) {
        ImGui::OpenPopup("Warning");
    }

    if (ImGui::BeginPopupModal("Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
        ImGui::Text("Max lights reached! Sorry for the inconvenience");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            maxLightsPopupOpen = false;
        }
        ImGui::EndPopup();
    }

    if (entityToAdd != -1) {
        ImGui::OpenPopup("AddEntity");
        if (ImGui::BeginPopupModal("AddEntity", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
            if (entityToAdd == TERRAIN) {

                bool terrainCreated = false;
                static float width = 10;
                static float length = 10;
                static float resX = 20;
                static float resZ = 20;

                p_context->fontMgr.setXL();
                ImGui::Text("Terrain Settings");
                ImGui::PopFont();

                ImGui::Dummy(ImVec2(0, 2.5f));

                ImGui::PushItemWidth(340);

                ImGui::PopItemWidth();

                p_context->fontMgr.setLG();
                ImGui::Text("General");
                ImGui::PopFont();

                ImGui::Dummy(ImVec2(0, 2.5f));
                ImGui::Text("Width");
                ImGui::SameLine();

                ImGui::Dummy(ImVec2(125, 0));
                ImGui::SameLine();
                ImGui::Text("Resolution X");

                ImGui::PushItemWidth(165);
                ImGui::InputFloat("##Width", &width);

                ImGui::SameLine();
                ImGui::InputFloat("##Resolution X", &resX);

                ImGui::Dummy(ImVec2(0, 2.5f));
                ImGui::Text("Length");
                ImGui::SameLine();

                ImGui::Dummy(ImVec2(125, 0));
                ImGui::SameLine();
                ImGui::Text("Resolution Z");

                ImGui::InputFloat("##Length", &length);

                ImGui::SameLine();
                ImGui::InputFloat("##Resolution Z", &resZ);
                ImGui::PopItemWidth();

                ImGui::Dummy(ImVec2(0, 5));

                if (!terrainCreated) {
                    if (ImGui::Button("Create")) {

                        TerrainEntity* _terrain;
                        p_context->entities.emplace_back(std::make_unique<TerrainEntity>(GenMeshPlane(width, length, resX, resZ), "terrain", "terrain"));
                        p_context->selectedEntity = static_cast<int>(p_context->entities.size() - 1);
                        p_context->entities.at(p_context->selectedEntity)->UpdateBuffers();
                        ImGui::CloseCurrentPopup();
                        entityToAdd = -1;
                        _terrain = dynamic_cast<TerrainEntity*>(p_context->entities.at(p_context->selectedEntity).get());
                        _terrain->noiseType = "simple";
                        terrainCreated = true;
                    }
                }
            // ----------------
            //      ROCK
            // ----------------
            } else if (entityToAdd == ROCK) {
                static RockEntity* _rock;
                if (p_context->selectedEntity >= 0 && p_context->selectedEntity < p_context->entities.size()) {

                    if (p_context->entities.at(p_context->selectedEntity)->e_type == "rock") {
                        _rock = dynamic_cast<RockEntity*>(p_context->entities.at(p_context->selectedEntity).get());
                    }
                } else {
                    _rock = nullptr;
                }

                static bool pushed = false;

                if (!pushed) {
                    p_context->entities.emplace_back(std::make_unique<RockEntity>(customMeshes.GenMeshIcosahedron(), "rock", "rock"));
                    p_context->selectedEntity = static_cast<int>(p_context->entities.size() - 1);
                    _rock = dynamic_cast<RockEntity*>(p_context->entities.at(p_context->selectedEntity).get());
                    p_context->entities.at(p_context->selectedEntity)->e_boundingBox = p_context->entities.at(p_context->selectedEntity)->GenMeshBoundingBox(*p_context->entities.at(p_context->selectedEntity)->e_mesh, p_context->entities.at(p_context->selectedEntity)->e_position);
                    pushed = true;
                }

                p_context->fontMgr.setXL();
                ImGui::Text("Rock Settings");
                ImGui::PopFont();

                ImGui::Dummy(ImVec2(0, 2.5f));

                p_context->entities.at(p_context->selectedEntity)->UpdateBuffers();

                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Additional settings not necessary!");
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Everything provided after creating the mesh!");

                ImGui::Dummy(ImVec2(0, 5));

                if (ImGui::Button("Create")) {
                    ImGui::CloseCurrentPopup();
                    entityToAdd = -1;
                    pushed = false;
                }
            }
            ImGui::EndPopup();
        }
    }

    // --------------
    // STATE TAB BAR
    // --------------
    p_context->ui->DrawStateBar(p_context, p_context->currentSh, SCENE);

    int mw = GetScreenWidth();
    int mh = GetScreenHeight();

    float menuHeight = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(ImVec2(mw - 400, menuHeight), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, mh - menuHeight), ImGuiCond_Once);

    ImGui::Begin("Scene Manager", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration);

    // --------------
    //    TAB BAR
    // --------------
    ImGui::BeginTabBar("UITabBar", ImGuiTabBarFlags_Reorderable);

    static int currentState = 0;
    const char* names[] = { "General", "Presets", "Performance & Diagnostics" };

    for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
        if (ImGui::BeginTabItem(names[i])) {
            currentState = i;
            switch (i) {
                case 0:
                case 1:
                case 2:
                default: break;
            }
            ImGui::EndTabItem();
        }
    }
    ImGui::EndTabBar();

    // ---------------
    // SCENE ENTITIES
    // ---------------
    ImGui::PushItemWidth(200);
    p_context->uiManager->Section("Scene Entities", p_context->fontMgr.getXXL());

    ImGui::PushItemWidth(380);

    p_context->fontMgr.setLG();
    if (!inputActive) {
        if (ImGui::Selectable("Search entity...")) {
            inputActive = true;
            shouldFocus = true;
        }
    }

    if (inputActive) {
        if (shouldFocus) {
            ImGui::SetKeyboardFocusHere();
            shouldFocus = false;
            searchJustActivated = true;
        }
        searchEnterPressed = ImGui::InputText("##SearchInput", searchBuffer, IM_ARRAYSIZE(searchBuffer), ImGuiInputTextFlags_EnterReturnsTrue);

        if (searchBuffer[0] == '\0' && searchEnterPressed) inputActive = false;
        if (searchBuffer[0] == '\0' && !searchJustActivated && !ImGui::IsItemActive() && !ImGui::IsItemHovered()) inputActive = false;

        searchJustActivated = false;
    }
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0, 2.5f));

    ImGui::PopItemWidth();

    if (p_context->entities.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No entities found!");
    }

    std::string substringToSearch = searchBuffer;

    for (int i = 0; i < p_context->entities.size(); i++) {
        const std::string& entityName = p_context->entities.at(i)->e_name;

        if (!substringToSearch.empty()) {
            if (!useStrictSearch) {
                if (entityName.find(substringToSearch) == std::string::npos) continue;
            } else {
                if (entityName.compare(0, strlen(searchBuffer), searchBuffer) != 0) continue;
            }
        }

        ImGui::PushID("Entity" + i);
        if (ImGui::Selectable(p_context->entities.at(i)->e_name.c_str(), p_context->selectedEntity == i)) {
            p_context->selectedEntity = i;
            selectedLight = -1;
        }
        ImGui::PopID();

        if ((ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) || shouldOpenContextPopup) {
            if (!shouldOpenContextPopup) p_context->selectedEntity = i;
            shouldOpenContextPopup = false;
            ImGui::OpenPopup("Context");
            contextForEntity = true;
        }
    }
    if (p_context->llights.empty()) {
        p_context->fontMgr.setMD();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No lights found!");
        ImGui::PopFont();
    } else {
        for (int i = 0; i < p_context->llights.size(); i++) {
            if (p_context->llights.at(i)->deleted) continue;
            ImGui::PushID("Light" + i);
            if (ImGui::Selectable(p_context->llights.at(i)->name.c_str(), selectedLight == i)) {
                selectedLight = i;
                p_context->selectedEntity = -1;
            }
            ImGui::PopID();

            if ((ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) || shouldOpenContextPopup) {
                if (!shouldOpenContextPopup) selectedLight = i;
                shouldOpenContextPopup = false;
                ImGui::OpenPopup("Context");
                contextForEntity = false;
            }
        }
    }
    // ENTITY OPTIONS
    if (ImGui::BeginPopup("Context")) {
        if (contextForEntity) {
            if (ImGui::MenuItem("Rename")) {
                if (p_context->selectedEntity >= 0 && p_context->selectedEntity < p_context->entities.size()) {

                    strncpy(renameBuffer, p_context->entities.at(p_context->selectedEntity)->e_name.c_str(), sizeof(renameBuffer));
                    renameBuffer[sizeof(renameBuffer) - 1] = '\0';
                    openRenamePopup = true;
                }
            }
            std::string label = p_context->entities.at(p_context->selectedEntity)->e_visible ? "Hide" : "Show";
            if (ImGui::MenuItem(label.c_str())) {
                p_context->entities.at(p_context->selectedEntity)->e_visible = !p_context->entities.at(p_context->selectedEntity)->e_visible;
            }
            if (ImGui::MenuItem("Delete")) {
                if (p_context->selectedEntity >= 0 && p_context->selectedEntity < p_context->entities.size()) {

                    std::unique_ptr<Entity>& entity = p_context->entities.at(p_context->selectedEntity);

                    for (auto& shaper : entity->e_shapers) {
                        delete shaper;
                    }

                    UnloadModel(entity->e_model);

                    p_context->entities.erase(p_context->entities.begin() + p_context->selectedEntity);
                    p_context->selectedEntity = -1;
                }
            }
        } else {
            if (ImGui::MenuItem("Rename")) {
                if (selectedLight >= 0 && selectedLight < p_context->llights.size()) {

                    strncpy(renameBuffer, p_context->llights.at(selectedLight)->name.c_str(), sizeof(renameBuffer));
                    renameBuffer[sizeof(renameBuffer) - 1] = '\0';
                    openRenamePopup = true;
                }
            }
            std::string label = p_context->llights.at(selectedLight)->_l_light.enabled ? "Disable" : "Enable";
            if (ImGui::MenuItem(label.c_str())) {
                p_context->llights.at(selectedLight)->_l_light.enabled = !p_context->llights.at(selectedLight)->_l_light.enabled;
                p_context->shaders->updateSingleLight(p_context, selectedLight);
            }
            if (ImGui::MenuItem("Delete")) {
                if (selectedLight >= 0 && selectedLight < p_context->llights.size()) {

                    int lastIndex = p_context->shaders->currLightsCount - 1;

                    if (selectedLight != lastIndex) {
                        std::swap(p_context->llights.at(selectedLight), p_context->llights.at(lastIndex));

                        char uniformName[64];
                        sprintf(uniformName, "lights[%d].enabled", selectedLight);
                        p_context->llights.at(selectedLight)->_l_light.enabledLoc = GetShaderLocation(*p_context->shaders->getShader(RENDER), uniformName);

                        sprintf(uniformName, "lights[%d].type", selectedLight);
                        p_context->llights.at(selectedLight)->_l_light.typeLoc = GetShaderLocation(*p_context->shaders->getShader(RENDER), uniformName);

                        sprintf(uniformName, "lights[%d].position", selectedLight);
                        p_context->llights.at(selectedLight)->_l_light.positionLoc = GetShaderLocation(*p_context->shaders->getShader(RENDER), uniformName);

                        sprintf(uniformName, "lights[%d].target", selectedLight);
                        p_context->llights.at(selectedLight)->_l_light.targetLoc = GetShaderLocation(*p_context->shaders->getShader(RENDER), uniformName);

                        sprintf(uniformName, "lights[%d].color", selectedLight);
                        p_context->llights.at(selectedLight)->_l_light.colorLoc = GetShaderLocation(*p_context->shaders->getShader(RENDER), uniformName);

                        p_context->shaders->updateSingleLight(p_context, selectedLight);
                    }

                    p_context->llights.at(lastIndex)->deleted = true;
                    p_context->llights.at(lastIndex)->_l_light.enabled = false;

                    p_context->shaders->updateSingleLight(p_context, lastIndex);

                    selectedLight = -1;
                    p_context->shaders->currLightsCount--;

                    p_context->shaders->updateLights();
                }
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
        bool renameEnterPressed = ImGui::InputText("##RenameInput", renameBuffer, sizeof(renameBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        static bool showEmptyRenameWarning = false;
        ImGui::SameLine();
        if (ImGui::Button("OK") || renameEnterPressed) {
            if (contextForEntity) {
                if (p_context->selectedEntity >= 0 && p_context->selectedEntity < p_context->entities.size()) {
                    if (renameBuffer[0] == '\0') {
                        showEmptyRenameWarning = true;
                    } else {
                        showEmptyRenameWarning = false;
                        p_context->entities.at(p_context->selectedEntity)->e_name = std::string(renameBuffer);
                        ImGui::CloseCurrentPopup();
                    }
                }
            } else {
                if (selectedLight >= 0 && selectedLight < p_context->llights.size()) {
                    if (renameBuffer[0] == '\0') {
                        showEmptyRenameWarning = true;
                    } else {
                        showEmptyRenameWarning = false;
                        p_context->llights.at(selectedLight)->name = std::string(renameBuffer);
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
        }
        if (showEmptyRenameWarning) {
            ImGui::SameLine();
            ImGui::SetCursorPosX(110);
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Name cannot be empty!");
        }
        ImGui::EndPopup();
    }
    // ENTITY SETTINGS
    if (p_context->selectedEntity >= 0 && p_context->selectedEntity < p_context->entities.size()) {
        p_context->uiManager->Section("Entity Settings", p_context->fontMgr.getXL());

        p_context->uiManager->Section("General", p_context->fontMgr.getLG(), 0);
        ImGui::Dummy(ImVec2(0, 5));

        ImGui::Text("Entity Color");
        ImGui::ColorEdit3("Mesh color", reinterpret_cast<float*>(&p_context->entities.at(p_context->selectedEntity)->e_colorValues));
        p_context->entities.at(p_context->selectedEntity)->e_color = p_context->entities.at(p_context->selectedEntity)->ImVecToColor(p_context->entities.at(p_context->selectedEntity)->e_colorValues);

        ImGui::Dummy(ImVec2(0, 2.5f));
        ImGui::Text("Entity Position");
        ImGui::Text("X");
        ImGui::SameLine();
        positionChanged |= ImGui::InputFloat("##EntityPosXInput", &p_context->entities.at(p_context->selectedEntity)->e_position[0], 0.5f);
        ImGui::Text("Y");
        ImGui::SameLine();
        positionChanged |= ImGui::InputFloat("##EntityPosYInput", &p_context->entities.at(p_context->selectedEntity)->e_position[1], 0.5f);
        ImGui::Text("Z");
        ImGui::SameLine();
        positionChanged |= ImGui::InputFloat("##EntityPosZInput", &p_context->entities.at(p_context->selectedEntity)->e_position[2], 0.5f);

        if (positionChanged) {
            positionChanged = false;
            p_context->entities.at(p_context->selectedEntity)->e_boundingBox = p_context->entities.at(p_context->selectedEntity)->GenMeshBoundingBox(*p_context->entities.at(p_context->selectedEntity)->e_mesh, p_context->entities.at(p_context->selectedEntity)->e_position);
        }

        ImGui::Dummy(ImVec2(0, 2.5f));
        ImGui::Checkbox("Toggle Wireframe", &toggleWireframe);
        if (toggleWireframe) showWires = false;
        ImGui::BeginDisabled(toggleWireframe);
        ImGui::Checkbox("Show Wires", &showWires);
        ImGui::EndDisabled();

        p_context->uiManager->Section("Shapers", p_context->fontMgr.getLG());

        ImGui::Text("Choose Shaper");
        ImGui::Combo("##ChooseShaper", &selectedShaper, shapers, IM_ARRAYSIZE(shapers));

        if (selectedShaper != 0) {
            if (!checkIfHasShaper(typeid(SubdivisionShaper), p_context)) {
                p_context->entities.at(p_context->selectedEntity)->e_shapers.push_back(new SubdivisionShaper(p_context->entities.at(p_context->selectedEntity).get(), p_context->entities.at(p_context->selectedEntity)->e_type != "terrain"));
            }

            Shaper* baseShaper = p_context->entities.at(p_context->selectedEntity)->e_shapers.at(0);
            auto* _subdivisionShaper = dynamic_cast<SubdivisionShaper*>(baseShaper);

            std::string subLabel = "Subdivision level: " + std::to_string(_subdivisionShaper->subdivisions);
            ImGui::Text(subLabel.c_str());

            ImGui::Dummy(ImVec2(0, 2.5f));

            bool shouldDisableSubdivision = _subdivisionShaper->subdivisions >= 3;

            if (shouldDisableSubdivision) ImGui::BeginDisabled();
            if (ImGui::Button("Subdivide")) {
                p_context->entities.at(p_context->selectedEntity)->e_shapers.at(0)->Apply(p_context->entities.at(p_context->selectedEntity));
                p_context->entities.at(p_context->selectedEntity)->UpdateBuffers();

            }
            if (shouldDisableSubdivision) ImGui::EndDisabled();
        }

        if (p_context->entities.at(p_context->selectedEntity)->e_type == "terrain") {
            auto* terrain = dynamic_cast<TerrainEntity*>(p_context->entities.at(p_context->selectedEntity).get());

            noise.frequency = terrain->frequency;
            noise.amplitude = terrain->amplitude;
            noise.lacunarity = terrain->lacunarity;
            noise.octaves = terrain->octaves;
            noise.persistence = terrain->persistence;

            static bool makeFractal = false;

            if (p_context->entities.at(p_context->selectedEntity)->e_regenSeed) {
                p_context->entities.at(p_context->selectedEntity)->e_regenSeed = false;
                int vertexCount = static_cast<int>(p_context->entities.at(p_context->selectedEntity)->e_vertices.size() / 3);
                for (int i = 0; i < vertexCount; i++) {
                    float x = p_context->entities.at(p_context->selectedEntity)->e_vertices[i * 3];
                    float z = p_context->entities.at(p_context->selectedEntity)->e_vertices[i * 3 + 2];

                    if (terrain->noiseType == "simple") {
                        p_context->entities.at(p_context->selectedEntity)->e_vertices[i * 3 + 1] = noise.getSimplePatternTerrain(x, z, p_context->entities.at(p_context->selectedEntity)->e_seedEnable);

                    } else if (terrain->noiseType == "octave") {
                        p_context->entities.at(p_context->selectedEntity)->e_vertices[i * 3 + 1] = noise.getFractalNoiseTerrain(x, z, p_context->entities.at(p_context->selectedEntity)->e_seedEnable);

                    } else if (terrain->noiseType == "value") {
                        p_context->entities.at(p_context->selectedEntity)->e_vertices[i * 3 + 1] = noise.getValueNoiseTerrain(x, z, p_context->entities.at(p_context->selectedEntity)->e_seedEnable, makeFractal);

                    } else if (terrain->noiseType == "perlin") {
                        p_context->entities.at(p_context->selectedEntity)->e_vertices[i * 3 + 1] = noise.getPerlinNoiseTerrain(x, z, p_context->entities.at(p_context->selectedEntity)->e_seedEnable, makeFractal);

                    }
                }
            }

            p_context->uiManager->Section("Noise", p_context->fontMgr.getLG());

            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::Text("Choose Noise Type");
            ImGui::Combo("##ChooseNoiseType", &selectedNoiseType, noiseTypes, IM_ARRAYSIZE(noiseTypes));

            if (selectedNoiseType == 0) {
                terrain->noiseType = "simple";

            } else if (selectedNoiseType == 1) {
                terrain->noiseType = "octave";

            } else if (selectedNoiseType == 2) {
                terrain->noiseType = "value";

            } else if (selectedNoiseType == 3) {
                terrain->noiseType = "perlin";

            }

            shouldUpdateBuffers |= p_context->uiManager->FloatSlider("Frequency", terrain->frequency, 0.01f, 1.0f);
            p_context->uiManager->SetItemTooltip("Frequency controls how fast the waves change", startHoverFreq, hoverDelay);

            shouldUpdateBuffers |= p_context->uiManager->FloatSlider("Amplitude", terrain->amplitude, 0.1f, 10.0f);
            p_context->uiManager->SetItemTooltip("Amplitude controls how tall the bumps are", startHoverAmp);

            if (selectedNoiseType == 2 || selectedNoiseType == 3) {
                ImGui::Dummy(ImVec2(0, 2.5f));
                ImGui::Checkbox("Make Fractal", &makeFractal);
            }

            if (selectedNoiseType == 1 || ((selectedNoiseType == 2 || selectedNoiseType == 3) && makeFractal)) {

                shouldUpdateBuffers |= p_context->uiManager->FloatSlider("Lacunarity", terrain->lacunarity, 0.01f, 10.0f);
                p_context->uiManager->SetItemTooltip("Lacunarity increases frequency each octave", startHoverLac, hoverDelay);

                shouldUpdateBuffers |= p_context->uiManager->FloatSlider("Persistence", terrain->persistence, 0.01f, 1.0f);
                p_context->uiManager->SetItemTooltip("Persistence reduces amplitude each octave", startHoverPer, hoverDelay);

                shouldUpdateBuffers |= p_context->uiManager->IntSlider("Octaves", terrain->octaves, 1, 12);
                p_context->uiManager->SetItemTooltip("Octaves control how many laters of detail are", startHoverOct);

                if (selectedNoiseType == 1) {
                    ImGui::Dummy(ImVec2(0, 2.5f));
                    ImGui::Checkbox("Use Improved Noise", &noise.improvedFakeNoise);
                }
                if (selectedNoiseType == 3) {
                    ImGui::Dummy(ImVec2(0, 2.5f));
                    ImGui::Checkbox("Enable Terracing", &noise.enableTerracing);
                }

            }
            if (noise.enableTerracing) {
                p_context->uiManager->FloatSlider("Terrace", noise.terraceSteps, 1.0f, 20.0f);
            }
            int& seedVal = p_context->entities.at(p_context->selectedEntity)->e_seed;

            p_context->uiManager->Section("Seed", p_context->fontMgr.getLG());

            ImGui::Dummy(ImVec2(0, 5));
            ImGui::Checkbox("Use Seed ##UseSeedChb", &p_context->entities.at(p_context->selectedEntity)->e_seedEnable);

            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::Text("Seed Value");
            bool shouldUpdateSeedValue = ImGui::InputInt("##SeedVal", &seedVal);
            if (seedVal < -10000) seedVal = -10000;
            if (seedVal > 10000) seedVal = 10000;
            if (shouldUpdateSeedValue) noise.updateSeedValue(seedVal);

            ImGui::Dummy(ImVec2(0, 2.5f));
            if (ImGui::Button("Generate New Seed")) {
                p_context->entities.at(p_context->selectedEntity)->e_seed = noise.genNewSeedValue();
                p_context->entities.at(p_context->selectedEntity)->e_regenSeed = true;
                shouldUpdateBuffers = true;
            }

            if (shouldUpdateBuffers) {
                p_context->entities.at(p_context->selectedEntity)->e_regenSeed = true;
            }

            ImGui::PopItemWidth();
        } else if (p_context->entities.at(p_context->selectedEntity)->e_type == "rock") {
            auto* rock = dynamic_cast<RockEntity*>(p_context->entities.at(p_context->selectedEntity).get());

            noise.frequency = rock->frequency;
            noise.amplitude = rock->amplitude;

            if (p_context->entities.at(p_context->selectedEntity)->e_regenSeed) {
                int vertexCount = static_cast<int>(p_context->entities.at(p_context->selectedEntity)->e_vertices.size() / 3);
                for (int i = 0; i < vertexCount; i++) {
                    float v0 = p_context->entities.at(p_context->selectedEntity)->e_vertices[i * 3];
                    float v1 = p_context->entities.at(p_context->selectedEntity)->e_vertices[i * 3 + 1];
                    float v2 = p_context->entities.at(p_context->selectedEntity)->e_vertices[i * 3 + 2];

                    glm::vec3 v(v0, v1, v2);

                    v = noise.getSimplePatternRock(v, p_context->entities.at(p_context->selectedEntity)->e_seedEnable);

                    p_context->entities.at(p_context->selectedEntity)->e_vertices[i * 3] = v.x;
                    p_context->entities.at(p_context->selectedEntity)->e_vertices[i * 3 + 1] = v.y;
                    p_context->entities.at(p_context->selectedEntity)->e_vertices[i * 3 + 2] = v.z;
                }
            }

            ImGui::Dummy(ImVec2(0, 5));
            p_context->fontMgr.setLG();
            ImGui::Text("Noise");
            ImGui::PopFont();

            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::Text("Frequency");
            shouldUpdateBuffers |= ImGui::SliderFloat("##RockFrequency", &rock->frequency, 0.01f, 1.0f);
            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::Text("Amplitude");
            shouldUpdateBuffers |= ImGui::SliderFloat("##RockAmplitude", &rock->amplitude, 0.10f, 1.0f);

            p_context->entities.at(p_context->selectedEntity)->UpdateBuffers();

            int& seedVal = p_context->entities.at(p_context->selectedEntity)->e_seed;

            p_context->uiManager->Section("Seed", p_context->fontMgr.getLG());

            ImGui::Dummy(ImVec2(0, 5));
            ImGui::Checkbox("Use Seed ##UseSeedChb", &p_context->entities.at(p_context->selectedEntity)->e_seedEnable);

            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::Text("Seed Value");
            bool shouldUpdateSeedValue = ImGui::InputInt("##SeedVal", &seedVal);
            if (seedVal < -10000) seedVal = -10000;
            if (seedVal > 10000) seedVal = 10000;
            if (shouldUpdateSeedValue) noise.updateSeedValue(seedVal);

            ImGui::Dummy(ImVec2(0, 2.5f));
            if (ImGui::Button("Generate New Seed")) {
                p_context->entities.at(p_context->selectedEntity)->e_seed = noise.genNewSeedValue();
                p_context->entities.at(p_context->selectedEntity)->e_regenSeed = true;
            }

            if (shouldUpdateBuffers) {
                p_context->entities.at(p_context->selectedEntity)->e_regenSeed = true;
            }

            ImGui::PopItemWidth();
        }
    }
    // SCENE SETTINGS
    ImGui::Dummy(ImVec2(0, 5));
    p_context->fontMgr.setXL();
    ImGui::Text("Scene settings");
    ImGui::PopFont();

    ImGui::Dummy(ImVec2(0, 5));
    ImGui::Checkbox("Show grid", &showGrid);
    ImGui::Dummy(ImVec2(0, 2.5f));
    ImGui::BeginDisabled(!showGrid);
    ImGui::Text("Chunk Size");
    ImGui::PushItemWidth(300);
    ImGui::SliderFloat("##ChunkSizeSlider", &chunkSize, 0.5f, 100.0f);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(50);
    ImGui::InputFloat("##ChunkSizeInput", &chunkSize);
    ImGui::PopItemWidth();
    if (chunkSize < 0.5f) chunkSize = 0.5f;
    if (chunkSize > 100.0f) chunkSize = 100.0f;
    ImGui::EndDisabled();

    static bool updateSolid = false;
    static bool updatePreview = false;
    static bool updateRender = false;
    static bool updateWireframe = false;

    ImGui::Dummy(ImVec2(0, 5));
    p_context->fontMgr.setLG();
    ImGui::Text("Void Colors");
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0, 2.5f));
    ImGui::Text("Solid");
    updateSolid = ImGui::ColorEdit3("##VoidColorSolidEdit", reinterpret_cast<float *>(&voidColSol));
    ImGui::Text("Material Preview");
    updatePreview = ImGui::ColorEdit3("##VoidColorMaterialPreviewEdit", reinterpret_cast<float *>(&voidColMat));
    ImGui::Text("Render");
    updateRender = ImGui::ColorEdit3("##VoidColorRenderEdit", reinterpret_cast<float *>(&voidColRen));
    ImGui::Text("Wireframe");
    updateWireframe = ImGui::ColorEdit3("##VoidColorWireframeEdit", reinterpret_cast<float *>(&voidColWir));

    if (updateSolid) p_context->shaders->voidColSolid = ImVecToColor(voidColSol);
    else if (updatePreview) p_context->shaders->voidColPreview = ImVecToColor(voidColMat);
    else if (updateRender) p_context->shaders->voidColRender = ImVecToColor(voidColRen);
    else if (updateWireframe) p_context->shaders->voidColWireframe = ImVecToColor(voidColWir);

    if (shouldUpdateBuffers && (p_context->selectedEntity >= 0 && p_context->selectedEntity < p_context->entities.size())) {
        p_context->entities.at(p_context->selectedEntity)->UpdateBuffers();
        shouldUpdateBuffers = false;
    }

    ImGui::End();
    rlImGuiEnd();

    // std::string text = "Current mode: " + curr_m;
    // DrawText(text.c_str(), 20, 30, 20, BLACK);

    EndDrawing();
}

void SceneEditorState::clean(std::shared_ptr<Context>& p_context) {
    for (auto& entityPtr : p_context->entities) {
        Entity& e = *entityPtr;

        for (auto& shaper : e.e_shapers) {
            delete shaper;
        }

        UnloadModel(e.e_model);
    }

    CloseWindow();
}

Color SceneEditorState::ImVecToColor(ImVec4 _color) {
    return (Color){
        static_cast<unsigned char>(_color.x * 255.0f),
        static_cast<unsigned char>(_color.y * 255.0f),
        static_cast<unsigned char>(_color.z * 255.0f),
        255
    };
}

void SceneEditorState::HandleMouseSelection(const int& btn, int& selectedEntity, bool& e_context, const Camera3D& _camera, const std::shared_ptr<Context>& _m_context, Ray _ray) {
    if (ImGui::GetIO().WantCaptureMouse || !IsMouseButtonPressed(btn)) return;

    _ray = GetMouseRay(GetMousePosition(), _camera);
    selectedEntity = -1;
    float closestHit = FLT_MAX;

    for (int i = 0; i < _m_context->entities.size(); i++) {
        if (!_m_context->entities.at(i)->e_visible) continue;

        RayCollision hit = GetRayCollisionBox(_ray, _m_context->entities.at(i)->e_boundingBox);

        if (hit.hit && hit.distance < closestHit) {
            closestHit = hit.distance;
            selectedEntity = i;
            selectedLight = -1;
        }
    }
    if (btn == MOUSE_BUTTON_RIGHT && (selectedEntity >= 0 && selectedEntity < _m_context->entities.size())) {
        e_context = true;
    }
}

bool SceneEditorState::checkIfHasShaper(const std::type_info &type, std::shared_ptr<Context>& p_context) const {
    for (const auto* shaper : p_context->entities.at(p_context->selectedEntity)->e_shapers) {
        if (typeid(*shaper) == type) return true;
    }
    return false;
}
