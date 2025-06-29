#include <iostream>
#include <vector>
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "external/imgui/imgui.h"
#include "external/rlImGui/rlImGui.h"
#include "external/glm/glm.hpp"
#include "external/glm/gtc/type_ptr.hpp"

std::vector<float> customVerts;
std::vector<float> customNormals;
std::vector<unsigned short> customIndices;

enum currentViewMode {
    SOLID = 0,
    M_PREVIEW = 1,
    RENDER = 2,
    WIREFRAME = 3
};

Color ImVecToColor(ImVec4 toConvert);
void CalcNormals(float vertices[], unsigned short indices[], float normals[], int vertexCount, int triangleCount);
Mesh GenerateGridMesh(int gridSize, float tileSize);
Matrix IdentityMatrix();

int main() {
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

    Model model = LoadModelFromMesh(gridMesh);
    Mesh* mesh = &model.meshes[0];

    Model cube = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
    cube.materials[0].shader = solidShader;

    model.materials[0].shader = solidShader;
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

    Ray ray = { 0 }; // ray line
    RayCollision collision = { 0 }; // for picking the hit info

    SetTargetFPS(144);

    std::string curr_m = "SOLID";
    bool showWires = false;
    int mode = SOLID;

    bool selected = false; // for dealing with ui and 3d model selection

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

    while (!WindowShouldClose()) {
        if (IsCursorHidden()) {
            UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        }
        if (!ImGui::GetIO().WantCaptureMouse && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            ray = GetScreenToWorldRay(GetMousePosition(), camera);
            collision = GetRayCollisionBox(ray, (BoundingBox){(Vector3){ 0, 0, 0 }, (Vector3){10, 0, 10}});

            selected = collision.hit;
        }

        int x = uiGridSize / 2;
        int z = uiGridSize / 2;
        int i = (z * (uiGridSize + 1) + x) * 3;

        if (IsKeyDown(KEY_W)) {
            customVerts[i + 1] += 0.01f;
            std::cout << customVerts[i + 1] << '\n';
        } else if (IsKeyDown(KEY_S)) {
            customVerts[i + 1] -= 0.01f;
            std::cout << customVerts[i + 1] << '\n';
        }

        if (IsKeyPressed(KEY_F1)) {
            mode = SOLID;
            curr_m = "SOLID";
        } else if (IsKeyPressed(KEY_F2)) {
            mode = M_PREVIEW;
            curr_m = "MATERIAL PREVIEW";
        } else if (IsKeyPressed(KEY_F3)) {
            mode = RENDER;
            curr_m = "RENDER";
        } else if (IsKeyPressed(KEY_F4)) {
            mode = WIREFRAME;
            curr_m = "WIREFRAME";
        }
        if (IsKeyPressed(KEY_TAB)) {
            showWires = !showWires;
        }

        CalcNormals(customVerts.data(), static_cast<unsigned short*>(mesh->indices), customNormals.data(), mesh->vertexCount, mesh->triangleCount);
        UpdateMeshBuffer(*mesh, 0, customVerts.data(), customVerts.size() * sizeof(float), 0);
        UpdateMeshBuffer(*mesh, 2, customNormals.data(), customNormals.size() * sizeof(float), 0);
        //UpdateMeshBuffer(mesh, 3, customColors.data(), customColors.size() * sizeof(unsigned char), 0);

        if (mode == SOLID) {
            SetShaderValue(solidShader, uBaseColorLoc, &lightColor, SHADER_UNIFORM_VEC3);
            SetShaderValue(solidShader, uLightDirLoc, &lightDir[0], SHADER_UNIFORM_VEC3);
            model.materials[0].shader = solidShader;
            cube.materials[0].shader = solidShader;
        } else if (mode == M_PREVIEW || mode == WIREFRAME) {
            model.materials[0].shader = materialPreviewsh;
            cube.materials[0].shader = materialPreviewsh;
        } else {
            float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
            SetShaderValue(rendersh, rendersh.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
            UpdateLightValues(rendersh, lights[0]);
            model.materials[0].shader = rendersh;
            cube.materials[0].shader = rendersh;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        if (collision.hit) {
            if (mode != WIREFRAME) DrawModel(model, {0,0,0}, 1.0f, ImVecToColor(onSelectionMeshColor));
            if (mode != SOLID) DrawModelWires(model, {0.1f,0.1f,0.1f}, 1.0f, ImVecToColor(onSelectionWiresColor));
        } else {
            if (mode != WIREFRAME) DrawModel(model, {0,0,0}, 1.0f, ImVecToColor(meshColor));
            if (showWires || mode == WIREFRAME) DrawModelWires(model, {0,0,0}, 1.0f, RED);
        }
        if (mode != WIREFRAME) DrawModel(cube, {5,2.f,5}, 1.f, BLUE);
        DrawModelWires(cube, {5,2.f,5}, 1.f, RED);
        DrawText("L", 1, 2, 12, BLACK);

        EndMode3D();

        DrawFPS(10, 10);

        std::string text = "Current mode: " + curr_m;
        DrawText(text.c_str(), 100, 10, 20, BLACK);

        rlImGuiBegin();

        ImGui::SetNextWindowPos(ImVec2(1680, 0), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(300, 1080), ImGuiCond_Once);

        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::Text("Procedural Controls!");
        ImGui::Checkbox("Show Wires", &showWires);
        if (selected) {
            ImGui::Separator();
            ImGui::Text("Selected Mesh");
            ImGui::ColorEdit3("Mesh color", (float*)&meshColor);
            ImGui::SliderInt("Grid size", &uiGridSize, 1, 100);
            ImGui::SliderFloat("Tile size", &uiTileSize, 0.1f, 10.0f);

            ImGui::Separator();
            ImGui::Text("Vertex Selector");
            ImGui::SliderInt("X Index", &selectedX, 0, uiGridSize);
            ImGui::SliderInt("Y Index", &selectedZ, 0, uiGridSize);

            int index = (selectedZ * (uiGridSize + 1) + selectedX) * 3;
            float& vertexY = customVerts[index + 1];

            ImGui::SliderFloat("Vertex Y", &vertexY, -5.0f, 5.0f);
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

        ImGui::End();

        rlImGuiEnd();

        if (uiGridSize != prevGridSize || uiTileSize != prevTileSize) {
            prevGridSize = uiGridSize;
            prevTileSize = uiTileSize;

            UnloadModel(model);

            Mesh newMesh = GenerateGridMesh(uiGridSize, uiTileSize);
            model = LoadModelFromMesh(newMesh);
            model.materials[0].shader = solidShader;

            // Update customVerts and customNormals from new mesh
            auto rawV = static_cast<float*>(model.meshes[0].vertices);
            customVerts.assign(rawV, rawV + model.meshes[0].vertexCount * 3);
            auto rawN = static_cast<float*>(model.meshes[0].normals);
            customNormals.assign(rawN, rawN + model.meshes[0].vertexCount * 3);
            auto rawI = static_cast<unsigned short*>(model.meshes[0].indices);
            customIndices.assign(rawI, rawI + model.meshes[0].triangleCount * 3);

            mesh = &model.meshes[0];
        }

        EndDrawing();
    }
    UnloadShader(solidShader);
    UnloadShader(rendersh);
    UnloadModel(model);
    UnloadModel(cube);

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