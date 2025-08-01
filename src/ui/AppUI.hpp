#ifndef APPUI_HPP
#define APPUI_HPP

#include <memory>

#include "../app/StateController.hpp"

enum DrawMainMenuBarCodes {

    ADD_TERRAIN = 1,
    ADD_ROCK = 2,
    ADD_DIRECTIONAL = 3,
    ADD_POINT = 4,
    OPEN_SETTINGS = 5,
    OPEN_EXPORTER = 6
};

class AppUI {
public:
    AppUI() = default;
    ~AppUI() = default;

    static int DrawMainMenuBar(std::shared_ptr<Context>& p_context, int& currentSh);
    static void DrawStateBar(std::shared_ptr<Context>& p_context, int& currentSh, int stateIndex);
};

#endif //APPUI_HPP
