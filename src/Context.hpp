#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "StateManager.hpp"
#include "Entities/Entity.hpp"
#include "lLight.hpp"
#include "FontManager.hpp"
#include "AppUI.hpp"

enum WindowState {
    NONE = 0,
    RESTART = 1,
    EXIT = 2
};

struct Context {
    // TODO:
    std::unique_ptr<Engine::StateManager> states;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<lLight>> llights;
    Engine::FontManager fontMgr;
    std::unique_ptr<AppUI> ui;

    Context() : states(std::make_unique<Engine::StateManager>()),
    entities(),
    ui(std::make_unique<AppUI>()) {
    }
};

#endif // CONTEXT_HPP