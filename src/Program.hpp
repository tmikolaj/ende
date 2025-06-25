#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <stack>
#include <memory>
#include <list>
#include "bStateTemplate.hpp"
#include "bEngineComponent.hpp"
#include "ShaderManager.hpp"

struct Context {
    // TODO:
    std::unique_ptr<Engine::bStateTemplate> states;
    std::unique_ptr<std::list<Model*>> models;

    Context() {
        states = std::make_unique<Engine::bStateTemplate>();
    }
};

class Program {
private:
    std::shared_ptr<Context> context;
    Engine::ShaderManager shmgr;
public:
    Program() = default;
    ~Program() = default;

    void init();
    void run();
};

#endif //PROGRAM_HPP
