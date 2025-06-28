#include "Program.hpp"

Program::Program() :
context(std::make_shared<Context>()) {

}
void Program::init() {
    InitWindow(800, 600, "3DProdGen");
    shmgr.init();
    context->states->add(std::make_unique<StartMenu>());
    context->states->processState();
    context->states->getCurrentState()->init();
}
void Program::run() {
    while (!WindowShouldClose()) {
        context->states->processState();
        context->states->getCurrentState()->process();
        context->states->getCurrentState()->draw(shmgr.set(shader));
    }
    context->states->getCurrentState()->clean();
    CloseWindow();
}
