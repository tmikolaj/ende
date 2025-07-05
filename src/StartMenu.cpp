#include "StartMenu.hpp"
#include "Scene.hpp"

StartMenu::StartMenu(std::shared_ptr<Context>& context) :
m_context(context),
nameBuffer{""},
showEmptyNameWarning(false) {

}

void StartMenu::init() {
    // TODO: Will later load some information
    rlImGuiSetup(true);
}

void StartMenu::process() {
    // TODO: Will later process some loaded information
}

void StartMenu::draw() {
    BeginDrawing();
    ClearBackground((Color){44, 44, 44, 255});

    rlImGuiBegin();

    const ImVec2 windowSize = ImVec2(800, 600);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowPos({0, 0});

    ImGui::Begin("Start Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::PushFont(ImGui::GetFont());
    ImGui::SetWindowFontScale(3.5f);
    ImGui::SetCursorPosX(30.0f);
    ImGui::SetCursorPosY(20.0f);
    ImGui::TextWrapped("3DProcGen - Start Menu");
    ImGui::Dummy({0, 20});
    ImGui::Separator();

    ImGui::SetWindowFontScale(1.0f);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 180));
    ImGui::SetCursorPosY(25.0f);

    ImGui::SetWindowFontScale(1.1f);
    if (ImGui::Button("Create New Project", ImVec2(160, 40))) {
        ImGui::OpenPopup("Create Project");
    }
    ImGui::SetWindowFontScale(1.0f);

    if (ImGui::BeginPopupModal("Create Project", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::SetWindowFontScale(1.6f);
        ImGui::Text("Create New Project");
        ImGui::SetWindowFontScale(1.0f);

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(50, 5));
        ImGui::SameLine();
        if (ImGui::Button("X")) {
            showEmptyNameWarning = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::Dummy(ImVec2(10, 5));
        ImGui::Text("Project Name");
        bool enterPressed = ImGui::InputText("##Project Name", nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::Dummy(ImVec2(0, 5));

        if (ImGui::Button("Create") || enterPressed) {
            if (nameBuffer[0] == '\0') {
                showEmptyNameWarning = true;
            } else {
                ImGui::CloseCurrentPopup();
                showEmptyNameWarning = false;
                m_context->states->add(std::make_unique<Scene>(m_context), true);
                m_context->states->setWindowState(RESTART);
            }
        }
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(2.5f, 0));
        ImGui::SameLine();
        if (showEmptyNameWarning) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Project Name Cannot Be Empty!");
        }

        ImGui::EndPopup();
    }

    ImGui::SetCursorPosX(30.0f);
    ImGui::SetCursorPosY(120.0f);

    // For now cause saving and loading is not implemented
    // TODO: implement loading and saving
    ImGui::SetWindowFontScale(2.0f);
    ImGui::Text("Projects: ");
    ImGui::SetWindowFontScale(1.5f);
    ImGui::SetCursorPosX(320.0f);
    ImGui::SetCursorPosY(250.0f);
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No projects yet!");
    ImGui::Spacing();

    ImGui::PopFont();

    ImGui::End();

    rlImGuiEnd();

    EndDrawing();
}

void StartMenu::clean() {

}