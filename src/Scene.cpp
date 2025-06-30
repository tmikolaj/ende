#include "Scene.hpp"

Scene::Scene(std::shared_ptr<Context>& context) :
m_context(context) {

}

void Scene::init() {
    InitWindow(1920, 1080, "Scene");
    m_context->states->setWindowState(NONE);
}

void Scene::process() {

}

void Scene::draw(const Shader& sh) {
    if (WindowShouldClose()) {
        m_context->states->setWindowState(EXIT);
        return;
    }
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("WORKS!!", 820, 500, 100, BLACK);
    EndDrawing();
}

void Scene::clean() {
    CloseWindow();
}