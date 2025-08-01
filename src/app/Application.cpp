#include <memory>
#include <fstream>
#include <iostream>

#include "Application.hpp"

#include "../states/StartMenuState.hpp"
#include "../ui/StyleManager.hpp"

Application::Application() :
context(std::make_shared<Context>()) {

}
void Application::init() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(800, 600, "3DProcGen");
    SetExitKey(KEY_NULL);

    Image endelogo = LoadImage("../assets/icons/ende-logo-icon.png");
    SetWindowIcon(endelogo);
    UnloadImage(endelogo);

    context->states->setContext(context);
    context->states->setWindowState(NONE);

    rlImGuiSetup(true);

    context->fontMgr.init();

    std::ifstream file("../endeconfig");
    int styleIndex;
    std::string line;
    if (!file) {
        std::cerr << "Application::init: Failed to open endeconfig file" << '\n';
        styleIndex = ENDE_PURPLE;

    } else {
        if (std::getline(file, line)) {
            try {
                styleIndex = std::stoi(line);
            } catch (std::invalid_argument& e) {
                styleIndex = ENDE_PURPLE;
            }
        }
    }

    StyleManager::initEndeStyle(styleIndex);

    context->states->add(std::make_unique<StartMenuState>());
    context->states->processState();

    context->shaders->init();

    context->customCamera->init();
}
void Application::run() {
    int w_state = context->states->getWindowState();
    while (true) {
        while (!WindowShouldClose()) {
            w_state = context->states->getWindowState();
            if (w_state == EXIT) {
                context->states->getCurrentState()->clean(context);
                context->shaders->clean();
                context->states->clean();
                ImGui::DestroyContext();
                exit(0);
            }
            context->states->processState();
            context->states->getCurrentState()->process(context);
            context->states->getCurrentState()->draw(context);
        }
        if (WindowShouldClose()) {
            context->states->getCurrentState()->clean(context);
            context->shaders->clean();
            context->states->clean();
            ImGui::DestroyContext();
            exit(0);
        }
    }
}
