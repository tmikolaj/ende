#include "Program.hpp"

Program::Program() :
context(std::make_shared<Context>()) {

}
void Program::init() {
    InitWindow(800, 600, "3DProcGen");
    context->states->setContext(context);
    context->states->setWindowState(NONE);
    rlImGuiSetup(true);
    context->fontMgr.init();
    context->states->add(std::make_unique<StartMenu>());
    context->states->processState();
    context->states->getCurrentState()->init(context);
}
void Program::run() {
    int w_state = context->states->getWindowState();
    while (true) {
        while (!WindowShouldClose()) {
            w_state = context->states->getWindowState();
            if (w_state == EXIT) {
                context->states->getCurrentState()->clean(context);
                exit(0);
            }
            context->states->processState();
            context->states->getCurrentState()->process(context);
            context->states->getCurrentState()->draw(context);
        }
        if (WindowShouldClose()) {
            context->states->getCurrentState()->clean(context);
            exit(0);
        }
    }
}
