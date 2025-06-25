#include "Program.hpp"

Program::Program() :
context(std::make_shared<Context>()) {
    context->states->add(std::make_unique<MainMenu>());
}
void Program::init() {
    InitWindow(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()), "3DProdGen");
    shmgr.init();
    context->states->processState();
    context->states->getCurrentState()->init();
}
void Program::run() {
    while (!WindowShouldClose()) {
        // TODO: get the shader before draw is called
        context->states->processState();
        context->states->getCurrentState()->process();
        // context->states->getCurrentState()->draw();
    }
    CloseWindow();
}
