#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "../states/BaseState.hpp"

#include "../core/Context.hpp"

class Application {
private:
    std::shared_ptr<Context> context;
public:
    Application();
    ~Application() = default;

    void init();
    void run();
};

#endif // APPLICATION_HPP
