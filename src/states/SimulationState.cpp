#include "SimulationState.hpp"

#include "../core/Context.hpp"

#include "rlImGui.h"

#include "raylib.h"
#include "src/entities/TerrainEntity.hpp"

void SimulationState::init(std::shared_ptr<Context>& p_context) {
    dontShowWarning = false;
}

void SimulationState::process(std::shared_ptr<Context>& p_context) {
    p_context->shaders->handleShaderSelection(p_context->currentSh);

    if (p_context->currentSh == RENDER) p_context->shaders->updateCamPos(p_context);
}

void SimulationState::draw(std::shared_ptr<Context>& p_context) {
    BeginDrawing();

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

    BeginMode3D(*p_context->camera);

    Color wirCol(static_cast<unsigned char>(wireframeColor.x), static_cast<unsigned char>(wireframeColor.y), static_cast<unsigned char>(wireframeColor.z), 255);

    Vector3 epos(p_context->entities.at(p_context->selectedEntity)->e_position[0], p_context->entities.at(p_context->selectedEntity)->e_position[1], p_context->entities.at(p_context->selectedEntity)->e_position[2]);

    if (p_context->currentSh != WIREFRAME) DrawModel(p_context->entities.at(p_context->selectedEntity)->e_model, epos, 1.0f, p_context->entities.at(p_context->selectedEntity)->e_color);
    else DrawModelWires(p_context->entities.at(p_context->selectedEntity)->e_model, epos, 1.0f, wirCol);

    EndMode3D();

    rlImGuiBegin();

    int code = p_context->ui->DrawMainMenuBar(p_context, p_context->currentSh);
    if (code != -1) ImGui::OpenPopup("Warning");

    if (ImGui::BeginPopupModal("Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
        p_context->fontMgr.setXL();
        ImGui::Text("Warning");
        ImGui::PopFont();
        ImGui::SameLine();

        ImGui::Dummy(ImVec2(200, 0));
        ImGui::SameLine();
        if (ImGui::Button("X")) {
            ImGui::CloseCurrentPopup();
        }

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

    p_context->ui->DrawStateBar(p_context, p_context->currentSh, SIMULATION);

    int mw = GetScreenWidth();
    float menuHeight = ImGui::GetFrameHeight();

    static int selectedSimulation = 0;
    const char* simulations[] = { "", "Thermal Erosion", "Hydraulic Erosion" };

    ImGui::SetNextWindowPos(ImVec2(mw - 400, menuHeight), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, 1080 - menuHeight), ImGuiCond_Once);

    ImGui::Begin("SimulationManager", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);

    p_context->uiManager->Section("Simulation Settings", p_context->fontMgr.getXXL(), 0);
    ImGui::Dummy(ImVec2(0, 5));

    ImGui::Combo("##ChooseSimulation", &selectedSimulation, simulations, IM_ARRAYSIZE(simulations));

    static bool showError = false;
    if (selectedSimulation == 0) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please select simulation to view options here!");
        ImGui::Dummy(ImVec2(0, 5));

    } else if (selectedSimulation == 1) {
        if (p_context->entities.at(p_context->selectedEntity)->e_type == "terrain") {
            static float startHoverTal = 0.0f;
            static float startHoverStr = 0.0f;
            static float startHoverIter = 0.0f;
            static float startHoverNei = 0.0f;

            p_context->uiManager->Section("Thermal Erosion Settings");

            float highestY = findHighestYValue(p_context->entities.at(p_context->selectedEntity)->e_vertices);

            if (highestY == 0) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please change amplitude in order to do that");
                showError = true;
            }

            p_context->uiManager->FloatSlider("Talus", thermalErosion.talus, 0.5f, highestY);
            p_context->uiManager->SetItemTooltip("Erosion occurs when the height difference between a point and its neighbor exceeds this threshold (talus angle)", startHoverTal);

            p_context->uiManager->FloatSlider("Strength", thermalErosion.strength, 0.1f, 10.0f);
            p_context->uiManager->SetItemTooltip("Controls how much material is transferred during each erosion step (higher = stronger effect)", startHoverStr);

            p_context->uiManager->IntInput("Iterations", thermalErosion.iterations, false);
            p_context->uiManager->SetItemTooltip("Number of times the erosion simulation is run (recommended value 5)", startHoverIter);

            ImGui::Dummy(ImVec2(0, 2.5f));
            ImGui::Checkbox("Apply on 4 neighbors (false = 8 neighbors)", &thermalErosion.use4);
            p_context->uiManager->SetItemTooltip("If enabled erosion compares each point to 4 direct neighbors (N, S, E, W) and if disabled it will use 8 neighbors", startHoverNei);

            if (ImGui::Button("Apply Erosion")) {

                thermalErosion.Apply(p_context->entities.at(p_context->selectedEntity)->e_vertices);
                p_context->entities.at(p_context->selectedEntity)->UpdateBuffers();
            }
        } else if (p_context->entities.at(p_context->selectedEntity)->e_type != "terrain" || showError) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Available only for terrain entities");
        }

    } else if (selectedSimulation == 2) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not yet implemented!");
        ImGui::Dummy(ImVec2(0, 5));
    }

    ImGui::Text("Wireframe Color");
    ImGui::ColorEdit3("##WireframeColor", reinterpret_cast<float*>(&wireframeColor));

    ImGui::End();

    rlImGuiEnd();

    EndDrawing();
}

void SimulationState::clean(std::shared_ptr<Context>& p_context) {

}

float SimulationState::findHighestYValue(const std::vector<float>& vertices) {
    float highest = -1;
    for (int i = 0; i < vertices.size(); i += 3) {
        highest = std::max(highest, vertices[i + 1]);
    }
    return highest;
}
