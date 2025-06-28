#include "StartMenu.hpp"

void StartMenu::init() {

}
void StartMenu::process() {

}
void StartMenu::draw(const Shader& sh) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawText("WELCOME TO 3DPRODGEN", 70, 300, 50, BLACK);
    EndDrawing();
}
void StartMenu::clean() {

}