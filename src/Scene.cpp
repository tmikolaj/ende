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

    m_context->entities->push_back(Entity(GenMeshCube(1.0f, 1.0f, 1.0f), "cube", "none"));
    m_context->entities->push_back(Entity(GenMeshPlane(10, 10, 20, 20), "plane", "terrain"));

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
    lightDir = glm::normalize(glm::vec3{0.256f, -0.333f, 0.881f});
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
    showGrid = true;

    // void color init
    voidCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    // solid shader init
    colorChanged = false;
    dirChanged = false;

    // mesh controls init
    positionChanged = false;
    showWires = false;
    toggleWireframe = false;

    // program settings init
    useStrictSearch = false;

    // collision (to check if the entity was hit) init
    Ray ray = { 0 };

    SetShaderValue(solidShader, uBaseColorLoc, &lightColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(solidShader, uLightDirLoc, &lightDir[0], SHADER_UNIFORM_VEC3);

    for (auto& e : *m_context->entities) {
        e.UpdateBuffers();
    }
}

void Scene::process() {
    if (!ImGui::GetIO().WantCaptureMouse) {
        distance -= GetMouseWheelMove() * zoomSpeed;

        Vector3 camPos = {
            0 + distance,
            0 + distance,
            0 + distance
        };
        camera.position = camPos;
    }

    ray = GetMouseRay(GetMousePosition(), camera);
    if (!ImGui::GetIO().WantCaptureMouse && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        selectedEntity = -1;
        float closestHit = FLT_MAX;

        for (int i = 0; i < m_context->entities->size(); i++) {
            if (!m_context->entities->at(i).e_visible) continue;

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
    } else if (IsKeyPressed(KEY_F2)) {

        curr_m = "MATERIAL PREVIEW";
        currentSh = M_PREVIEW;
    } else if (IsKeyPressed(KEY_F3)) {

        curr_m = "RENDER";
        currentSh = RENDER;

        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(renderShader, renderShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
    } else if (IsKeyPressed(KEY_F4)) {

        curr_m = "WIREFRAME";
        currentSh = WIREFRAME;
    }
}

void Scene::draw() {
    if (WindowShouldClose()) {
        m_context->states->setWindowState(EXIT);
        return;
    }

    BeginDrawing();
    ClearBackground(ImVecToColor(voidCol));

    // apply shader to all entities
    for (Entity& e : *m_context->entities) {
        if (currentSh == SOLID) {
            e.e_model.materials[0].shader = solidShader;
        } else if (currentSh == M_PREVIEW || currentSh == WIREFRAME) {
            e.e_model.materials[0].shader = materialPreviewShader;
        } else {
            e.e_model.materials[0].shader = renderShader;
        }
    }

    if (colorChanged) {
        SetShaderValue(solidShader, uBaseColorLoc, &lightColor, SHADER_UNIFORM_VEC3);
    }
    if (dirChanged) {
        auto newDir = glm::vec3(lightDirection[0], lightDirection[1], lightDirection[2]);
        lightDir = glm::normalize(newDir);
        SetShaderValue(solidShader, uLightDirLoc, &lightDir[0], SHADER_UNIFORM_VEC3);
    }

    BeginMode3D(camera);

    // draw entities
    for (int i = 0; i < m_context->entities->size(); i++) {
        if (!m_context->entities->at(i).e_visible) continue;

        Vector3 epos(m_context->entities->at(i).e_position[0], m_context->entities->at(i).e_position[1], m_context->entities->at(i).e_position[2]);

        if (selectedEntity == i) {
            if ((currentSh != WIREFRAME && !toggleWireframe) || currentSh == SOLID) DrawModel(m_context->entities->at(i).e_model, epos, 1.0f, ImVecToColor(onSelectionMeshColor));
            if (currentSh != SOLID) DrawModelWires(m_context->entities->at(i).e_model, epos, 1.0f, ImVecToColor(onSelectionWiresColor));
        } else {
            if ((currentSh != WIREFRAME && !toggleWireframe) || currentSh == SOLID) DrawModel(m_context->entities->at(i).e_model, epos, 1.0f, m_context->entities->at(i).e_color);
            if (currentSh != SOLID && (showWires || toggleWireframe || currentSh == WIREFRAME)) DrawModelWires(m_context->entities->at(i).e_model, epos, 1.0f, RED);
        }
    }

    if (showGrid) DrawGrid(100, chunkSize);

    EndMode3D();

    rlImGuiBegin();

    int mw = GetScreenWidth();
    int mh = GetScreenHeight();

    ImGui::SetNextWindowPos(ImVec2(mw - 400, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, 1080), ImGuiCond_Once);

    ImGui::Begin("Scene Manager", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::PushItemWidth(200);

    // SCENE ENTITIES
    ImGui::SetWindowFontScale(2.0f);
    ImGui::Text("Scene Entities");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Dummy(ImVec2(0, 5));

    static char searchBuffer[40] = "";
    static bool inputActive = false;
    static bool shouldFocus = false;

    static bool shouldUpdateBuffers = false;

    ImGui::PushItemWidth(380);

    ImGui::SetWindowFontScale(1.2f);
    if (!inputActive) {
        if (ImGui::Selectable("Search entity...")) {
            inputActive = true;
            shouldFocus = true;
        }
    }
    static bool searchEnterPressed = false;
    static bool searchJustActivated = false;

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
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Dummy(ImVec2(0, 2.5f));

    ImGui::PopItemWidth();

    if (m_context->entities->empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No entities found!");
    }

    std::string substringToSearch = searchBuffer;

    for (int i = 0; i < m_context->entities->size(); i++) {
        const std::string& entityName = m_context->entities->at(i).e_name;

        if (!substringToSearch.empty()) {
            if (!useStrictSearch) {
                if (entityName.find(substringToSearch) == std::string::npos) continue;
            } else {
                if (entityName.compare(0, strlen(searchBuffer), searchBuffer) != 0) continue;
            }
        }

        if (ImGui::Selectable(m_context->entities->at(i).e_name.c_str(), selectedEntity == i)) {
            selectedEntity = i;
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            selectedEntity = i;
            ImGui::OpenPopup("Context");
        }
    }
    // ENTITY OPTIONS
    if (ImGui::BeginPopup("Context")) {
        if (ImGui::MenuItem("Rename")) {
            if (selectedEntity >= 0 && selectedEntity < m_context->entities->size()) {

                strncpy(renameBuffer, m_context->entities->at(selectedEntity).e_name.c_str(), sizeof(renameBuffer));
                renameBuffer[sizeof(renameBuffer) - 1] = '\0';
                openRenamePopup = true;
            }
        }
        std::string label = m_context->entities->at(selectedEntity).e_visible ? "Hide" : "Show";
        if (ImGui::MenuItem(label.c_str())) {
            m_context->entities->at(selectedEntity).e_visible = !m_context->entities->at(selectedEntity).e_visible;
        }
        if (ImGui::MenuItem("Delete")) {
            if (selectedEntity >= 0 && selectedEntity < m_context->entities->size()) {

                m_context->entities->erase(m_context->entities->begin() + selectedEntity);
                selectedEntity = -1;
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
            if (selectedEntity >= 0 && selectedEntity < m_context->entities->size()) {
                if (renameBuffer[0] == '\0') {
                    showEmptyRenameWarning = true;
                } else {
                    showEmptyRenameWarning = false;
                    m_context->entities->at(selectedEntity).e_name = std::string(renameBuffer);
                    ImGui::CloseCurrentPopup();
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
    if (selectedEntity >= 0 && selectedEntity < m_context->entities->size()) {
        ImGui::Dummy(ImVec2(0, 5));
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("Entity Settings");
        ImGui::SetWindowFontScale(1.0f);

        ImGui::Dummy(ImVec2(0, 2.5f));
        ImGui::Text("Entity Color");
        ImGui::ColorEdit3("Mesh color", reinterpret_cast<float*>(&m_context->entities->at(selectedEntity).e_colorValues));
        m_context->entities->at(selectedEntity).e_color = m_context->entities->at(selectedEntity).ImVecToColor(m_context->entities->at(selectedEntity).e_colorValues);

        ImGui::Dummy(ImVec2(0, 2.5f));
        ImGui::Text("Entity Position");
        ImGui::Text("X");
        ImGui::SameLine();
        positionChanged |= ImGui::InputFloat("##EntityPosXInput", &m_context->entities->at(selectedEntity).e_position[0], 0.5f);
        ImGui::Text("Y");
        ImGui::SameLine();
        positionChanged |= ImGui::InputFloat("##EntityPosYInput", &m_context->entities->at(selectedEntity).e_position[1], 0.5f);
        ImGui::Text("Z");
        ImGui::SameLine();
        positionChanged |= ImGui::InputFloat("##EntityPosZInput", &m_context->entities->at(selectedEntity).e_position[2], 0.5f);

        if (positionChanged) {
            positionChanged = false;
            m_context->entities->at(selectedEntity).e_boundingBox = m_context->entities->at(selectedEntity).GenBoundingBox(*m_context->entities->at(selectedEntity).e_mesh, m_context->entities->at(selectedEntity).e_position);
        }

        ImGui::Dummy(ImVec2(0, 2.5f));
        ImGui::Checkbox("Toggle Wireframe", &toggleWireframe);
        if (toggleWireframe) showWires = false;
        ImGui::BeginDisabled(toggleWireframe);
        ImGui::Checkbox("Show Wires", &showWires);
        ImGui::EndDisabled();

        if (m_context->entities->at(selectedEntity).e_type == "terrain") {
            perlin.frequency = m_context->entities->at(selectedEntity).e_terrain->frequency;
            perlin.amplitude = m_context->entities->at(selectedEntity).e_terrain->amplitude;

            int vertexCount = (int)m_context->entities->at(selectedEntity).e_vertices.size() / 3;
            for (int i = 0; i < vertexCount; i++) {
                float x = m_context->entities->at(selectedEntity).e_vertices[i * 3];
                float z = m_context->entities->at(selectedEntity).e_vertices[i * 3 + 2];
                m_context->entities->at(selectedEntity).e_vertices[i * 3 + 1] = perlin.get(x, z);
            }

            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::PushItemWidth(300);
            shouldUpdateBuffers |= ImGui::SliderFloat("Frequency", &m_context->entities->at(selectedEntity).e_terrain->frequency, 0.01f, 1.0f);
            shouldUpdateBuffers |= ImGui::SliderFloat("Amplitude", &m_context->entities->at(selectedEntity).e_terrain->amplitude, -20.0f, 20.0f);
            ImGui::PopItemWidth();
        }
    }
    // SCENE SETTINGS
    ImGui::Dummy(ImVec2(0, 5));
    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text("Scene settings");
    ImGui::SetWindowFontScale(1.0f);

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

    ImGui::Dummy(ImVec2(0, 2.5f));
    ImGui::Text("Void Color");
    ImGui::ColorEdit3("##VoidColorEdit", reinterpret_cast<float *>(&voidCol));

    // ENTITY ADD
    ImGui::Dummy(ImVec2(0, 5));
    ImGui::SetCursorPos(ImVec2(10, mh - 100));
    ImGui::SetWindowFontScale(1.1f);
    static int selectedEntityType = 0;
    if (ImGui::Button("Add Entity", ImVec2(380, 40))) {
        ImGui::OpenPopup("Add Entity");
        selectedEntityType = 0;

    }
    if (ImGui::BeginPopupModal("Add Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
        ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("Add Entity");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::SameLine();

        ImGui::Dummy(ImVec2(0, 5));
        const char* entityTypes[] = { "", "Terrain" };
        ImGui::Combo("Choose Entity Type", &selectedEntityType, entityTypes, IM_ARRAYSIZE(entityTypes));

        ImGui::Dummy(ImVec2(0, 5));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 5));

        static bool pushed = false;

        if (selectedEntityType == 0) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Select Entity Type To View Additional Settings Here!");
        } else if (selectedEntityType == 1) {
            static float width = 10;
            static float length = 10;
            static float resX = 20;
            static float resZ = 20;
            static bool shouldUpdate = false;
            if (!pushed) {
                width = length = 10;
                resX = resZ = 20;
            }
            if (!pushed || shouldUpdate) {
                if (shouldUpdate) m_context->entities->pop_back();
                shouldUpdate = false;
                m_context->entities->push_back(Entity(GenMeshPlane(width, length, resX, resZ), "terrain", "terrain"));
                pushed = true;
            }
            selectedEntity = m_context->entities->size() - 1;

            ImGui::SetWindowFontScale(1.1f);
            ImGui::Text("Terrain Settings");
            ImGui::SetWindowFontScale(1.0f);

            ImGui::Dummy(ImVec2(0, 2.5f));
            perlin.frequency = m_context->entities->at(selectedEntity).e_terrain->frequency;
            perlin.amplitude = m_context->entities->at(selectedEntity).e_terrain->amplitude;

            int vertexCount = (int)m_context->entities->at(selectedEntity).e_vertices.size() / 3;
            for (int i = 0; i < vertexCount; i++) {
                float x = m_context->entities->at(selectedEntity).e_vertices[i * 3];
                float z = m_context->entities->at(selectedEntity).e_vertices[i * 3 + 2];
                m_context->entities->at(selectedEntity).e_vertices[i * 3 + 1] = perlin.get(x, z);
            }

            m_context->entities->at(selectedEntity).UpdateBuffers();
            ImGui::PushItemWidth(300);
            shouldUpdateBuffers |= ImGui::SliderFloat("Frequency", &m_context->entities->at(selectedEntity).e_terrain->frequency, 0.01f, 1.0f);
            shouldUpdateBuffers |= ImGui::SliderFloat("Amplitude", &m_context->entities->at(selectedEntity).e_terrain->amplitude, -20.0f, 20.0f);
            ImGui::PopItemWidth();
            shouldUpdate |= ImGui::InputFloat("Width", &width);
            shouldUpdate |= ImGui::InputFloat("Length", &length);
            shouldUpdate |= ImGui::InputFloat("Resolution X", &resX);
            shouldUpdate |= ImGui::InputFloat("Resolution Z", &resZ);
        }
        ImGui::Dummy(ImVec2(0, 5));
        if (selectedEntityType != 0 && ImGui::Button("Create")) {
            pushed = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SetCursorPos(ImVec2(350, 10));
        if (ImGui::Button("X")) {
            if (selectedEntityType != 0) {
                m_context->entities->pop_back();
                selectedEntity = -1;
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // ADVANCED SETTINGS
    ImGui::SetCursorPos(ImVec2(10, mh - 50));
    ImGui::SetWindowFontScale(1.1f);
    if (ImGui::Button("Advanced Settings", ImVec2(380, 40))) {
        ImGui::OpenPopup("AdvancedSettingsPopup");
    }
    ImGui::SetWindowFontScale(1.0f);
    if (ImGui::BeginPopupModal("AdvancedSettingsPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
        ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("Advanced Settings");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(300, 10));
        if (ImGui::Button("X")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::PushItemWidth(200);
        ImGui::Dummy(ImVec2(0, 5));
        if (ImGui::CollapsingHeader("Camera")) {
            ImGui::Text("Zoom Speed");
            ImGui::SliderFloat("##ZoomSpeedSlider", &zoomSpeed, 1.0f, 50.0f);
            ImGui::SameLine();
            ImGui::PushItemWidth(80);
            ImGui::InputFloat("##ZoomSpeedInput", &zoomSpeed);
            ImGui::PopItemWidth();
            if (zoomSpeed < 1.0f) zoomSpeed = 1.0f;
            if (zoomSpeed > 50.0f) zoomSpeed = 50.0f;
        }
        if (ImGui::CollapsingHeader("Solid Shader")) {
            ImGui::Text("Light Color");
            colorChanged = ImGui::ColorEdit3("##SolidLightColor", lightColor);
            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::Text("Light Direction");
            dirChanged = ImGui::SliderFloat3("##SolidLightDirection", lightDirection, -1.0f, 1.0f);
        }
        if (ImGui::CollapsingHeader("Color Tweaking")) {
            ImGui::Text("On Selection Mesh Color");
            ImGui::ColorEdit3("##OnSelectionMeshColorEdit", reinterpret_cast<float *>(&onSelectionMeshColor));
            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::Text("On Selection Wires Color");
            ImGui::ColorEdit3("##OnSelectionWiresColorEdit", reinterpret_cast<float *>(&onSelectionWiresColor));
        }
        if (ImGui::CollapsingHeader("Advanced Scene Settings")) {
            ImGui::Checkbox("Strict Search", &useStrictSearch);
        }

        ImGui::EndPopup();
    }

    if (shouldUpdateBuffers) {
        m_context->entities->at(selectedEntity).UpdateBuffers();
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