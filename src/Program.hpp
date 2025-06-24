#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <stack>
#include <memory>
#include "ShaderManager.hpp"

struct Context {
    // TODO:
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
