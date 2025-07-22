#include "AppUI.hpp"
#include "imgui.h"
#include "Context.hpp"

int AppUI::DrawMainMenuBar(std::shared_ptr<Context>& _m_context, int& currentSh) {
    int code = -1;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Close Project")) {
                _m_context->states->requestStateChange(STARTMENU);
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

void AppUI::DrawStateBar(std::shared_ptr<Context>& _m_context, int& currentSh, int stateIndex) {
    float menuHeight = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(ImVec2(0, menuHeight));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - 400, menuHeight));

    ImGui::Begin("StateTab", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings);

    if (ImGui::BeginTabBar("StateTabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("Scene")) {

            if (stateIndex != SCENE && !_m_context->states->isChangePending()) {
                _m_context->states->requestStateChange(SCENE, true);
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Entity Paint")) {

            if (stateIndex != ENTITYPAINT && !_m_context->states->isChangePending()) {
                _m_context->states->requestStateChange(ENTITYPAINT, false, 1);
                currentSh = 1;
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Simulation")) {

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
