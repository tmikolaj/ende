#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "bStateTemplate.hpp"
#include "ShaderManager.hpp"
#include "StartMenu.hpp"
#include "Context.hpp"

class Program {
private:
    std::shared_ptr<Context> context;
    Engine::ShaderManager shmgr;
    Shader shader;
public:
    Program();
    ~Program() = default;

    void init();
    void run();
};

#endif //PROGRAM_HPP
