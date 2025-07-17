#include "Program.hpp"

Program::Program() :
context(std::make_shared<Context>()) {

}
void Program::init() {
    InitWindow(800, 600, "3DProcGen");
    context->states->setWindowState(NONE);
    context->states->add(std::make_unique<StartMenu>(context));
    context->states->processState();
    context->states->getCurrentState()->init();
}
void Program::run() {
    int w_state = context->states->getWindowState();
    while (true) {
        while (!WindowShouldClose()) {
            w_state = context->states->getWindowState();
            if (w_state == EXIT) {
                context->states->getCurrentState()->clean();
                exit(0);
            }
            context->states->processState();
            context->states->getCurrentState()->process();
            context->states->getCurrentState()->draw();
        }
        if (WindowShouldClose()) {
            context->states->getCurrentState()->clean();
            exit(0);
        }
    }
}
