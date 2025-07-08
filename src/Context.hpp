#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "StateManager.hpp"
#include "Entities/Entity.hpp"

enum WindowState {
    NONE = 0,
    RESTART = 1,
    EXIT = 2
};

struct Context {
    // TODO:
    std::unique_ptr<Engine::StateManager> states;
    std::vector<std::unique_ptr<Entity>> entities;

    Context() : entities(),
    states(std::make_unique<Engine::StateManager>()) {

    }
};

#endif // CONTEXT_HPP