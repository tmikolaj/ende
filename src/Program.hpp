#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "bStateTemplate.hpp"
#include "bEngineComponent.hpp"
#include "ShaderManager.hpp"
#include "StateManager.hpp"
#include "MainMenu.hpp"
#include "Entity.hpp"

struct Context {
    // TODO:
    std::unique_ptr<Engine::StateManager> states;
    std::unique_ptr<std::vector<Entity>> entities;

    Context() {
        states = std::make_unique<Engine::StateManager>();
        entities = std::make_unique<std::vector<Entity>>();
    }
};

class Program {
private:
    std::shared_ptr<Context> context;
    Engine::ShaderManager shmgr;
public:
    Program();
    ~Program() = default;

    void init();
    void run();
};

#endif //PROGRAM_HPP
