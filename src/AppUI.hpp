#ifndef APPUI_HPP
#define APPUI_HPP

#include <memory>
#include "StateManager.hpp"

enum DrawMainMenuBarCodes {

    ADD_TERRAIN = 1,
    ADD_ROCK = 2,
    ADD_DIRECTIONAL = 3,
    ADD_POINT = 4,
    OPEN_SETTINGS = 5
};

class AppUI {
public:
    AppUI() = default;
    ~AppUI() = default;

    int DrawMainMenuBar(std::shared_ptr<Context>& _m_context, int& currentSh);
    void DrawStateBar(std::shared_ptr<Context>& _m_context, int& currentSh, int stateIndex);
};

#endif //APPUI_HPP
