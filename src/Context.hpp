#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "StateManager.hpp"
#include "Entity.hpp"

enum WindowState {
    NONE = 0,
    RESTART = 1,
    EXIT = 2
};

struct Context {
    // TODO:
    std::unique_ptr<Engine::StateManager> states;
    std::unique_ptr<std::vector<Entity>> entities;

    Context() {
        states = std::make_unique<Engine::StateManager>();
        entities = std::make_unique<std::vector<Entity>>();
    }
};

#endif // CONTEXT_HPP