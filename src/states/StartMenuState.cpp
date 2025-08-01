#include "StartMenuState.hpp"
#include "SceneEditorState.hpp"

#include <fstream>
#include <iostream>

#include "../ui/StyleManager.hpp"

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

    Image logo = LoadImage("../assets/icons/ende-logo-icon.png");
    endelogo = LoadTextureFromImage(logo);
    UnloadImage(logo);

    Image i1 = LoadImage("../assets/icons/buttons/styles/ende-purple.png");
    Image i2 = LoadImage("../assets/icons/buttons/styles/ende-blue.png");
    Image i3 = LoadImage("../assets/icons/buttons/styles/ende-monochrome.png");
    Image i4 = LoadImage("../assets/icons/buttons/styles/ende-warm.png");

    endepurpleTex = LoadTextureFromImage(i1);
    endeblueTex = LoadTextureFromImage(i2);
    endemonochromeTex = LoadTextureFromImage(i3);
    endewarmTex = LoadTextureFromImage(i4);

    UnloadImage(i1);
    UnloadImage(i2);
    UnloadImage(i3);
    UnloadImage(i4);
}

void StartMenuState::process(std::shared_ptr<Context>& m_context) {
    // TODO: Will later process some loaded information
}

void StartMenuState::draw(std::shared_ptr<Context>& m_context) {
    BeginDrawing();
    ClearBackground((Color){44, 44, 44, 255});

    static bool shouldUpdate = false;
    static int styleIndex = -1;

    if (shouldUpdate) {
        std::ifstream ifs("../endeconfig");
        if (!ifs) {
            std::cerr << "StartMenu::draw: Failed to open endeconfig file" << '\n';

        } else {
            std::string line;
            int newIndex;
            if (std::getline(ifs, line)) {
                try {
                    newIndex = std::stoi(line);
                } catch (std::invalid_argument& e) {
                    newIndex = -1;
                }

                if (styleIndex == -1) {
                    StyleManager::initEndeStyle(ENDE_PURPLE);
                }
                if (newIndex != styleIndex && (newIndex >= 0 && newIndex < 4)) {
                    styleIndex = newIndex;
                    StyleManager::initEndeStyle(styleIndex);
                }
            }
        }
        ifs.close();
    }

    rlImGuiBegin();

    const ImVec2 windowSize = ImVec2(800, 600);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowPos({0, 0});

    ImGui::Begin("Start Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    rlImGuiImage(&endelogo, 100);
    ImGui::PushFont(ImGui::GetFont());
    m_context->fontMgr.setTITLE();
    ImGui::SetCursorPosX(130.0f);
    ImGui::SetCursorPosY(20.0f);
    ImGui::TextWrapped("ENDE");
    ImGui::Dummy({0, 20});
    ImGui::Separator();

    ImGui::PopFont();
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 200));
    ImGui::SetCursorPosY(15.0f);

    m_context->fontMgr.setMD();
    if (ImGui::Button("Create New Project", ImVec2(180, 40))) {
        ImGui::OpenPopup("Create Project");
    }
    ImGui::PopFont();

    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 200));
    ImGui::SetCursorPosY(70.0f);
    if (ImGui::Button("Settings", ImVec2(180, 35))) {
        ImGui::OpenPopup("Settings");
    }

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

    if (ImGui::BeginPopupModal("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove)) {
        m_context->fontMgr.setXL();
        ImGui::Dummy(ImVec2(0, 5));
        ImGui::Dummy(ImVec2(5, 0));
        ImGui::SameLine();
        ImGui::Text("Ende Settings");
        ImGui::PopFont();

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(60, 0));
        ImGui::SameLine();
        if (ImGui::Button("X")) {
            showEmptyNameWarning = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::Dummy(ImVec2(0, 5));
        ImGui::Dummy(ImVec2(5, 0));
        ImGui::SameLine();
        m_context->fontMgr.setLG();
        ImGui::Text("Style");
        ImGui::PopFont();

        ImGui::Dummy(ImVec2(0, 5));
        if (rlImGuiImageButton("EndePurpleStyle", &endepurpleTex, 64)) {
            if (styleIndex != ENDE_PURPLE) {
                shouldUpdate = true;
                std::ofstream ofs("../endeconfig");

                if (!ofs) {
                    std::cerr << "StartMenu::draw: Failed to open endeconfig file" << '\n';

                } else {
                    ofs << ENDE_PURPLE;
                }
            }
        }
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(10, 0));
        ImGui::SameLine();
        if (rlImGuiImageButton("EndeBlueStyle", &endeblueTex, 64)) {
            if (styleIndex != ENDE_BLUE) {
                shouldUpdate = true;
                std::ofstream ofs("../endeconfig");

                if (!ofs) {
                    std::cerr << "StartMenu::draw: Failed to open endeconfig file" << '\n';

                } else {
                    ofs << ENDE_BLUE;
                }
            }
        }
        if (rlImGuiImageButton("EndeMonochromeStyle", &endemonochromeTex, 64)) {
            if (styleIndex != ENDE_MONOCHROME) {
                shouldUpdate = true;
                std::ofstream ofs("../endeconfig");

                if (!ofs) {
                    std::cerr << "StartMenu::draw: Failed to open endeconfig file" << '\n';

                } else {
                    ofs << ENDE_MONOCHROME;
                }
            }
        }
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(10, 0));
        ImGui::SameLine();
        if (rlImGuiImageButton("EndeWarmStyle", &endewarmTex, 64)) {
            if (styleIndex != ENDE_WARM) {
                shouldUpdate = true;
                std::ofstream ofs("../endeconfig");

                if (!ofs) {
                    std::cerr << "StartMenu::draw: Failed to open endeconfig file" << '\n';

                } else {
                    ofs << ENDE_WARM;
                }
            }
        }
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
    UnloadTexture(endelogo);

    UnloadTexture(endepurpleTex);
    UnloadTexture(endeblueTex);
    UnloadTexture(endemonochromeTex);
    UnloadTexture(endewarmTex);
}