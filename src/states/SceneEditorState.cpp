#include "SceneEditorState.hpp"
#define RLIGHTS_IMPLEMENTATION
#include "raymath.h"
#include "rlgl.h"
#include "rlights.h"

#include "external/rlImGui/rlImGui.h"

#include "external/glm/glm.hpp"

#include "../entities/TerrainEntity.hpp"
#include "../entities/RockEntity.hpp"

#include "../shapers/Shaper.hpp"
#include "../shapers/SubdivisionShaper.hpp"

SceneEditorState::SceneEditorState() :
openRenamePopup(false) {

}

void SceneEditorState::init(std::shared_ptr<Context>& m_context) {
    SetWindowSize(1920, 1080);
    SetWindowPosition(0, 0);
    m_context->states->setWindowState(NONE);

    // lights init
    typeToAdd = -1;

    // render/draw variables init
    sceneTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    gizmoTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    selectedEntity = -1;
    selectedLight = -1;
    currentSh = 0;
    curr_m = "SOLID";
    onSelectionMeshColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    onSelectionWiresColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    entityToAdd = -1;
    isGizmoDragged = false;
    isGizmoHovered = false;
    shader = m_context->shaders->getShader(SOLID);

    // camera related variables init
    zoomSpeed = 1.0f;
    distance = 10.0f;

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

    // collision (to check if the entity was hit) init
    Ray ray = { 0 };

    shouldOpenContextPopup = false;
    contextForEntity = true;
    hoverDelay = 1.0f;

    m_context->shaders->handleSetShaderValue(SOLID, m_context);

    for (auto& e : m_context->entities) {
        e->UpdateBuffers();
    }
}

void SceneEditorState::process(std::shared_ptr<Context>& m_context) {
    if (!ImGui::GetIO().WantCaptureMouse) {
        distance -= GetMouseWheelMove() * zoomSpeed;

        Vector3 camPos = {
            0 + distance,
            0 + distance,
            0 + distance
        };
        m_context->camera->position = camPos;
    }

    int btn;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        btn = MOUSE_BUTTON_LEFT;
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        btn = MOUSE_BUTTON_RIGHT;
    }
    if (!isGizmoDragged && !isGizmoHovered) HandleMouseSelection(btn, selectedEntity, shouldOpenContextPopup, *m_context->camera, m_context, ray);

    m_context->shaders->handleShaderSelection(currentSh);

    if (currentSh == RENDER) m_context->shaders->updateCamPos(m_context);

    static bool assignedTexture = false;
    static int indexAssignedTo = -1;
    if (showUV && (selectedEntity >= 0 && selectedEntity < m_context->entities.size())) {
        if (!assignedTexture) {
            Texture2D checker = uvController.GenerateCheckerTexture();
            Texture2D curr = m_context->entities.at(selectedEntity)->e_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture;
            uvController.swapTexture(curr, checker);
            m_context->entities.at(selectedEntity)->e_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = checker;
            indexAssignedTo = selectedEntity;
            assignedTexture = true;
        }
    } else if (!showUV && (indexAssignedTo >= 0 && indexAssignedTo < m_context->entities.size())) {
        m_context->entities.at(indexAssignedTo)->e_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = uvController.swappedTexture;
        assignedTexture = false;
    }
}

void SceneEditorState::draw(std::shared_ptr<Context>& m_context) {
    if (WindowShouldClose()) {
        m_context->states->setWindowState(EXIT);
        return;
    }

    BeginDrawing();

    // -----------------
    //   SCENE DRAWING
    // -----------------
    BeginTextureMode(sceneTexture);

    m_context->shaders->updateLightValues(m_context);

    m_context->shaders->handleBackgroundClearing(currentSh);

    // apply shader to all entities
    if (!m_context->entities.empty()) {
        for (const auto& entityPtr : m_context->entities) {
            Entity& e = *entityPtr;
            if (currentSh == SOLID) {
                shader = m_context->shaders->getShader(SOLID);
                m_context->shaders->handleSetShaderValue(SOLID, m_context);
                e.e_model.materials[0].shader = *shader;
            } else if (currentSh == M_PREVIEW || currentSh == WIREFRAME) {
                shader = m_context->shaders->getShader(M_PREVIEW);
                e.e_model.materials[0].shader = *shader;
            } else {
                shader = m_context->shaders->getShader(RENDER);
                m_context->shaders->handleSetShaderValue(RENDER, m_context);
                e.e_model.materials[0].shader = *shader;
            }
        }
    }

    if (colorChanged) m_context->shaders->changeColor();
    if (dirChanged) m_context->shaders->changeDirection();

    BeginMode3D(*m_context->camera);

    // draw entities
    for (int i = 0; i < m_context->entities.size(); i++) {
        if (!m_context->entities.at(i)->e_visible) continue;

        Vector3 epos(m_context->entities.at(i)->e_position[0], m_context->entities.at(i)->e_position[1], m_context->entities.at(i)->e_position[2]);

        if (selectedEntity == i) {
            if ((currentSh != WIREFRAME && !toggleWireframe) || currentSh == SOLID) DrawModel(m_context->entities.at(i)->e_model, epos, 1.0f, ImVecToColor(onSelectionMeshColor));
            if (currentSh != SOLID) DrawModelWires(m_context->entities.at(i)->e_model, epos, 1.0f, ImVecToColor(onSelectionWiresColor));
        } else {
            if ((currentSh != WIREFRAME && !toggleWireframe) || currentSh == SOLID) DrawModel(m_context->entities.at(i)->e_model, epos, 1.0f, m_context->entities.at(i)->e_color);
            if (currentSh != SOLID && (showWires || toggleWireframe || currentSh == WIREFRAME)) DrawModelWires(m_context->entities.at(i)->e_model, epos, 1.0f, RED);
        }
    }

    if ((selectedLight >= 0 && selectedLight < m_context->llights.size()) && m_context->llights.at(selectedLight)->_l_light.enabled) {
        DrawSphere(m_context->llights.at(selectedLight)->_l_light.position, 0.5f, ImVecToColor(onSelectionMeshColor));
    }

    if (showGrid && currentSh != RENDER) DrawGrid(100, chunkSize);

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

    BeginMode3D(*m_context->camera);
    rlDisableDepthTest();

    if (selectedEntity >= 0 && selectedEntity < m_context->entities.size()) {
        if (showEdgeNormals) normalController.DrawEdgeNormals(*m_context->entities.at(selectedEntity)->e_mesh, length, ImVecToColor(normalsColor));
        if (showFaceNormals) normalController.DrawFaceNormals(*m_context->entities.at(selectedEntity)->e_mesh, length, ImVecToColor(normalsColor));
        if (showVertexNormals) normalController.DrawVertexNormals(*m_context->entities.at(selectedEntity)->e_mesh, length, ImVecToColor(normalsColor));
    }

    static int dragAxis; // X = 0, Y = 1, Z = 2
    static glm::vec3 dragStartEntityPos;
    static glm::vec3 dragStartHitPoint;

    if (selectedEntity >= 0 && selectedEntity < m_context->entities.size()) {
        float size = 0.05f * (distance /  5.0f);

        float lineLength = size * 8.0f;

        constexpr Vector3 rlAxisX = { 1, 0, 0 };
        constexpr Vector3 rlAxisY = { 0, 1, 0 };
        constexpr Vector3 rlAxisZ = { 0, 0, 1 };

        constexpr glm::vec3 glAxisX = { 1, 0, 0 };
        constexpr glm::vec3 glAxisY = { 0, 1, 0 };
        constexpr glm::vec3 glAxisZ = { 0, 0, 1 };

        Vector3 rlEpos(m_context->entities.at(selectedEntity)->e_position[0], m_context->entities.at(selectedEntity)->e_position[1], m_context->entities.at(selectedEntity)->e_position[2]);
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

        Ray gizmoRay = GetScreenToWorldRay(GetMousePosition(), *m_context->camera);
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
            Ray ray = GetScreenToWorldRay(GetMousePosition(), *m_context->camera);
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

                m_context->entities.at(selectedEntity)->e_position[0] = newPos.x;
                m_context->entities.at(selectedEntity)->e_position[1] = newPos.y;
                m_context->entities.at(selectedEntity)->e_position[2] = newPos.z;

                m_context->entities.at(selectedEntity)->e_boundingBox = m_context->entities.at(selectedEntity)->GenMeshBoundingBox(*m_context->entities.at(selectedEntity)->e_mesh, m_context->entities.at(selectedEntity)->e_position);
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

    int code = m_context->ui->DrawMainMenuBar(m_context, currentSh);

    if (code != -1) {
        if (code == OPEN_SETTINGS) ImGui::OpenPopup("Settings");
        else if (code == ADD_TERRAIN) entityToAdd = TERRAIN;
        else if (code == ADD_ROCK) entityToAdd = ROCK;
        else if (code == ADD_POINT) typeToAdd = LIGHT_POINT;
        else if (code == ADD_DIRECTIONAL) typeToAdd = LIGHT_DIRECTIONAL;
    }

    // ---------
    // SETTINGS
    // ---------
    if (ImGui::BeginPopupModal("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {

        ImGui::Dummy(ImVec2(0, 2.5f));
        m_context->fontMgr.setXL();
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
            case 0:
                ImGui::Checkbox("Strict Search", &useStrictSearch);
                break;
            case 1:
                m_context->uiManager->FloatInput("Zoom Speed", zoomSpeed, true, 1.0f, 50.0f);
                break;
            case 2:
                ImGui::Text("Solid Shader");
                colorChanged = ImGui::ColorEdit3("##SolidLightColor", m_context->shaders->lightColor);
                ImGui::Dummy(ImVec2(0, 2.5f));
                ImGui::Text("Light Direction");
                dirChanged = ImGui::SliderFloat3("##SolidLightDirection", m_context->shaders->lightDirection, -1.0f, 1.0f);

                break;
            case 3:
                ImGui::Checkbox("Show Vertex Normals", &showVertexNormals);
                ImGui::Checkbox("Show Face Normals", &showFaceNormals);
                ImGui::Checkbox("Show Edge Normals", &showEdgeNormals);
                if (!(showEdgeNormals || showFaceNormals || showVertexNormals)) ImGui::BeginDisabled();
                m_context->uiManager->FloatSlider("Normals Length", length, 0.1f, 5.0f);
                if (!(showEdgeNormals || showFaceNormals || showVertexNormals)) ImGui::EndDisabled();
                ImGui::Checkbox("Check UV (selected entity)", &showUV);
                break;
            case 4:
                ImGui::ColorEdit3("Selection Mesh", reinterpret_cast<float*>(&onSelectionMeshColor));
                ImGui::ColorEdit3("Selection Wires", reinterpret_cast<float*>(&onSelectionWiresColor));
                break;
            default:
                break;
        }

        ImGui::EndChild();

        ImGui::EndPopup();
    }

    // -----------
    // ENTITY ADD
    // -----------
    if (typeToAdd != -1) {

        if (m_context->shaders->currLightsCount >= m_context->shaders->MAX_LIGHTS_COUNT) {
            maxLightsPopupOpen = true;

        } else {
            Vector3 pos = { 10.0f, 10.0f, 10.0f };
            Vector3 target = { 0.0f, 0.0f, 0.0f };
            Color color = WHITE;
            bool reused = false;

            for (auto& lightPtr : m_context->llights) {
                if (lightPtr->deleted) {
                    lightPtr->deleted = false;
                    lightPtr->_l_light.type = typeToAdd;
                    lightPtr->_l_light.position = pos;
                    lightPtr->_l_light.color = color;
                    lightPtr->_l_light.target = target;
                    lightPtr->_l_light.enabled = true;
                    lightPtr->name = typeToAdd == LIGHT_POINT ? "point light" : "directional light";

                    UpdateLightValues(*m_context->shaders->getShader(RENDER), lightPtr->_l_light);
                    m_context->shaders->currLightsCount++;
                    reused = true;
                    break;
                }
            }

            if (!reused) {
                Light rawLight = CreateLight(typeToAdd, pos, target, color, *m_context->shaders->getShader(RENDER));
                m_context->llights.push_back(std::make_unique<lLight>(typeToAdd == LIGHT_POINT ? "point light" : "directional light", rawLight));
                m_context->shaders->currLightsCount++;
            }

            m_context->shaders->updateLights();
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
                static TerrainEntity* _terrain;
                if (selectedEntity >= 0 && selectedEntity < m_context->entities.size()) {

                    if (m_context->entities.at(selectedEntity)->e_type == "rock") {
                        _terrain = dynamic_cast<TerrainEntity*>(m_context->entities.at(selectedEntity).get());
                    }
                } else {
                    _terrain = nullptr;
                }

                static bool pushed = false;
                static float width = 10;
                static float length = 10;
                static float resX = 20;
                static float resZ = 20;
                static bool shouldUpdate = false;

                if (!pushed || shouldUpdate) {
                    if (shouldUpdate) m_context->entities.pop_back();
                    shouldUpdate = false;
                    m_context->entities.emplace_back(std::make_unique<TerrainEntity>(GenMeshPlane(width, length, resX, resZ), "terrain", "terrain"));
                    selectedEntity = static_cast<int>(m_context->entities.size() - 1);
                    _terrain = dynamic_cast<TerrainEntity*>(m_context->entities.at(selectedEntity).get());
                    _terrain->noiseType = "perlin";
                    pushed = true;
                }

                m_context->fontMgr.setXL();
                ImGui::Text("Terrain Settings");
                ImGui::PopFont();

                ImGui::Dummy(ImVec2(0, 2.5f));

                m_context->entities.at(selectedEntity)->UpdateBuffers();
                ImGui::PushItemWidth(340);

                ImGui::PopItemWidth();

                m_context->fontMgr.setLG();
                ImGui::Text("General");
                ImGui::PopFont();

                ImGui::Dummy(ImVec2(0, 2.5f));
                ImGui::Text("Width");
                ImGui::SameLine();

                ImGui::Dummy(ImVec2(125, 0));
                ImGui::SameLine();
                ImGui::Text("Resolution X");

                ImGui::PushItemWidth(165);
                shouldUpdate |= ImGui::InputFloat("##Width", &width);

                ImGui::SameLine();
                shouldUpdate |= ImGui::InputFloat("##Resolution X", &resX);

                ImGui::Dummy(ImVec2(0, 2.5f));
                ImGui::Text("Length");
                ImGui::SameLine();

                ImGui::Dummy(ImVec2(125, 0));
                ImGui::SameLine();
                ImGui::Text("Resolution Z");

                shouldUpdate |= ImGui::InputFloat("##Length", &length);

                ImGui::SameLine();
                shouldUpdate |= ImGui::InputFloat("##Resolution Z", &resZ);
                ImGui::PopItemWidth();

                ImGui::Dummy(ImVec2(0, 5));

                if (ImGui::Button("Create")) {
                    ImGui::CloseCurrentPopup();
                    entityToAdd = -1;
                    pushed = false;
                }
            // ----------------
            //      ROCK
            // ----------------
            } else if (entityToAdd == ROCK) {
                static RockEntity* _rock;
                if (selectedEntity >= 0 && selectedEntity < m_context->entities.size()) {

                    if (m_context->entities.at(selectedEntity)->e_type == "rock") {
                        _rock = dynamic_cast<RockEntity*>(m_context->entities.at(selectedEntity).get());
                    }
                } else {
                    _rock = nullptr;
                }

                static bool pushed = false;

                if (!pushed) {
                    m_context->entities.emplace_back(std::make_unique<RockEntity>(customMeshes.GenMeshIcosahedron(), "rock", "rock"));
                    selectedEntity = static_cast<int>(m_context->entities.size() - 1);
                    _rock = dynamic_cast<RockEntity*>(m_context->entities.at(selectedEntity).get());
                    m_context->entities.at(selectedEntity)->e_boundingBox = m_context->entities.at(selectedEntity)->GenMeshBoundingBox(*m_context->entities.at(selectedEntity)->e_mesh, m_context->entities.at(selectedEntity)->e_position);
                    pushed = true;
                }

                m_context->fontMgr.setXL();
                ImGui::Text("Rock Settings");
                ImGui::PopFont();

                ImGui::Dummy(ImVec2(0, 2.5f));

                m_context->entities.at(selectedEntity)->UpdateBuffers();

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
    m_context->ui->DrawStateBar(m_context, currentSh, SCENE);

    int mw = GetScreenWidth();
    int mh = GetScreenHeight();

    float menuHeight = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(ImVec2(mw - 400, menuHeight), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, 1080 - menuHeight), ImGuiCond_Once);

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
    m_context->uiManager->Section("Scene Entities", m_context->fontMgr.getXXL());

    ImGui::PushItemWidth(380);

    m_context->fontMgr.setLG();
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

    if (m_context->entities.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No entities found!");
    }

    std::string substringToSearch = searchBuffer;

    for (int i = 0; i < m_context->entities.size(); i++) {
        const std::string& entityName = m_context->entities.at(i)->e_name;

        if (!substringToSearch.empty()) {
            if (!useStrictSearch) {
                if (entityName.find(substringToSearch) == std::string::npos) continue;
            } else {
                if (entityName.compare(0, strlen(searchBuffer), searchBuffer) != 0) continue;
            }
        }

        ImGui::PushID("Entity" + i);
        if (ImGui::Selectable(m_context->entities.at(i)->e_name.c_str(), selectedEntity == i)) {
            selectedEntity = i;
            selectedLight = -1;
        }
        ImGui::PopID();

        if ((ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) || shouldOpenContextPopup) {
            if (!shouldOpenContextPopup) selectedEntity = i;
            shouldOpenContextPopup = false;
            ImGui::OpenPopup("Context");
            contextForEntity = true;
        }
    }
    if (m_context->llights.empty()) {
        m_context->fontMgr.setMD();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No lights found!");
        ImGui::PopFont();
    } else {
        for (int i = 0; i < m_context->llights.size(); i++) {
            if (m_context->llights.at(i)->deleted) continue;
            ImGui::PushID("Light" + i);
            if (ImGui::Selectable(m_context->llights.at(i)->name.c_str(), selectedLight == i)) {
                selectedLight = i;
                selectedEntity = -1;
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
                if (selectedEntity >= 0 && selectedEntity < m_context->entities.size()) {

                    strncpy(renameBuffer, m_context->entities.at(selectedEntity)->e_name.c_str(), sizeof(renameBuffer));
                    renameBuffer[sizeof(renameBuffer) - 1] = '\0';
                    openRenamePopup = true;
                }
            }
            std::string label = m_context->entities.at(selectedEntity)->e_visible ? "Hide" : "Show";
            if (ImGui::MenuItem(label.c_str())) {
                m_context->entities.at(selectedEntity)->e_visible = !m_context->entities.at(selectedEntity)->e_visible;
            }
            if (ImGui::MenuItem("Delete")) {
                if (selectedEntity >= 0 && selectedEntity < m_context->entities.size()) {

                    std::unique_ptr<Entity>& entity = m_context->entities.at(selectedEntity);

                    for (auto& shaper : entity->e_shapers) {
                        delete shaper;
                    }

                    UnloadModel(entity->e_model);

                    m_context->entities.erase(m_context->entities.begin() + selectedEntity);
                    selectedEntity = -1;
                }
            }
        } else {
            if (ImGui::MenuItem("Rename")) {
                if (selectedLight >= 0 && selectedLight < m_context->llights.size()) {

                    strncpy(renameBuffer, m_context->llights.at(selectedLight)->name.c_str(), sizeof(renameBuffer));
                    renameBuffer[sizeof(renameBuffer) - 1] = '\0';
                    openRenamePopup = true;
                }
            }
            std::string label = m_context->llights.at(selectedLight)->_l_light.enabled ? "Disable" : "Enable";
            if (ImGui::MenuItem(label.c_str())) {
                m_context->llights.at(selectedLight)->_l_light.enabled = !m_context->llights.at(selectedLight)->_l_light.enabled;
                m_context->shaders->updateSingleLight(m_context, selectedLight);
            }
            if (ImGui::MenuItem("Delete")) {
                if (selectedLight >= 0 && selectedLight < m_context->llights.size()) {

                    int lastIndex = m_context->shaders->currLightsCount - 1;

                    if (selectedLight != lastIndex) {
                        std::swap(m_context->llights.at(selectedLight), m_context->llights.at(lastIndex));

                        char uniformName[64];
                        sprintf(uniformName, "lights[%d].enabled", selectedLight);
                        m_context->llights.at(selectedLight)->_l_light.enabledLoc = GetShaderLocation(*m_context->shaders->getShader(RENDER), uniformName);

                        sprintf(uniformName, "lights[%d].type", selectedLight);
                        m_context->llights.at(selectedLight)->_l_light.typeLoc = GetShaderLocation(*m_context->shaders->getShader(RENDER), uniformName);

                        sprintf(uniformName, "lights[%d].position", selectedLight);
                        m_context->llights.at(selectedLight)->_l_light.positionLoc = GetShaderLocation(*m_context->shaders->getShader(RENDER), uniformName);

                        sprintf(uniformName, "lights[%d].target", selectedLight);
                        m_context->llights.at(selectedLight)->_l_light.targetLoc = GetShaderLocation(*m_context->shaders->getShader(RENDER), uniformName);

                        sprintf(uniformName, "lights[%d].color", selectedLight);
                        m_context->llights.at(selectedLight)->_l_light.colorLoc = GetShaderLocation(*m_context->shaders->getShader(RENDER), uniformName);

                        m_context->shaders->updateSingleLight(m_context, selectedLight);
                    }

                    m_context->llights.at(lastIndex)->deleted = true;
                    m_context->llights.at(lastIndex)->_l_light.enabled = false;

                    m_context->shaders->updateSingleLight(m_context, lastIndex);

                    selectedLight = -1;
                    m_context->shaders->currLightsCount--;

                    m_context->shaders->updateLights();
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
                if (selectedEntity >= 0 && selectedEntity < m_context->entities.size()) {
                    if (renameBuffer[0] == '\0') {
                        showEmptyRenameWarning = true;
                    } else {
                        showEmptyRenameWarning = false;
                        m_context->entities.at(selectedEntity)->e_name = std::string(renameBuffer);
                        ImGui::CloseCurrentPopup();
                    }
                }
            } else {
                if (selectedLight >= 0 && selectedLight < m_context->llights.size()) {
                    if (renameBuffer[0] == '\0') {
                        showEmptyRenameWarning = true;
                    } else {
                        showEmptyRenameWarning = false;
                        m_context->llights.at(selectedLight)->name = std::string(renameBuffer);
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
    if (selectedEntity >= 0 && selectedEntity < m_context->entities.size()) {
        m_context->uiManager->Section("Entity Settings", m_context->fontMgr.getXL());

        m_context->uiManager->Section("General", m_context->fontMgr.getLG(), 0);
        ImGui::Dummy(ImVec2(0, 5));

        ImGui::Text("Entity Color");
        ImGui::ColorEdit3("Mesh color", reinterpret_cast<float*>(&m_context->entities.at(selectedEntity)->e_colorValues));
        m_context->entities.at(selectedEntity)->e_color = m_context->entities.at(selectedEntity)->ImVecToColor(m_context->entities.at(selectedEntity)->e_colorValues);

        ImGui::Dummy(ImVec2(0, 2.5f));
        ImGui::Text("Entity Position");
        ImGui::Text("X");
        ImGui::SameLine();
        positionChanged |= ImGui::InputFloat("##EntityPosXInput", &m_context->entities.at(selectedEntity)->e_position[0], 0.5f);
        ImGui::Text("Y");
        ImGui::SameLine();
        positionChanged |= ImGui::InputFloat("##EntityPosYInput", &m_context->entities.at(selectedEntity)->e_position[1], 0.5f);
        ImGui::Text("Z");
        ImGui::SameLine();
        positionChanged |= ImGui::InputFloat("##EntityPosZInput", &m_context->entities.at(selectedEntity)->e_position[2], 0.5f);

        if (positionChanged) {
            positionChanged = false;
            m_context->entities.at(selectedEntity)->e_boundingBox = m_context->entities.at(selectedEntity)->GenMeshBoundingBox(*m_context->entities.at(selectedEntity)->e_mesh, m_context->entities.at(selectedEntity)->e_position);
        }

        ImGui::Dummy(ImVec2(0, 2.5f));
        ImGui::Checkbox("Toggle Wireframe", &toggleWireframe);
        if (toggleWireframe) showWires = false;
        ImGui::BeginDisabled(toggleWireframe);
        ImGui::Checkbox("Show Wires", &showWires);
        ImGui::EndDisabled();

        m_context->uiManager->Section("Shapers", m_context->fontMgr.getLG());

        ImGui::Text("Choose Shaper");
        ImGui::Combo("##ChooseShaper", &selectedShaper, shapers, IM_ARRAYSIZE(shapers));

        if (selectedShaper != 0) {
            if (!checkIfHasShaper(typeid(SubdivisionShaper), m_context)) {
                m_context->entities.at(selectedEntity)->e_shapers.push_back(new SubdivisionShaper(m_context->entities.at(selectedEntity).get(), m_context->entities.at(selectedEntity)->e_type != "terrain"));
            }

            Shaper* baseShaper = m_context->entities.at(selectedEntity)->e_shapers.at(0);
            auto* _subdivisionShaper = dynamic_cast<SubdivisionShaper*>(baseShaper);

            std::string subLabel = "Subdivision level: " + std::to_string(_subdivisionShaper->subdivisions);
            ImGui::Text(subLabel.c_str());

            ImGui::Dummy(ImVec2(0, 2.5f));

            bool shouldDisableSubdivision = _subdivisionShaper->subdivisions >= 3;

            if (shouldDisableSubdivision) ImGui::BeginDisabled();
            if (ImGui::Button("Subdivide")) {
                m_context->entities.at(selectedEntity)->e_shapers.at(0)->Apply(m_context->entities.at(selectedEntity));
                m_context->entities.at(selectedEntity)->UpdateBuffers();

            }
            if (shouldDisableSubdivision) ImGui::EndDisabled();
        }

        if (m_context->entities.at(selectedEntity)->e_type == "terrain") {
            auto* terrain = dynamic_cast<TerrainEntity*>(m_context->entities.at(selectedEntity).get());

            noise.frequency = terrain->frequency;
            noise.amplitude = terrain->amplitude;
            noise.lacunarity = terrain->lacunarity;
            noise.octaves = terrain->octaves;
            noise.persistence = terrain->persistence;

            static bool makeFractal = false;

            int vertexCount = static_cast<int>(m_context->entities.at(selectedEntity)->e_vertices.size() / 3);
            for (int i = 0; i < vertexCount; i++) {
                float x = m_context->entities.at(selectedEntity)->e_vertices[i * 3];
                float z = m_context->entities.at(selectedEntity)->e_vertices[i * 3 + 2];

                if (terrain->noiseType == "simple") {
                    m_context->entities.at(selectedEntity)->e_vertices[i * 3 + 1] = noise.getSimplePatternTerrain(x, z, m_context->entities.at(selectedEntity)->e_seedEnable);

                } else if (terrain->noiseType == "octave") {
                    m_context->entities.at(selectedEntity)->e_vertices[i * 3 + 1] = noise.getFractalNoiseTerrain(x, z, m_context->entities.at(selectedEntity)->e_seedEnable);

                } else if (terrain->noiseType == "value") {
                    m_context->entities.at(selectedEntity)->e_vertices[i * 3 + 1] = noise.getValueNoiseTerrain(x, z, m_context->entities.at(selectedEntity)->e_seedEnable, makeFractal);

                } else if (terrain->noiseType == "perlin") {
                    m_context->entities.at(selectedEntity)->e_vertices[i * 3 + 1] = noise.getPerlinNoiseTerrain(x, z, m_context->entities.at(selectedEntity)->e_seedEnable, makeFractal);

                }
            }

            m_context->uiManager->Section("Noise", m_context->fontMgr.getLG());

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

            shouldUpdateBuffers |= m_context->uiManager->FloatSlider("Frequency", terrain->frequency, 0.01f, 1.0f);
            m_context->uiManager->SetItemTooltip("Frequency controls how fast the waves change", startHoverFreq, hoverDelay);

            shouldUpdateBuffers |= m_context->uiManager->FloatSlider("Amplitude", terrain->amplitude, 0.1f, 10.0f);
            m_context->uiManager->SetItemTooltip("Amplitude controls how tall the bumps are", startHoverAmp);

            if (selectedNoiseType == 2 || selectedNoiseType == 3) {
                ImGui::Dummy(ImVec2(0, 2.5f));
                ImGui::Checkbox("Make Fractal", &makeFractal);
            }

            if (selectedNoiseType == 1 || ((selectedNoiseType == 2 || selectedNoiseType == 3) && makeFractal)) {

                shouldUpdateBuffers |= m_context->uiManager->FloatSlider("Lacunarity", terrain->lacunarity, 0.01f, 10.0f);
                m_context->uiManager->SetItemTooltip("Lacunarity increases frequency each octave", startHoverLac, hoverDelay);

                shouldUpdateBuffers |= m_context->uiManager->FloatSlider("Persistence", terrain->persistence, 0.01f, 1.0f);
                m_context->uiManager->SetItemTooltip("Persistence reduces amplitude each octave", startHoverPer, hoverDelay);

                shouldUpdateBuffers |= m_context->uiManager->IntSlider("Octaves", terrain->octaves, 1, 12);
                m_context->uiManager->SetItemTooltip("Octaves control how many laters of detail are", startHoverOct);

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
                m_context->uiManager->FloatSlider("Terrace", noise.terraceSteps, 1.0f, 20.0f);
            }
            int& seedVal = m_context->entities.at(selectedEntity)->e_seed;

            m_context->uiManager->Section("Seed", m_context->fontMgr.getLG());

            ImGui::Dummy(ImVec2(0, 5));
            ImGui::Checkbox("Use Seed ##UseSeedChb", &m_context->entities.at(selectedEntity)->e_seedEnable);

            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::Text("Seed Value");
            bool shouldUpdateSeedValue = ImGui::InputInt("##SeedVal", &seedVal);
            if (seedVal < -10000) seedVal = -10000;
            if (seedVal > 10000) seedVal = 10000;
            if (shouldUpdateSeedValue) noise.updateSeedValue(seedVal);

            ImGui::Dummy(ImVec2(0, 2.5f));
            if (ImGui::Button("Generate New Seed")) {
                m_context->entities.at(selectedEntity)->e_seed = noise.genNewSeedValue();
            }

            ImGui::PopItemWidth();
        } else if (m_context->entities.at(selectedEntity)->e_type == "rock") {
            auto* rock = dynamic_cast<RockEntity*>(m_context->entities.at(selectedEntity).get());

            noise.frequency = rock->frequency;
            noise.amplitude = rock->amplitude;

            int vertexCount = (int)m_context->entities.at(selectedEntity)->e_vertices.size() / 3;
            for (int i = 0; i < vertexCount; i++) {
                float v0 = m_context->entities.at(selectedEntity)->e_vertices[i * 3];
                float v1 = m_context->entities.at(selectedEntity)->e_vertices[i * 3 + 1];
                float v2 = m_context->entities.at(selectedEntity)->e_vertices[i * 3 + 2];

                glm::vec3 v(v0, v1, v2);

                v = noise.getSimplePatternRock(v, m_context->entities.at(selectedEntity)->e_seedEnable);

                m_context->entities.at(selectedEntity)->e_vertices[i * 3] = v.x;
                m_context->entities.at(selectedEntity)->e_vertices[i * 3 + 1] = v.y;
                m_context->entities.at(selectedEntity)->e_vertices[i * 3 + 2] = v.z;
            }

            ImGui::Dummy(ImVec2(0, 5));
            m_context->fontMgr.setLG();
            ImGui::Text("Noise");
            ImGui::PopFont();

            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::Text("Frequency");
            ImGui::SliderFloat("##RockFrequency", &rock->frequency, 0.01f, 1.0f);
            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::Text("Amplitude");
            ImGui::SliderFloat("##RockAmplitude", &rock->amplitude, 0.10f, 1.0f);

            m_context->entities.at(selectedEntity)->UpdateBuffers();

            int& seedVal = m_context->entities.at(selectedEntity)->e_seed;

            m_context->uiManager->Section("Seed", m_context->fontMgr.getLG());

            ImGui::Dummy(ImVec2(0, 5));
            ImGui::Checkbox("Use Seed ##UseSeedChb", &m_context->entities.at(selectedEntity)->e_seedEnable);

            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::Text("Seed Value");
            bool shouldUpdateSeedValue = ImGui::InputInt("##SeedVal", &seedVal);
            if (seedVal < -10000) seedVal = -10000;
            if (seedVal > 10000) seedVal = 10000;
            if (shouldUpdateSeedValue) noise.updateSeedValue(seedVal);

            ImGui::Dummy(ImVec2(0, 2.5f));
            if (ImGui::Button("Generate New Seed")) {
                m_context->entities.at(selectedEntity)->e_seed = noise.genNewSeedValue();
            }

            ImGui::PopItemWidth();
        }
    }
    // SCENE SETTINGS
    ImGui::Dummy(ImVec2(0, 5));
    m_context->fontMgr.setXL();
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
    m_context->fontMgr.setLG();
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

    if (updateSolid) m_context->shaders->voidColSolid = ImVecToColor(voidColSol);
    else if (updatePreview) m_context->shaders->voidColPreview = ImVecToColor(voidColMat);
    else if (updateRender) m_context->shaders->voidColRender = ImVecToColor(voidColRen);
    else if (updateWireframe) m_context->shaders->voidColWireframe = ImVecToColor(voidColWir);

    if (shouldUpdateBuffers && (selectedEntity >= 0 && selectedEntity < m_context->entities.size())) {
        m_context->entities.at(selectedEntity)->UpdateBuffers();
    }

    ImGui::End();
    rlImGuiEnd();

    // std::string text = "Current mode: " + curr_m;
    // DrawText(text.c_str(), 20, 30, 20, BLACK);

    EndDrawing();
}

void SceneEditorState::clean(std::shared_ptr<Context>& m_context) {
    for (auto& entityPtr : m_context->entities) {
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

bool SceneEditorState::checkIfHasShaper(const std::type_info &type, std::shared_ptr<Context>& m_context) const {
    for (const auto* shaper : m_context->entities.at(selectedEntity)->e_shapers) {
        if (typeid(*shaper) == type) return true;
    }
    return false;
}
