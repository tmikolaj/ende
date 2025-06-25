#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <list>
#include "bStateTemplate.hpp"
#include "bEngineComponent.hpp"
#include "ShaderManager.hpp"
#include "StateManager.hpp"
#include "MainMenu.hpp"

struct Context {
    // TODO:
    std::unique_ptr<Engine::StateManager> states;
    std::unique_ptr<std::list<Model*>> models;

    Context() {
        states = std::make_unique<Engine::StateManager>();
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
