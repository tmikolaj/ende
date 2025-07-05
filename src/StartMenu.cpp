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

    if (ImGui::IsPopupOpen("Create Project")) {
        ImVec2 center = ImVec2((800 - 300) * 0.5f, (600 - 200) * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);
        ImGui::SetNextWindowSize({300, 120});
    }

    if (ImGui::BeginPopupModal("Create Project", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::SetWindowFontScale(1.6f);
        ImGui::Text("Create New Project");
        ImGui::Dummy({50, 20});

        ImGui::SetWindowFontScale(1.0f);
        ImGui::PushItemWidth(180);
        ImGui::Text("Project Name");
        ImGui::SameLine();
        ImGui::SetKeyboardFocusHere();
        bool enterPressed = ImGui::InputText("##", nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
        ImGui::PopItemWidth();

        ImGui::SetCursorPosX(270.0f);
        ImGui::SetCursorPosY(10.0f);
        if (ImGui::Button("X", {20, 20})) {
            showEmptyNameWarning = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetCursorPosX(10.0f);
        ImGui::SetCursorPosY(85.0f);
        if (ImGui::Button("Create") || enterPressed) {
            if (nameBuffer[0] == '\0') {
                showEmptyNameWarning = true;
            } else {
                showEmptyNameWarning = false;
                projectName = nameBuffer;
                m_context->states->setWindowState(RESTART);
                m_context->states->add(std::make_unique<Scene>(m_context), true);
            }
        }
        if (showEmptyNameWarning) {
            ImGui::SetCursorPosX(80.0f);
            ImGui::SetCursorPosY(87.0f);
            ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, "Project name cannot be empty!");
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