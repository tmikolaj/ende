#include <memory>

#include "Application.hpp"

#include "../states/StartMenuState.hpp"

Application::Application() :
context(std::make_shared<Context>()) {

}
void Application::init() {
    InitWindow(800, 600, "3DProcGen");

    context->states->setContext(context);
    context->states->setWindowState(NONE);

    rlImGuiSetup(true);

    context->fontMgr.init();

    context->states->add(std::make_unique<StartMenuState>());
    context->states->processState();

    context->shaders->init();

    context->camera->position = { 5, 0, 5 };
    context->camera->target = { 0, 0, 0 };
    context->camera->up = { 0, 1, 0 };
    context->camera->fovy = 45.0f;
    context->camera->projection = CAMERA_PERSPECTIVE;
}
void Application::run() {
    int w_state = context->states->getWindowState();
    while (true) {
        while (!WindowShouldClose()) {
            w_state = context->states->getWindowState();
            if (w_state == EXIT) {
                context->states->getCurrentState()->clean(context);
                context->shaders->clean();
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
            ImGui::DestroyContext();
            exit(0);
        }
    }
}
