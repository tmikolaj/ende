#include "AppUI.hpp"

#include "imgui.h"

#include "../core/Context.hpp"

int AppUI::DrawMainMenuBar(std::shared_ptr<Context>& p_context, int& currentSh) {
    int code = -1;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Close Project")) {
                p_context->states->requestStateChange(STARTMENU);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Preferences")) {
                code = OPEN_SETTINGS;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::BeginMenu("Change Shader")) {
                if (ImGui::MenuItem("Solid", nullptr, currentSh == 0)) {
                    currentSh = 0;
                }
                if (ImGui::MenuItem("Material Preview", nullptr, currentSh == 1)) {
                    currentSh = 1;
                }
                if (ImGui::MenuItem("Render", nullptr, currentSh == 2)) {
                    currentSh = 2;
                }
                if (ImGui::MenuItem("Wireframe", nullptr, currentSh == 3)) {
                    currentSh = 3;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Add")) {
            if (ImGui::BeginMenu("Light")) {

                if (ImGui::MenuItem("Point")) code = ADD_POINT;
                if (ImGui::MenuItem("Directional")) code = ADD_DIRECTIONAL;

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Entity")) {

                if (ImGui::MenuItem("Terrain")) code = ADD_TERRAIN;
                if (ImGui::MenuItem("Rock")) code = ADD_ROCK;

                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    return code;
}

void AppUI::DrawStateBar(std::shared_ptr<Context>& p_context, int& currentSh, int stateIndex) {
    float menuHeight = ImGui::GetFrameHeight();
    static float startEntityPaintHover = 0.0f;
    static float startSimulationHover = 0.0f;

    bool shouldDisable = !(p_context->selectedEntity >= 0 && p_context->selectedEntity < p_context->entities.size());

    ImGui::SetNextWindowPos(ImVec2(0, menuHeight));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - 400, menuHeight));

    ImGui::Begin("StateTab", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings);

    if (ImGui::BeginTabBar("StateTabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("Scene")) {

            if (stateIndex != SCENE && !p_context->states->isChangePending()) {
                p_context->states->popCurrent();
                p_context->states->processState();
            }
            ImGui::EndTabItem();
        }
    }
    if (shouldDisable) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    }

    bool entityPaintClicked = ImGui::BeginTabItem("Entity Paint");

    if (shouldDisable && ImGui::IsItemHovered()) {
        p_context->uiManager->SetItemTooltip("You need to select an Entity first to enter this state!", startEntityPaintHover, 0.0f);
    }

    if (entityPaintClicked) {
        if (!shouldDisable && stateIndex != ENTITYPAINT && !p_context->states->isChangePending()) {
            p_context->states->requestStateChange(ENTITYPAINT, false, 1);
            currentSh = 1;
        }
        ImGui::EndTabItem();
    }

    bool simulationClicked = ImGui::BeginTabItem("Simulation");

    if (shouldDisable && ImGui::IsItemHovered()) {
        p_context->uiManager->SetItemTooltip("You need to select an Entity first to enter this state!", startSimulationHover, 0.0f);
    }

    if (simulationClicked) {
        if (!shouldDisable && stateIndex != SIMULATION && !p_context->states->isChangePending()) {
            p_context->states->requestStateChange(SIMULATION, false);
        }
        ImGui::EndTabItem();
    }

    if (shouldDisable) {
        ImGui::PopStyleColor();
    }

    ImGui::EndTabBar();

    ImGui::End();
}
