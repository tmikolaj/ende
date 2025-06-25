#include <iostream>
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "external/glm/glm.hpp"
#include "external/glm/gtc/type_ptr.hpp"

enum currentViewMode {
    SOLID = 0,
    DEFAULT = 1,
    M_PREVIEW = 2
};

void CalcNormals(float vertices[], unsigned short indices[], float normals[], int vertexCount, int triangleCount);
Mesh GenerateGridMesh(int gridSize, float tileSize);
Matrix IdentityMatrix();

int main() {
    InitWindow(800, 600, "Grid mesh");

    Shader solidShader = LoadShader("../shaders/solid.vs", "../shaders/solid.fs");

    int uLightDirLoc = GetShaderLocation(solidShader, "lightDir");
    int uBaseColorLoc = GetShaderLocation(solidShader, "baseColor");

    Shader defaultsh = LoadShader(0, 0);

    Shader materialPreview = LoadShader(TextFormat("../shaders/raylibshaders/lighting.vs"), TextFormat("../shaders/raylibshaders/lighting.fs"));
    // TODO: add validation check for shaders loading in Engine::ShaderManager

    materialPreview.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(materialPreview, "viewPos");

    Light lights[1] = { 0 };
    lights[0] = CreateLight(LIGHT_DIRECTIONAL, (Vector3){ -2, 1, -2}, Vector3Zero(), WHITE, materialPreview);
    lights[0].enabled = true;

    Mesh gridMesh = GenerateGridMesh(10,1.0f);
    Model model = LoadModelFromMesh(gridMesh);
    Mesh& mesh = model.meshes[0];

    Model cube = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
    cube.materials[0].shader = solidShader;

    model.materials[0].shader = solidShader;
    Vector3 color = (Vector3){ 1.0f, 1.0f, 1.0f };
    glm::vec3 lightDir = glm::normalize(glm::vec3{-1.0f, 1.0f, -1.0f});

    SetShaderValue(solidShader, uBaseColorLoc, &color, SHADER_UNIFORM_VEC3);

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

    while (!WindowShouldClose()) {
        if (IsCursorHidden()) {
            UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (!collision.hit) {
                ray = GetScreenToWorldRay(GetMousePosition(), camera);

                collision = GetRayCollisionBox(ray, (BoundingBox){(Vector3){ 0, 0, 0 }, (Vector3){10, 0, 10}});
            } else {
                collision.hit = false;
            }
        }

        float t = GetTime();
        auto verts = static_cast<float*>(mesh.vertices);
        int gridSize = 10;
        int x = 5.0f, z = 5.0f;
        int i = (z * (gridSize + 1) + x) * 3;

        if (IsKeyDown(KEY_W)) {
            verts[i + 1] += 0.01f;
            std::cout << verts[i + 1] << '\n';
        } else if (IsKeyDown(KEY_S)) {
            verts[i + 1] -= 0.01f;
            std::cout << verts[i + 1] << '\n';
        }

        if (IsKeyPressed(KEY_F1)) {
            mode = SOLID;
            curr_m = "SOLID";
        } else if (IsKeyPressed(KEY_F2)) {
            mode = DEFAULT;
            curr_m = "DEFAULT";
        } else if (IsKeyPressed(KEY_F3)) {
            mode = M_PREVIEW;
            curr_m = "MATERIAL PREVIEW";
        }
        if (IsKeyPressed(KEY_TAB)) {
            showWires = !showWires;
        }

        CalcNormals(static_cast<float*>(mesh.vertices), static_cast<unsigned short*>(mesh.indices), static_cast<float*>(mesh.normals), mesh.vertexCount, mesh.triangleCount);
        UpdateMeshBuffer(mesh, 0, verts,mesh.vertexCount*3*sizeof(float), 0);
        UpdateMeshBuffer(mesh, 2, mesh.normals,mesh.vertexCount*3*sizeof(float), 0);
        UpdateMeshBuffer(mesh, 3, mesh.colors, mesh.vertexCount*sizeof(Color), 0);


        if (mode == SOLID) {
            SetShaderValue(solidShader, uLightDirLoc, &lightDir[0], SHADER_UNIFORM_VEC3);
            model.materials[0].shader = solidShader;
            cube.materials[0].shader = solidShader;
        } else if (mode == DEFAULT) {
            model.materials[0].shader = defaultsh;
            cube.materials[0].shader = defaultsh;
        } else {
            float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
            SetShaderValue(materialPreview, materialPreview.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
            UpdateLightValues(materialPreview, lights[0]);
            model.materials[0].shader = materialPreview;
            cube.materials[0].shader = materialPreview;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        if (collision.hit) {
            DrawModel(model, {0,0,0}, 1.0f, RED);
            if (mode != SOLID) DrawModelWires(model, {0.1f,0.1f,0.1f}, 1.0f, GREEN);
        } else {
            DrawModel(model, {0,0,0}, 1.0f, BLUE);
            if (showWires) DrawModelWires(model, {0,0,0}, 1.0f, RED);
        }
        DrawModel(cube, {5,2.f,5}, 1.f, BLUE);
        DrawModelWires(cube, {5,2.f,5}, 1.f, RED);
        DrawText("L", 1, 2, 12, BLACK);

        EndMode3D();

        DrawFPS(10, 10);

        std::string text = "Current mode: " + curr_m;
        DrawText(text.c_str(), 100, 10, 20, BLACK);
        EndDrawing();
    }
    UnloadShader(solidShader);
    UnloadShader(materialPreview);
    UnloadShader(defaultsh);
    UnloadModel(model);
    UnloadModel(cube);

    CloseWindow();
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