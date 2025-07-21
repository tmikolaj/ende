#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP

#include <stack>
#include <memory>
#include "States/bStateTemplate.hpp"

enum stateIndex {
    STARTMENU = 0,
    SCENE = 1,
    ENTITYPAINT = 2,
    SIMULATION = 3,
};

namespace Engine {

class StateManager {
private:
    std::stack<std::unique_ptr<bStateTemplate>> stateStack;
    std::unique_ptr<bStateTemplate> newState;

    std::weak_ptr<Context> context;

    bool m_add;
    bool m_replace;
    bool m_remove;

    int windowStatus;
public:
    StateManager();
    ~StateManager() = default;

    void setContext(std::weak_ptr<Context> ctx);

    void add(std::unique_ptr<bStateTemplate> state, bool replace = false);
    void popCurrent();
    void processState();
    std::unique_ptr<bStateTemplate>& getCurrentState();
    void requestStateChange(int stateIndex, bool replace = false);

    void setWindowState(int status);
    int getWindowState();
};

}

#endif //STATEMANAGER_HPP
