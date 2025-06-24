#include "Program.hpp"
#include "raylib.h"

void Program::init() {
    InitWindow(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()), "3DProdGen");
    shmgr.init();
    // TODO: init current state
}
void Program::run() {
    while (!WindowShouldClose()) {
        // TODO: call methods from currentState
    }
    CloseWindow();
}
