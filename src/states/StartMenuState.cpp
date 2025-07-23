#include "StartMenuState.hpp"
#include "SceneEditorState.hpp"

StartMenuState::StartMenuState() :
nameBuffer{""},
showEmptyNameWarning(false),
seedBuffer("") {

}

void StartMenuState::init(std::shared_ptr<Context>& m_context) {
    // TODO: Will later load some information
    SetWindowSize(800, 600);

    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    int posX = (screenWidth - 800) / 2;
    int posY = (screenHeight - 600) / 2;

    SetWindowPosition(posX, posY);
}

void StartMenuState::process(std::shared_ptr<Context>& m_context) {
    // TODO: Will later process some loaded information
}

void StartMenuState::draw(std::shared_ptr<Context>& m_context) {
    BeginDrawing();
    ClearBackground((Color){44, 44, 44, 255});

    rlImGuiBegin();

    const ImVec2 windowSize = ImVec2(800, 600);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowPos({0, 0});

    ImGui::Begin("Start Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::PushFont(ImGui::GetFont());
    m_context->fontMgr.setTITLE();
    ImGui::SetCursorPosX(30.0f);
    ImGui::SetCursorPosY(20.0f);
    ImGui::TextWrapped("3DProcGen - Start Menu");
    ImGui::Dummy({0, 20});
    ImGui::Separator();

    ImGui::PopFont();
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 200));
    ImGui::SetCursorPosY(25.0f);

    m_context->fontMgr.setMD();
    if (ImGui::Button("Create New Project", ImVec2(180, 40))) {
        ImGui::OpenPopup("Create Project");
    }
    ImGui::PopFont();

    if (ImGui::BeginPopupModal("Create Project", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        m_context->fontMgr.setLG();
        ImGui::Dummy(ImVec2(0, 5));
        ImGui::Dummy(ImVec2(10, 0));
        ImGui::SameLine();
        ImGui::Text("Create New Project");
        ImGui::PopFont();

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(60, 0));
        ImGui::SameLine();
        if (ImGui::Button("X")) {
            showEmptyNameWarning = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Dummy(ImVec2(10, 0));
        ImGui::SameLine();
        m_context->fontMgr.setMD();
        ImGui::Text("Project Name");
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(10, 0));
        ImGui::SameLine();
        m_context->fontMgr.setSM();
        ImGui::PushItemWidth(222);
        bool enterPressed = ImGui::InputText("##Project Name", nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(0, 5));

        ImGui::Dummy(ImVec2(10, 0));
        ImGui::SameLine();
        m_context->fontMgr.setMD();
        ImGui::Text("Seed");
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(10, 0));
        ImGui::SameLine();
        m_context->fontMgr.setSM();
        ImGui::InputText("##Seed", seedBuffer, IM_ARRAYSIZE(seedBuffer));
        ImGui::PopItemWidth();
        ImGui::PopFont();

        static float hoverSeed = 0.0f;
        float delay = 1.0f;
        if (ImGui::IsItemHovered() && !ImGui::IsItemActive()) {
            if (hoverSeed == 0.0f) hoverSeed = ImGui::GetTime();

            if (ImGui::GetTime() - hoverSeed > delay) {
                ImGui::BeginTooltip();
                ImGui::Text("Seed will be used for terrain generation.");
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "If left empty random number will be used as seed!");
                ImGui::EndTooltip();
            }
        } else {
            hoverSeed = 0.0f;
        }

        ImGui::Dummy(ImVec2(0, 5));

        ImGui::Dummy(ImVec2(10, 0));
        ImGui::SameLine();
        if (ImGui::Button("Create") || enterPressed) {
            if (nameBuffer[0] == '\0') {
                showEmptyNameWarning = true;
            } else {
                ImGui::CloseCurrentPopup();
                showEmptyNameWarning = false;
                noise.init(seedBuffer);
                m_context->states->add(std::make_unique<SceneEditorState>(), true);
                m_context->states->setWindowState(RESTART);
            }
        }
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(0, 5));
        m_context->fontMgr.setSM();
        if (showEmptyNameWarning) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Project name cannot be empty!");
        }
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(0, 5));

        ImGui::EndPopup();
    }

    ImGui::SetCursorPosX(30.0f);
    ImGui::SetCursorPosY(120.0f);

    // For now cause saving and loading is not implemented
    // TODO: implement loading and saving
    m_context->fontMgr.setXL();
    ImGui::Text("Projects: ");
    ImGui::PopFont();
    m_context->fontMgr.setLG();
    ImGui::SetCursorPosX(320.0f);
    ImGui::SetCursorPosY(250.0f);
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No projects yet!");
    ImGui::Spacing();

    ImGui::PopFont();
    ImGui::PopFont();

    ImGui::End();

    rlImGuiEnd();

    EndDrawing();
}

void StartMenuState::clean(std::shared_ptr<Context>& m_context) {

}