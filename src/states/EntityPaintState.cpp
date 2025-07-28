#include <rlImGui.h>

#include "EntityPaintState.hpp"

#include "../core/Context.hpp"

#include "raymath.h"

void EntityPaintState::init(std::shared_ptr<Context>& p_context) {

    // brush related variables init
    brushSize = 1;
    paintColor = ImVec4(1, 0, 0, 1);
    const int textureSize = 512;
    textureHeight = textureSize;
    textureWidth = textureSize;
    validHit = false;
    hitPos = { 0 };
    hitNormal = { 0 };

    paintCanvas = LoadRenderTexture(textureSize, textureSize);

    BeginTextureMode(paintCanvas);
    ClearBackground(WHITE);
    EndTextureMode();

    p_context->entities.at(p_context->selectedEntity)->e_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = paintCanvas.texture;
}

void EntityPaintState::process(std::shared_ptr<Context>& p_context) {
    Ray ray = GetMouseRay(GetMousePosition(), *p_context->camera);

    bool didHit = false;
    Vector2 hitUV;
    validHit = false;

    for (int i = 0; i < p_context->entities.at(p_context->selectedEntity)->e_mesh->triangleCount; i++) {
        int i0 = p_context->entities.at(p_context->selectedEntity)->e_mesh->indices[i * 3];
        int i1 = p_context->entities.at(p_context->selectedEntity)->e_mesh->indices[i * 3 + 1];
        int i2 = p_context->entities.at(p_context->selectedEntity)->e_mesh->indices[i * 3 + 2];

        Vector3 epos = { p_context->entities.at(p_context->selectedEntity)->e_position[0], p_context->entities.at(p_context->selectedEntity)->e_position[1], p_context->entities.at(p_context->selectedEntity)->e_position[2] };

        Vector3 v0 = { p_context->entities.at(p_context->selectedEntity)->e_mesh->vertices[i0 * 3], p_context->entities.at(p_context->selectedEntity)->e_mesh->vertices[i0 * 3 + 1], p_context->entities.at(p_context->selectedEntity)->e_mesh->vertices[i0 * 3 + 2] };
        Vector3 v1 = { p_context->entities.at(p_context->selectedEntity)->e_mesh->vertices[i1 * 3], p_context->entities.at(p_context->selectedEntity)->e_mesh->vertices[i1 * 3 + 1], p_context->entities.at(p_context->selectedEntity)->e_mesh->vertices[i1 * 3 + 2] };
        Vector3 v2 = { p_context->entities.at(p_context->selectedEntity)->e_mesh->vertices[i2 * 3], p_context->entities.at(p_context->selectedEntity)->e_mesh->vertices[i2 * 3 + 1], p_context->entities.at(p_context->selectedEntity)->e_mesh->vertices[i2 * 3 + 2] };

        v0 = Vector3Add(v0, epos);
        v1 = Vector3Add(v1, epos);
        v2 = Vector3Add(v2, epos);

        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = v2 - v0;

        if (CheckCollisionRayTriangle(ray, v0, v1, v2, &hitPos)) {
            Vector2 uv0 = { p_context->entities.at(p_context->selectedEntity)->e_mesh->texcoords[i0 * 2], p_context->entities.at(p_context->selectedEntity)->e_mesh->texcoords[i0 * 2 + 1] };
            Vector2 uv1 = { p_context->entities.at(p_context->selectedEntity)->e_mesh->texcoords[i1 * 2], p_context->entities.at(p_context->selectedEntity)->e_mesh->texcoords[i1 * 2 + 1] };
            Vector2 uv2 = { p_context->entities.at(p_context->selectedEntity)->e_mesh->texcoords[i2 * 2], p_context->entities.at(p_context->selectedEntity)->e_mesh->texcoords[i2 * 2 + 1] };

            Vector3 bary = Vector3Barycenter(hitPos, v0, v1, v2);
            hitUV = Vector2 { bary.x * uv0.x + bary.y * uv1.x + bary.z * uv2.x, bary.x * uv0.y + bary.y * uv1.y + bary.z * uv2.y};
            hitNormal = Vector3Normalize(Vector3CrossProduct(edge1, edge2));

            didHit = true;
            validHit = true;
            break;
        }
    }

    if (validHit) HideCursor();
    else ShowCursor();

    if (didHit && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        int px = static_cast<int>(hitUV.x * textureWidth);
        int py = static_cast<int>((1.0f - hitUV.y) * textureHeight);

        BeginTextureMode(paintCanvas);
        DrawCircle(px, py, brushSize, ImVec4ToColor(paintColor));
        EndTextureMode();
    }
}

void EntityPaintState::draw(std::shared_ptr<Context>& p_context) {
    BeginDrawing();
    p_context->shaders->handleBackgroundClearing(p_context->currentSh);

    BeginMode3D(*p_context->camera);

    Vector3 epos = { p_context->entities.at(p_context->selectedEntity)->e_position[0], p_context->entities.at(p_context->selectedEntity)->e_position[1], p_context->entities.at(p_context->selectedEntity)->e_position[2] };
    DrawModel(p_context->entities.at(p_context->selectedEntity)->e_model, epos, 1.0f, p_context->entities.at(p_context->selectedEntity)->e_color);

    EndMode3D();

    if (validHit) {
        Vector2 screenPos = GetWorldToScreen(hitPos, *p_context->camera);
        DrawCircleV(screenPos, brushSize * 3, Color{0, 255, 0, 100});
    }

    rlImGuiBegin();

    if (ImGui::GetIO().WantCaptureMouse) {
        ShowCursor();
    } else {
        HideCursor();
    }

    int code = p_context->ui->DrawMainMenuBar(p_context, p_context->currentSh);
    if (p_context->currentSh != 2) p_context->currentSh = 2;
    if (code != -1) ImGui::OpenPopup("Warning");

    if (ImGui::BeginPopupModal("Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
        p_context->fontMgr.setXL();
        ImGui::Text("Warning");
        ImGui::PopFont();
        ImGui::SameLine();

        ImGui::Dummy(ImVec2(0, 5));
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please switch back to scene in order to do that!");
        ImGui::Dummy(ImVec2(0, 5));

        ImGui::Dummy(ImVec2(280, 0));
        ImGui::SameLine();
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    p_context->ui->DrawStateBar(p_context, p_context->currentSh, ENTITYPAINT);

    int mw = GetScreenWidth();
    float menuHeight = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(ImVec2(mw - 400, menuHeight), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, 1080 - menuHeight), ImGuiCond_Once);

    ImGui::Begin("EntityPaintManager", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);

    p_context->uiManager->Section("Entity Paint Settings", p_context->fontMgr.getXXL(), 0);
    ImGui::Dummy(ImVec2(0, 5));

    p_context->uiManager->FloatInput("Brush Size", brushSize, true, 0.1f, 500.0f);

    ImGui::Dummy(ImVec2(0, 2.5f));
    ImGui::Text("Brush Color");
    ImGui::ColorEdit3("##BrushColor", reinterpret_cast<float*>(&paintColor));

    ImGui::End();

    rlImGuiEnd();

    EndDrawing();
}

void EntityPaintState::clean(std::shared_ptr<Context>& p_context) {
    ShowCursor();
}

Color EntityPaintState::ImVec4ToColor(ImVec4 p_color) {
    return (Color){
        static_cast<unsigned char>(p_color.x * 255.0f),
        static_cast<unsigned char>(p_color.y * 255.0f),
        static_cast<unsigned char>(p_color.z * 255.0f),
        255
    };
}

bool EntityPaintState::CheckCollisionRayTriangle(const Ray& ray, Vector3 v0, Vector3 v1, Vector3 v2, Vector3* p_hitPos) {
    constexpr float EPS = 0.000001f;

    Vector3 edge1 = Vector3Subtract(v1, v0);
    Vector3 edge2 = Vector3Subtract(v2, v0);

    Vector3 h = Vector3CrossProduct(ray.direction, edge2);
    float a = Vector3DotProduct(edge1, h);

    // parallel
    if (a > -EPS && a < EPS) return false;

    float f = 1.0f / a;

    // first barycentric coordinate
    Vector3 s = Vector3Subtract(ray.position, v0);
    float u = f * Vector3DotProduct(s, h);

    if (u < 0.0f || u > 1.0f) return false;

    // second barycentric coordinate
    Vector3 q = Vector3CrossProduct(s, edge1);
    float v = f * Vector3DotProduct(ray.direction, q);

    if (v < 0.0f || u + v > 1.0f) return false;

    // intersection point
    float t = f * Vector3DotProduct(edge2, q);

    if (t > EPS) {
        if (p_hitPos) *p_hitPos = Vector3Add(ray.position, Vector3Scale(ray.direction, t));
        return true;
    }

    return false;
}
