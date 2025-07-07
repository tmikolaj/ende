#include <iostream>
#include <vector>
#include "raylib.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "external/imgui/imgui.h"
#include "external/rlImGui/rlImGui.h"
#include "external/glm/glm.hpp"
#include "external/glm/gtc/type_ptr.hpp"
#include "src/Entity.hpp"
#include "src/Program.hpp"

std::vector<float> customVerts;
std::vector<float> customNormals;
std::vector<unsigned short> customIndices;

enum currentViewMode {
    mSOLID = 0,
    mM_PREVIEW = 1,
    mRENDER = 2,
    mWIREFRAME = 3
};

Color ImVecToColor(ImVec4 toConvert);
void CalcNormals(float vertices[], unsigned short indices[], float normals[], int vertexCount, int triangleCount);
Mesh GenerateGridMesh(int gridSize, float tileSize);
Matrix IdentityMatrix();

int main() {
    Program program;
    program.init();
    program.run();
    return 0;

    InitWindow(1920, 1080, "Grid mesh");
    rlImGuiSetup(true);

    Shader solidShader = LoadShader("../shaders/solid.vs", "../shaders/solid.fs");

    int uLightDirLoc = GetShaderLocation(solidShader, "lightDir");
    int uBaseColorLoc = GetShaderLocation(solidShader, "baseColor");

    Shader materialPreviewsh = LoadShader(nullptr, nullptr);

    Shader rendersh = LoadShader(TextFormat("../shaders/raylibshaders/lighting.vs"), TextFormat("../shaders/raylibshaders/lighting.fs"));

    rendersh.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(rendersh, "viewPos");

    Light lights[1] = { 0 };
    lights[0] = CreateLight(LIGHT_DIRECTIONAL, (Vector3){ -2, 1, -2}, Vector3Zero(), WHITE, rendersh);
    lights[0].enabled = true;

    Mesh gridMesh = GenerateGridMesh(10,1.0f);
    auto rawVerts = static_cast<float*>(gridMesh.vertices);
    customVerts.assign(rawVerts, rawVerts + gridMesh.vertexCount * 3);
    auto rawNormals = static_cast<float*>(gridMesh.normals);
    customNormals.assign(rawNormals, rawNormals + gridMesh.vertexCount * 3);
    auto rawIndices = static_cast<unsigned short*>(gridMesh.indices);
    customIndices.assign(rawIndices, rawIndices + gridMesh.triangleCount * 3);

    Entity entity(GenerateGridMesh(10, 1.0f), "plane", "terrain");
    Entity ecube(GenMeshCube(2.0f, 2.0f, 2.0f), "cube", "none");

    std::vector<Entity> entities;
    entities.emplace_back(std::move(ecube));
    entities.emplace_back(std::move(entity));
    int selectedEntity = -1;

    float lightColor[3] = { 1.0f, 1.0f, 1.0f };
    glm::vec3 lightDir = glm::normalize(glm::vec3{-1.0f, 1.0f, -1.0f});
    float lightDirection[3] = { lightDir.x, lightDir.y, lightDir.z };

    SetShaderValue(solidShader, uBaseColorLoc, &lightColor, SHADER_UNIFORM_VEC3);

    Camera3D camera = { 0 };
    camera.position = {8, 12, 8};
    camera.target = {5, 0, 5};
    camera.up = {0, 10, 0};
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 target = { 0, 0, 0 };
    float distance = 10.0f;
    float zoomSpeed = 1.0f;

    Ray ray = { 0 }; // ray line
    RayCollision collision = { 0 }; // for picking the hit info

    SetTargetFPS(144);

    std::string curr_m = "SOLID";
    bool showWires = false;
    int mode = mSOLID;

    // for imgui (to change the values of these variables)
    int uiGridSize = 10;
    float uiTileSize = 1.0f;
    int prevGridSize = uiGridSize;
    float prevTileSize = uiTileSize;
    int selectedX = uiGridSize / 2;
    int selectedZ = uiGridSize / 2;

    ImVec4 meshColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 onSelectionMeshColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 onSelectionWiresColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

    float position[3] = { 0, 0, 0 };

    float chunkSize = 2.5f;

    bool isCursorHidden = false;

    while (!WindowShouldClose()) {
        distance -= GetMouseWheelMove() * zoomSpeed;
        distance = Clamp(distance, 2.0f, 50.0f);

        Vector3 cameraPos = {
            target.x + distance,
            target.y + distance,
            target.z + distance
        };
        camera.position = cameraPos;
        camera.target = target;
        camera.up = { 0.0f, 1.0f, 0.0f };

        ray = GetMouseRay(GetMousePosition(), camera);
        if (!ImGui::GetIO().WantCaptureMouse && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            selectedEntity = -1;
            float closestHit = FLT_MAX;

            for (int i = 0; i < entities.size(); i++) {
                RayCollision hit = GetRayCollisionBox(ray, entities[i].e_boundingBox);
                if (hit.hit && hit.distance < closestHit) {
                    closestHit = hit.distance;
                    selectedEntity = i;
                }
            }
        }

        if (IsKeyPressed(KEY_F1)) {
            mode = mSOLID;
            curr_m = "SOLID";
        } else if (IsKeyPressed(KEY_F2)) {
            mode = mM_PREVIEW;
            curr_m = "MATERIAL PREVIEW";
        } else if (IsKeyPressed(KEY_F3)) {
            mode = mRENDER;
            curr_m = "RENDER";
        } else if (IsKeyPressed(KEY_F4)) {
            mode = mWIREFRAME;
            curr_m = "WIREFRAME";
        }

        CalcNormals(customVerts.data(), static_cast<unsigned short*>(entity.e_mesh->indices), customNormals.data(), entity.e_mesh->vertexCount, entity.e_mesh->triangleCount);
        UpdateMeshBuffer(*entity.e_mesh, 0, customVerts.data(), customVerts.size() * sizeof(float), 0);
        UpdateMeshBuffer(*entity.e_mesh, 2, customNormals.data(), customNormals.size() * sizeof(float), 0);
        //UpdateMeshBuffer(mesh, 3, customColors.data(), customColors.size() * sizeof(unsigned char), 0);

        if (mode == mSOLID) {
            SetShaderValue(solidShader, uBaseColorLoc, &lightColor, SHADER_UNIFORM_VEC3);
            SetShaderValue(solidShader, uLightDirLoc, &lightDir[0], SHADER_UNIFORM_VEC3);
            entity.e_model.materials[0].shader = solidShader;
            ecube.e_model.materials[0].shader = solidShader;
        } else if (mode == mM_PREVIEW || mode == mWIREFRAME) {
            entity.e_model.materials[0].shader = materialPreviewsh;
            ecube.e_model.materials[0].shader = materialPreviewsh;
        } else {
            float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
            SetShaderValue(rendersh, rendersh.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
            UpdateLightValues(rendersh, lights[0]);
            entity.e_model.materials[0].shader = rendersh;
            ecube.e_model.materials[0].shader = rendersh;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        for (int i = 0; i < static_cast<int>(entities.size()); i++) {
            Vector3 epos(entities[i].e_position[0], entities[i].e_position[1], entities[i].e_position[2]);
            if (selectedEntity == i) {
                if (mode != mWIREFRAME) DrawModel(entities[i].e_model, epos, 1.0f, entities[i].ImVecToColor(onSelectionMeshColor));
                if (mode != mSOLID) DrawModelWires(entities[i].e_model, epos, 1.0f, entities[i].ImVecToColor(onSelectionWiresColor));
            } else {
                if (mode != mWIREFRAME) DrawModel(entities[i].e_model, epos, 1.0f, entities[i].e_color);
                if (showWires || mode == mWIREFRAME) DrawModelWires(entities[i].e_model, epos, 1.0f, RED);
            }
        }

        DrawGrid(100, chunkSize);
        EndMode3D();

        DrawFPS(10, 10);

        std::string text = "Current mode: " + curr_m;
        DrawText(text.c_str(), 100, 10, 20, BLACK);

        rlImGuiBegin();

        ImGui::SetNextWindowPos(ImVec2(1580, 0), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(400, 1080), ImGuiCond_Once);

        ImGui::Begin("Scene Settings", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::PushItemWidth(200);

        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("Scene Entities");
        ImGui::Dummy(ImVec2(0, 5));
        ImGui::SetWindowFontScale(1.0f);

        if (entities.empty()) {
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No entities found!");
        }

        char renameBuffer[40];
        bool openRenamePopup = false;

        for (int i = 0; i < entities.size(); i++) {
            std::string label = entities[i].e_name;

            if (ImGui::Selectable(label.c_str(), selectedEntity == i)) {
                selectedEntity = i;
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                selectedEntity = i;
                ImGui::OpenPopup("context");
            }
        }

        if (ImGui::BeginPopup("context")) {
            if (ImGui::MenuItem("Delete")) {
                if (selectedEntity >= 0 && selectedEntity < entities.size()) {

                    entities.erase(entities.begin() + selectedEntity);
                    selectedEntity = -1;
                }
            }
            if (ImGui::MenuItem("Rename")) {
                if (selectedEntity >= 0 && selectedEntity < entities.size()) {

                    strncpy(renameBuffer, entities[selectedEntity].e_name.c_str(), sizeof(renameBuffer));
                    renameBuffer[sizeof(renameBuffer) - 1] = '\0';
                    openRenamePopup = true;
                }
            }
            ImGui::EndPopup();
        }

        if (openRenamePopup) {
            ImGui::OpenPopup("Rename Entity");
            openRenamePopup = false;
        }

        if (ImGui::BeginPopupModal("Rename Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("New Name");
            ImGui::SameLine();
            ImGui::InputText("##", renameBuffer, IM_ARRAYSIZE(renameBuffer));

            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("OK")) {
                if (selectedEntity >= 0 && selectedEntity < entities.size()) {
                    entities[selectedEntity].e_name = std::string(renameBuffer);
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }

        ImGui::Dummy(ImVec2(0, 5));
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("Procedural Controls!");
        ImGui::Dummy(ImVec2(0, 5));
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Chunk Size");
        ImGui::SameLine();

        ImGui::PushID("ChunkSizeControl");
        ImGui::SliderFloat("##Slider", &chunkSize, 0.5f, 100.0f);
        ImGui::SameLine();
        ImGui::PushItemWidth(50);
        ImGui::InputFloat("##Input", &chunkSize, 0.0f, 0.0f);
        ImGui::PopItemWidth();
        ImGui::PushItemWidth(200);
        ImGui::PopID();

        if (chunkSize > 100.0f) {
            chunkSize = 100.0f;
        } else if (chunkSize < 0.5f) {
            chunkSize = 0.5f;
        }

        ImGui::Checkbox("Show Wires", &showWires);

        if (selectedEntity >= 0 && selectedEntity < entities.size()) {
            ImGui::Separator();
            ImGui::Text("Selected Mesh");
            ImGui::ColorEdit3("Mesh color", (float*)&entities[selectedEntity].e_colorValues);
            entities[selectedEntity].e_color = entities[selectedEntity].ImVecToColor(entities[selectedEntity].e_colorValues);
            ImGui::SliderInt("Grid size", &uiGridSize, 1, 100);
            ImGui::SliderFloat("Tile size", &uiTileSize, 0.1f, 10.0f);
            ImGui::SliderFloat3("Position", (float*)&entities[selectedEntity].e_position, -100, 100);

            ImGui::Separator();
            ImGui::Text("Mesh Vertex Selector");
            ImGui::SliderInt("X Index", &selectedX, 0, uiGridSize);
            ImGui::SliderInt("Y Index", &selectedZ, 0, uiGridSize);

            int index = (selectedZ * (uiGridSize + 1) + selectedX) * 3;
            float& vertexY = customVerts[index + 1];

            ImGui::SliderFloat("Vertex Y", &vertexY, -5.0f, 5.0f);

            ImGui::Separator();
            if (ImGui::CollapsingHeader("Modifiers")) {
                // TODO: place it in a separate file (inside a struct) to have better control over it
                ImGui::Text("Not yet working (was just testing UI");
                static int selectedModifier = 0;
                const char* modifiers[] = { "Subdivision" };
                ImGui::Combo("Add Modifier", &selectedModifier, modifiers, IM_ARRAYSIZE(modifiers));

                ImGui::Separator();

                if (selectedModifier == 0) {
                    static int levels = 1;
                    ImGui::SliderInt("Levels", &levels, 1, 6);
                }
            }
        }
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Solid Shader Advanced Settings", ImGuiTreeNodeFlags_None)) {
            ImGui::SliderFloat3("Light direction", lightDirection, -1.0f, 1.0f);
            glm::vec3 newDir = glm::vec3(lightDirection[0], lightDirection[1], lightDirection[2]);
            lightDir = glm::normalize(newDir);
            ImGui::ColorEdit3("Light color", lightColor);
        }
        if (ImGui::CollapsingHeader("Selection Color Settings", ImGuiTreeNodeFlags_None)) {
            ImGui::ColorEdit3("On Selected Mesh", (float*)&onSelectionMeshColor);
            ImGui::ColorEdit3("On Selected Wires", (float*)&onSelectionWiresColor);
        }
        if (ImGui::CollapsingHeader("General settings")) {
            ImGui::Indent();
            if (ImGui::CollapsingHeader("Camera")) {
                ImGui::InputFloat("Scroll speed", &zoomSpeed);
                ImGui::InputFloat("FOV", &camera.fovy, 0.0f, 90.0f);
            }
            ImGui::Unindent();
        }
        ImGui::PopItemWidth();
        ImGui::End();

        rlImGuiEnd();

        if (uiGridSize != prevGridSize || uiTileSize != prevTileSize) {
            prevGridSize = uiGridSize;
            prevTileSize = uiTileSize;

            UnloadModel(entity.e_model);

            Mesh newMesh = GenerateGridMesh(uiGridSize, uiTileSize);
            entity.e_model = LoadModelFromMesh(newMesh);
            entity.e_model.materials[0].shader = solidShader;

            // Update customVerts and customNormals from new mesh
            auto rawV = static_cast<float*>(entity.e_model.meshes[0].vertices);
            customVerts.assign(rawV, rawV + entity.e_model.meshes[0].vertexCount * 3);
            auto rawN = static_cast<float*>(entity.e_model.meshes[0].normals);
            customNormals.assign(rawN, rawN + entity.e_model.meshes[0].vertexCount * 3);
            auto rawI = static_cast<unsigned short*>(entity.e_model.meshes[0].indices);
            customIndices.assign(rawI, rawI + entity.e_model.meshes[0].triangleCount * 3);

            entity.e_mesh = &entity.e_model.meshes[0];
        }

        EndDrawing();
    }
    UnloadShader(solidShader);
    UnloadShader(rendersh);
    UnloadModel(entity.e_model);
    UnloadModel(ecube.e_model);

    rlImGuiShutdown();
    CloseWindow();
}

Color ImVecToColor(ImVec4 toConvert) {
    return (Color){
        static_cast<unsigned char>(toConvert.x * 255.0f),
        static_cast<unsigned char>(toConvert.y * 255.0f),
        static_cast<unsigned char>(toConvert.z * 255.0f),
        255
    };
}

Mesh GenerateGridMesh(int gridSize, float tileSize) {
    int verticesPerRow = gridSize + 1;
    int vertexCount = verticesPerRow * verticesPerRow;
    int triangleCount = gridSize * gridSize * 2;

    auto vertices = static_cast<float*>(malloc(sizeof(float) * vertexCount * 3));
    auto indices = static_cast<unsigned short*>(malloc(sizeof(unsigned short) * triangleCount * 3));
    auto normals = static_cast<float*>(calloc(vertexCount * 3, sizeof(float)));

    int v = 0;
    for (int z = 0; z <= gridSize; z++) {
        for (int x = 0; x <= gridSize; x++) {
            vertices[v++] = x * tileSize;
            vertices[v++] = 0.0f;
            vertices[v++] = z * tileSize;
        }
    }

    int t = 0;
    for (int z = 0; z < gridSize; z++) {
        for (int x = 0; x < gridSize; x++) {
            int i0 = z * (gridSize + 1) + x;
            int i1 = i0 + 1;
            int i2 = i0 + (gridSize + 1);
            int i3 = i2 + 1;

            indices[t++] = i0;
            indices[t++] = i2;
            indices[t++] = i1;

            indices[t++] = i1;
            indices[t++] = i2;
            indices[t++] = i3;
        }
    }

    for (int i = 0; i < triangleCount * 3; i += 3) {
        unsigned short i0 = indices[i];
        unsigned short i1 = indices[i + 1];
        unsigned short i2 = indices[i + 2];

        glm::vec3 v0(vertices[i0 * 3], vertices[i0 * 3 + 1], vertices[i0 * 3 + 2]);
        glm::vec3 v1(vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
        glm::vec3 v2(vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        for (int j : {i0, i1, i2}) {
            normals[j * 3 + 0] += faceNormal.x;
            normals[j * 3 + 1] += faceNormal.y;
            normals[j * 3 + 2] += faceNormal.z;
        }
    }

    for (int i = 0; i < vertexCount; i++) {
        glm::vec3 n(normals[i*3], normals[i*3+1], normals[i*3+2]);
        n = glm::normalize(n);
        normals[i * 3 + 0] = n.x;
        normals[i * 3 + 1] = n.y;
        normals[i * 3 + 2] = n.z;
    }

    Mesh mesh = { 0 };
    mesh.triangleCount = triangleCount;
    mesh.vertexCount = vertexCount;
    mesh.vertices = vertices;
    mesh.indices = indices;
    mesh.normals = normals;

    UploadMesh(&mesh, true);

    return mesh;
}

void CalcNormals(float vertices[], unsigned short indices[], float normals[], int vertexCount, int triangleCount) {
    // Reset normals
    for (int i = 0; i < vertexCount * 3; i++) {
        normals[i] = 0.0f;
    }

    for (int i = 0; i < triangleCount * 3; i += 3) {
        unsigned short i0 = indices[i];
        unsigned short i1 = indices[i + 1];
        unsigned short i2 = indices[i + 2];

        glm::vec3 v0(vertices[i0 * 3], vertices[i0 * 3 + 1], vertices[i0 * 3 + 2]);
        glm::vec3 v1(vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
        glm::vec3 v2(vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        for (int j : {i0, i1, i2}) {
            normals[j * 3 + 0] += faceNormal.x;
            normals[j * 3 + 1] += faceNormal.y;
            normals[j * 3 + 2] += faceNormal.z;
        }
    }

    for (int i = 0; i < vertexCount; i++) {
        glm::vec3 n(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
        n = glm::normalize(n);

        normals[i * 3 + 0] = n.x;
        normals[i * 3 + 1] = n.y;
        normals[i * 3 + 2] = n.z;
    }
}

Matrix IdentityMatrix() {
    return Matrix{1,0,0,0,
                  0,1,0,0,
                  0,0,1,0,
                  0,0,0,1};
}